#include "MultiMeshFromTag.hpp"

#include <queue>
#include <set>
#include <wmtk/EdgeMesh.hpp>
#include <wmtk/PointMesh.hpp>
#include <wmtk/TetMesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/simplex/cofaces_single_dimension.hpp>
#include <wmtk/simplex/faces_single_dimension.hpp>
#include <wmtk/simplex/top_dimension_cofaces.hpp>
#include <wmtk/simplex/tuples_preserving_primitive_types.hpp>
#include <wmtk/utils/Logger.hpp>
#include <wmtk/utils/primitive_range.hpp>

namespace wmtk::components::internal {

MultiMeshFromTag::MultiMeshFromTag(
    Mesh& mesh,
    const attribute::MeshAttributeHandle& tag_handle,
    const int64_t tag_value)
    : m_mesh(mesh)
    , m_tag_handle(tag_handle)
    , m_tag_value(tag_value)
    , m_tag_acc(m_mesh.create_const_accessor<int64_t>(m_tag_handle))
    , m_tag_ptype(tag_handle.primitive_type())
{
    assert(m_mesh.get_child_meshes().empty());

    create_substructure_soup();

    Mesh& child = *m_child_ptr;

    // create attributes to store new ids
    for (const PrimitiveType pt : utils::primitive_below(m_tag_ptype)) {
        if (pt == m_tag_ptype) {
            continue;
        }

        const int64_t pt_id = get_primitive_type_id(pt);

        const int64_t n_ids = m_n_local_ids[get_primitive_type_id(m_tag_ptype)][pt_id];

        m_new_id_handles[pt] = child.register_attribute<int64_t>(
            std::string("multimesh_from_tag_new_ids_") + std::to_string(pt_id),
            m_tag_ptype,
            n_ids,
            false,
            -1);
    }

    build_adjacency();
}

Eigen::MatrixX<int64_t> MultiMeshFromTag::get_new_id_matrix(const PrimitiveType ptype) const
{
    return m_new_id_matrices.at(ptype);
}

VectorXl MultiMeshFromTag::get_new_top_coface_vector(const PrimitiveType ptype) const
{
    return m_new_top_coface_vectors.at(ptype);
}

void MultiMeshFromTag::compute_substructure_ids()
{
    Mesh& child = *m_child_ptr;

    const auto top_dimension_child_tuples = child.get_all(m_tag_ptype);

    for (const PrimitiveType pt : utils::primitive_below(m_tag_ptype)) {
        if (pt == m_tag_ptype) {
            // there is nothing to do for the top dimension
            continue;
        }

        auto get_id = [pt](const attribute::Accessor<int64_t>& acc, const Tuple& t) -> int64_t {
            int64_t local_id = -1;
            switch (pt) {
            case PrimitiveType::Vertex: local_id = t.local_vid(); break;
            case PrimitiveType::Edge: local_id = t.local_eid(); break;
            case PrimitiveType::Triangle: local_id = t.local_fid(); break;
            case PrimitiveType::Tetrahedron:
            default: assert(false); break;
            }

            return acc.const_vector_attribute(t)(local_id);
        };

        auto set_id =
            [pt](attribute::Accessor<int64_t>& acc, const Tuple& t, const int64_t val) -> void {
            int64_t local_id = -1;
            switch (pt) {
            case PrimitiveType::Vertex: local_id = t.local_vid(); break;
            case PrimitiveType::Edge: local_id = t.local_eid(); break;
            case PrimitiveType::Triangle: local_id = t.local_fid(); break;
            case PrimitiveType::Tetrahedron:
            default: assert(false); break;
            }

            acc.vector_attribute(t)(local_id) = val;
        };

        auto id_acc = child.create_accessor<int64_t>(m_new_id_handles[pt]);

        int64_t simplex_counter = 0;
        // assign ids with duplication at non-manifold simplices
        for (const Tuple& cell_tuple : top_dimension_child_tuples) {
            const auto simplex_tuples = simplex::faces_single_dimension_tuples(
                child,
                simplex::Simplex(m_tag_ptype, cell_tuple),
                pt);

            for (const Tuple& s_tuple : simplex_tuples) {
                if (get_id(id_acc, s_tuple) != -1) {
                    // simplex already has an id assigned
                    continue;
                }

                const std::vector<Tuple> s_region = get_connected_region(s_tuple, pt);

                for (const Tuple& t : s_region) {
                    set_id(id_acc, t, simplex_counter);
                }

                ++simplex_counter;
            }
        }

        // build id and top-coface matrix
        Eigen::MatrixX<int64_t> id_matrix;
        id_matrix.resize(top_dimension_child_tuples.size(), id_acc.dimension());
        VectorXl coface_vector;
        coface_vector.resize(simplex_counter);


        for (size_t i = 0; i < top_dimension_child_tuples.size(); ++i) {
            const Tuple& cell_tuple = top_dimension_child_tuples[i];

            const auto face_tuples = simplex::faces_single_dimension_tuples(
                child,
                simplex::Simplex(m_tag_ptype, cell_tuple),
                pt);

            assert(face_tuples.size() == id_acc.dimension());

            for (size_t j = 0; j < face_tuples.size(); ++j) {
                const int64_t id = get_id(id_acc, face_tuples[j]);
                assert(id != -1);
                id_matrix(i, j) = id;
                coface_vector(id) = i;
            }
        }

        m_new_id_matrices[pt] = id_matrix;
        m_new_top_coface_vectors[pt] = coface_vector;
    }
}

std::vector<Tuple> MultiMeshFromTag::get_connected_region(
    const Tuple& t_in,
    const PrimitiveType ptype)
{
    Mesh& child = *m_child_ptr;

    const PrimitiveType connecting_ptype =
        get_primitive_type_from_id(get_primitive_type_id(m_tag_ptype) - 1);

    std::vector<Tuple> connected_region;

    std::set<Tuple> touched_tuples;
    std::queue<Tuple> q;
    q.push(t_in);

    while (!q.empty()) {
        const Tuple t = q.front();
        q.pop();

        {
            // check if cell already exists
            const auto [it, did_insert] = touched_tuples.insert(t);
            if (!did_insert) {
                continue;
            }
        }
        connected_region.emplace_back(t);

        const std::vector<Tuple> pt_intersection =
            simplex::tuples_preserving_primitive_types(child, t, ptype, m_tag_ptype);


        for (const Tuple& t_version : pt_intersection) {
            const simplex::Simplex face = simplex::Simplex(connecting_ptype, t_version);

            const simplex::Simplex root_face = child.map_to_parent(face);

            const std::vector<simplex::Simplex> child_faces = m_mesh.map_to_child(child, root_face);

            if (child_faces.size() != 2) {
                // the connection through this face is not manifold or a boundary
                continue;
            }

            assert(child_faces[0].tuple() == t_version || child_faces[1].tuple() == t_version);

            q.push(
                child_faces[0].tuple() == t_version ? child_faces[1].tuple()
                                                    : child_faces[0].tuple());
        }
    }

    return connected_region;
}

void MultiMeshFromTag::build_adjacency()
{
    if (m_tag_ptype == PrimitiveType::Vertex) {
        // A point mesh does not have any adjacency
        return;
    }

    Mesh& child = *m_child_ptr;

    const int64_t tag_ptype_id = get_primitive_type_id(m_tag_ptype);

    const int64_t connecting_pt_id = tag_ptype_id - 1;
    const PrimitiveType connecting_ptype = get_primitive_type_from_id(connecting_pt_id);

    auto get_id =
        [connecting_ptype](const attribute::Accessor<int64_t>& acc, const Tuple& t) -> int64_t {
        int64_t local_id = -1;
        switch (connecting_ptype) {
        case PrimitiveType::Vertex: local_id = t.local_vid(); break;
        case PrimitiveType::Edge: local_id = t.local_eid(); break;
        case PrimitiveType::Triangle: local_id = t.local_fid(); break;
        case PrimitiveType::Tetrahedron:
        default: assert(false); break;
        }

        return acc.const_vector_attribute(t)(local_id);
    };

    auto set_id = [connecting_ptype](
                      attribute::Accessor<int64_t>& acc,
                      const Tuple& t,
                      const int64_t val) -> void {
        int64_t local_id = -1;
        switch (connecting_ptype) {
        case PrimitiveType::Vertex: local_id = t.local_vid(); break;
        case PrimitiveType::Edge: local_id = t.local_eid(); break;
        case PrimitiveType::Triangle: local_id = t.local_fid(); break;
        case PrimitiveType::Tetrahedron:
        default: assert(false); break;
        }

        acc.vector_attribute(t)(local_id) = val;
    };

    m_adjacency_handle = child.register_attribute<int64_t>(
        "multimesh_from_tag_adjacency",
        m_tag_ptype,
        m_n_local_ids[tag_ptype_id][connecting_pt_id],
        false,
        -1);

    auto adj_acc = child.create_accessor<int64_t>(m_adjacency_handle);

    const auto top_dimension_child_tuples = child.get_all(m_tag_ptype);

    auto top_simplex_id_handle = child.register_attribute<int64_t>(
        "multimesh_from_tag_top_substructure_simplex_id",
        m_tag_ptype,
        1,
        false,
        -1);
    auto top_simplex_id_acc = child.create_accessor<int64_t>(top_simplex_id_handle);

    // set cell ids
    for (size_t i = 0; i < top_dimension_child_tuples.size(); ++i) {
        top_simplex_id_acc.scalar_attribute(top_dimension_child_tuples[i]) = i;
    }

    // create adjacency matrix
    for (const Tuple& cell_tuple : top_dimension_child_tuples) {
        const auto face_tuples = simplex::faces_single_dimension_tuples(
            child,
            simplex::Simplex(m_tag_ptype, cell_tuple),
            connecting_ptype);

        for (const Tuple& ft : face_tuples) {
            const simplex::Simplex face = simplex::Simplex(connecting_ptype, ft);

            const simplex::Simplex root_face = child.map_to_parent(face);

            const std::vector<simplex::Simplex> child_faces = m_mesh.map_to_child(child, root_face);

            if (child_faces.size() != 2) {
                // the connection through this face is either non-manifold or a boundary
                continue;
            }

            assert(child_faces[0].tuple() == ft || child_faces[1].tuple() == ft);

            const Tuple neighbor =
                child_faces[0].tuple() == ft ? child_faces[1].tuple() : child_faces[0].tuple();

            // set adjacency
            set_id(adj_acc, ft, top_simplex_id_acc.const_scalar_attribute(neighbor));
        }
    }

    Eigen::MatrixX<int64_t> adj_matrix;
    adj_matrix.resize(top_dimension_child_tuples.size(), m_adjacency_handle.dimension());

    for (size_t i = 0; i < top_dimension_child_tuples.size(); ++i) {
        const auto face_tuples = simplex::faces_single_dimension_tuples(
            child,
            simplex::Simplex(m_tag_ptype, top_dimension_child_tuples[i]),
            connecting_ptype);

        assert(face_tuples.size() == adj_matrix.cols());

        for (size_t j = 0; j < face_tuples.size(); ++j) {
            const int64_t id = get_id(adj_acc, face_tuples[j]);
            adj_matrix(i, j) = id;
        }
    }

    m_adjacency_matrix = adj_matrix;
}

void MultiMeshFromTag::create_substructure_soup()
{
    const int64_t n_vertices_per_simplex = m_n_local_ids[get_primitive_type_id(m_tag_ptype)][0];

    std::vector<Tuple> tagged_tuples;
    {
        const auto tag_type_tuples = m_mesh.get_all(m_tag_ptype);
        for (const Tuple& t : tag_type_tuples) {
            if (m_tag_acc.const_scalar_attribute(t) != m_tag_value) {
                continue;
            }
            tagged_tuples.emplace_back(t);
        }
    }

    // vertex matrix
    Eigen::MatrixX<int64_t> id_matrix;
    id_matrix.resize(tagged_tuples.size(), n_vertices_per_simplex);

    for (size_t i = 0; i < tagged_tuples.size(); ++i) {
        for (int64_t j = 0; j < n_vertices_per_simplex; ++j) {
            id_matrix(i, j) = n_vertices_per_simplex * i + j;
        }
    }

    switch (m_tag_ptype) {
    case PrimitiveType::Vertex: {
        m_child_ptr = std ::make_shared<PointMesh>();
        static_cast<PointMesh&>(*m_child_ptr).initialize(tagged_tuples.size());
        break;
    }
    case PrimitiveType::Edge: {
        m_child_ptr = std ::make_shared<EdgeMesh>();
        static_cast<EdgeMesh&>(*m_child_ptr).initialize(id_matrix);
        break;
    }
    case PrimitiveType::Triangle: {
        m_child_ptr = std ::make_shared<TriMesh>();
        static_cast<TriMesh&>(*m_child_ptr).initialize(id_matrix);
        break;
    }
    case PrimitiveType::Tetrahedron: {
        m_child_ptr = std ::make_shared<TetMesh>();
        static_cast<TetMesh&>(*m_child_ptr).initialize(id_matrix);
        break;
    }
    default: log_and_throw_error("Unknown primitive type for tag");
    }

    std::vector<std::array<Tuple, 2>> child_to_parent_map(tagged_tuples.size());

    const auto child_top_dimension_tuples = m_child_ptr->get_all(m_tag_ptype);

    assert(tagged_tuples.size() == child_top_dimension_tuples.size());

    for (size_t i = 0; i < tagged_tuples.size(); ++i) {
        child_to_parent_map[i] = {{child_top_dimension_tuples[i], tagged_tuples[i]}};
    }

    m_mesh.register_child_mesh(m_child_ptr, child_to_parent_map);
}

std::shared_ptr<Mesh> MultiMeshFromTag::substructure_soup() const
{
    return m_child_ptr;
}

} // namespace wmtk::components::internal
