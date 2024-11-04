

#pragma once

#include <wmtk/attribute/MeshAttributeHandle.hpp>
#include "Invariant.hpp"

namespace wmtk {
namespace invariants {
class MinEdgeLengthInvariant : public Invariant
{
public:
    // NOTE: this takes in the threshold squared rather than the threshold itself
    MinEdgeLengthInvariant(
        const Mesh& m,
        const TypedAttributeHandle<double>& coordinate,
        double threshold_squared);
    bool before(const simplex::Simplex& t) const override;

private:
    const TypedAttributeHandle<double> m_coordinate_handle;
    double m_threshold_squared;
};
} // namespace invariants

using MinEdgeLengthInvariant = invariants::MinEdgeLengthInvariant;
} // namespace wmtk
