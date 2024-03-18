#pragma once

#include <wmtk/attribute/TypedAttributeHandle.hpp>
#include "Invariant.hpp"

namespace wmtk {
class Swap23EnergyBeforeInvariant : public Invariant
{
public:
    Swap23EnergyBeforeInvariant(const Mesh& m, const attribute::TypedAttributeHandle<double>& coordinate);
    using Invariant::Invariant;

    bool before(const simplex::Simplex& t) const override;

private:
    const attribute::TypedAttributeHandle<double> m_coordinate_handle;
};
} // namespace wmtk
