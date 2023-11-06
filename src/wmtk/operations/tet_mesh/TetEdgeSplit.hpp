#pragma once
#include <optional>
#include <wmtk/TetMesh.hpp>
#include <wmtk/invariants/InvariantCollection.hpp>
#include <wmtk/operations/TupleOperation.hpp>
#include "TetMeshOperation.hpp"

namespace wmtk::operations {
namespace tet_mesh {
class TetEdgeSplit;
}

template <>
struct OperationSettings<tet_mesh::TetEdgeSplit>
{
    bool split_boundary_edges = true;
    InvariantCollection invariants;

    void initialize_invariants(const TetMesh& m);
    // debug functionality to make sure operations are constructed properly
    bool are_invariants_initialized() const;
};

namespace tet_mesh {
class TetEdgeSplit : public TetMeshOperation, private TupleOperation
{
public:
    TetEdgeSplit(Mesh& m, const Tuple& t, const OperationSettings<TetEdgeSplit>& settings);
    TetEdgeSplit(TetMesh& m, const Tuple& t, const OperationSettings<TetEdgeSplit>& settings);
    //~TetEdgeSplit();

    std::string name() const override;

    Tuple new_vertex() const;
    Tuple return_tuple() const;
    std::vector<Tuple> modified_primitives(PrimitiveType) const override;
    // std::vector<Tuple> new_triangles() const ;
    // std::vector<Tuple> new_edges() const ;

    // std::array<Tuple,2> spline_edges() const;
    // std::vector<Tuple> rib_edges() const;

    static PrimitiveType primitive_type() { return PrimitiveType::Edge; }

    using TetMeshOperation::hash_accessor;

protected:
    bool execute() override;

private:
    Tuple m_output_tuple;

    const OperationSettings<TetEdgeSplit>& m_settings;
};

} // namespace tet_mesh
} // namespace wmtk::operations
