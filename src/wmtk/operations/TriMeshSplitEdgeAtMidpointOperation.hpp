#pragma once
#include <wmtk/TriMesh.hpp>
#include "Operation.hpp"

namespace wmtk::operations {
namespace tri_mesh {
class TriMeshEdgeSplitAtMidpoint;
}

template <>
struct OperationSettings<tri_mesh::TriMeshEdgeSplitAtMidpoint>
{
    MeshAttributeHandle<double> position;
    double min_squared_length = -1;
    bool split_boundary_edges = true;
};

namespace tri_mesh {
class TriMeshEdgeSplitAtMidpoint : public Operation
{
public:
    TriMeshEdgeSplitAtMidpoint(
        wmtk::Mesh& m,
        const Tuple& t,
        const OperationSettings<TriMeshEdgeSplitAtMidpoint>& settings);

    std::string name() const override;

    Tuple return_tuple() const;

    static PrimitiveType primitive_type() { return PrimitiveType::Edge; }

protected:
    bool before() const override;
    bool execute() override;

private:
    Tuple m_input_tuple;
    Tuple m_output_tuple;
    Accessor<double> m_pos_accessor;

    const OperationSettings<TriMeshEdgeSplitAtMidpoint>& m_settings;

    Eigen::Vector3d p0;
    Eigen::Vector3d p1;
};

} // namespace tri_mesh
} // namespace wmtk::operations
