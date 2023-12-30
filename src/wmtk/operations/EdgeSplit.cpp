#include "EdgeSplit.hpp"

#include <wmtk/EdgeMesh.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/TetMesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/operations/tet_mesh/EdgeOperationData.hpp>
#include <wmtk/utils/Logger.hpp>

#include <wmtk/utils/Logger.hpp>

#include "tri_mesh/BasicSplitNewAttributeStrategy.hpp"
#include "tri_mesh/PredicateAwareSplitNewAttributeStrategy.hpp"
#include "utils/multi_mesh_edge_split.hpp"

namespace wmtk::operations {

EdgeSplit::EdgeSplit(Mesh& m)
    : MeshOperation(m)
{
    // PredicateAwareSplitNewAttributeStrategy BasicSplitNewAttributeStrategy

    const int top_cell_dimension = m.top_cell_dimension();

    for (const auto& attr : m.attributes()) {
        std::visit(
            [&](auto&& val) {
                using T = typename std::decay_t<decltype(val)>::Type;

                if (top_cell_dimension == 2)
                    m_new_attr_strategies.emplace_back(
                        std::make_shared<operations::tri_mesh::BasicSplitNewAttributeStrategy<T>>(
                            val));
                else {
                    throw std::runtime_error("collapse not implemented for edge/tet mesh");
                }
            },
            attr);

        m_new_attr_strategies.back()->update_handle_mesh(m);
    }
}

///////////////////////////////
std::vector<Simplex> EdgeSplit::execute(EdgeMesh& mesh, const Simplex& simplex)
{
    auto return_data = utils::multi_mesh_edge_split(mesh, simplex.tuple(), m_new_attr_strategies);

    const edge_mesh::EdgeOperationData& my_data = return_data.get(mesh, simplex);

    return {simplex::Simplex::vertex(my_data.m_output_tuple)};
}

std::vector<Simplex> EdgeSplit::unmodified_primitives(const EdgeMesh& mesh, const Simplex& simplex)
    const
{
    return {simplex};
}
///////////////////////////////


///////////////////////////////
std::vector<Simplex> EdgeSplit::execute(TriMesh& mesh, const Simplex& simplex)
{
    auto return_data = utils::multi_mesh_edge_split(mesh, simplex.tuple(), m_new_attr_strategies);

    const tri_mesh::EdgeOperationData& my_data = return_data.get(mesh, simplex);

    return {simplex::Simplex::vertex(my_data.m_output_tuple)};
}

std::vector<Simplex> EdgeSplit::unmodified_primitives(const TriMesh& mesh, const Simplex& simplex)
    const
{
    return {simplex};
}
///////////////////////////////


///////////////////////////////
std::vector<Simplex> EdgeSplit::execute(TetMesh& mesh, const Simplex& simplex)
{
    auto return_data = utils::multi_mesh_edge_split(mesh, simplex.tuple(), m_new_attr_strategies);

    wmtk::logger().trace("{}", primitive_type_name(simplex.primitive_type()));

    const tet_mesh::EdgeOperationData& my_data = return_data.get(mesh, simplex);

    return {simplex::Simplex::vertex(my_data.m_output_tuple)};
}

std::vector<Simplex> EdgeSplit::unmodified_primitives(const TetMesh& mesh, const Simplex& simplex)
    const
{
    return {simplex};
}
///////////////////////////////


void EdgeSplit::set_standard_strategy(
    const attribute::MeshAttributeHandleVariant& attribute,
    const wmtk::operations::NewAttributeStrategy::SplitBasicStrategy& spine,
    const wmtk::operations::NewAttributeStrategy::SplitRibBasicStrategy& rib)
{
    std::visit(
        [&](auto&& val) -> void {
            using T = typename std::decay_t<decltype(val)>::Type;
            using PASNAS = operations::tri_mesh::PredicateAwareSplitNewAttributeStrategy<T>;

            std::shared_ptr<PASNAS> tmp = std::make_shared<PASNAS>(val, mesh());
            tmp->set_standard_split_strategy(spine);
            tmp->set_standard_split_rib_strategy(rib);

            set_strategy(attribute, tmp);
        },
        attribute);
}

std::pair<Tuple, Tuple> EdgeSplit::new_spine_edges(const Mesh& mesh, const Tuple& new_vertex)
{
    // new_vertex is a spine edge on a face pointing to the new vertex, so we
    // * PE -> new edge
    // * PF -> other face
    // * PE -> other spine edge
    constexpr static PrimitiveType PE = PrimitiveType::Edge;
    constexpr static PrimitiveType PF = PrimitiveType::Face;
    constexpr static PrimitiveType PT = PrimitiveType::Tetrahedron;

    std::pair<Tuple, Tuple> ret;

    switch (mesh.top_simplex_type()) {
    case PE: {
        ret = {new_vertex, mesh.switch_tuples(new_vertex, {PE})};
        break;
    }
    case PF: {
        ret = {new_vertex, mesh.switch_tuples(new_vertex, {PE, PF, PE})};
        break;
    }
    case PT: {
        ret = {new_vertex, mesh.switch_tuples(new_vertex, {PE, PF, PT, PF, PE})};
    }
    }
    return ret;
}


} // namespace wmtk::operations
