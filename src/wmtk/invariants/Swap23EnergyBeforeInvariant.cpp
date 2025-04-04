#include "Swap23EnergyBeforeInvariant.hpp"
#include <wmtk/Mesh.hpp>
#include <wmtk/function/utils/amips.hpp>
#include <wmtk/utils/orient.hpp>

namespace wmtk {
Swap23EnergyBeforeInvariant::Swap23EnergyBeforeInvariant(
    const Mesh& m,
    const attribute::TypedAttributeHandle<Rational>& coordinate)
    : Invariant(m, true, false, false)
    , m_coordinate_handle(coordinate)
{}

bool Swap23EnergyBeforeInvariant::before(const simplex::Simplex& t) const
{
    constexpr static PrimitiveType PV = PrimitiveType::Vertex;
    constexpr static PrimitiveType PE = PrimitiveType::Edge;
    constexpr static PrimitiveType PF = PrimitiveType::Triangle;
    constexpr static PrimitiveType PT = PrimitiveType::Tetrahedron;

    auto accessor = mesh().create_const_accessor(m_coordinate_handle);

    // get the coords of the vertices
    // input face end points
    const Tuple v0 = t.tuple();
    const Tuple v1 = mesh().switch_tuple(v0, PV);
    const Tuple v2 = mesh().switch_tuples(v0, {PE, PV});
    // other 2 vertices
    const Tuple v3 = mesh().switch_tuples(v0, {PF, PE, PV});
    const Tuple v4 = mesh().switch_tuples(v0, {PT, PF, PE, PV});


    std::array<Eigen::Vector3<Rational>, 5> positions = {
        {accessor.const_vector_attribute(v0),
         accessor.const_vector_attribute(v1),
         accessor.const_vector_attribute(v2),
         accessor.const_vector_attribute(v3),
         accessor.const_vector_attribute(v4)}};
    std::array<Eigen::Vector3d, 5> positions_double = {
        {accessor.const_vector_attribute(v0).cast<double>(),
         accessor.const_vector_attribute(v1).cast<double>(),
         accessor.const_vector_attribute(v2).cast<double>(),
         accessor.const_vector_attribute(v3).cast<double>(),
         accessor.const_vector_attribute(v4).cast<double>()}};

    std::array<std::array<int, 4>, 2> old_tets = {{{{0, 1, 2, 3}}, {{0, 1, 2, 4}}}};
    std::array<std::array<int, 4>, 3> new_tets = {{{{3, 4, 0, 1}}, {{3, 4, 1, 2}}, {{3, 4, 2, 0}}}};

    double old_energy_max = std::numeric_limits<double>::lowest();
    double new_energy_max = std::numeric_limits<double>::lowest();

    for (int i = 0; i < 2; ++i) {
        if (utils::wmtk_orient3d(
                positions[old_tets[i][0]],
                positions[old_tets[i][1]],
                positions[old_tets[i][2]],
                positions[old_tets[i][3]]) > 0) {
            auto energy = wmtk::function::utils::Tet_AMIPS_energy({{
                positions_double[old_tets[i][0]][0],
                positions_double[old_tets[i][0]][1],
                positions_double[old_tets[i][0]][2],
                positions_double[old_tets[i][1]][0],
                positions_double[old_tets[i][1]][1],
                positions_double[old_tets[i][1]][2],
                positions_double[old_tets[i][2]][0],
                positions_double[old_tets[i][2]][1],
                positions_double[old_tets[i][2]][2],
                positions_double[old_tets[i][3]][0],
                positions_double[old_tets[i][3]][1],
                positions_double[old_tets[i][3]][2],
            }});

            old_energy_max = std::max(energy, old_energy_max);
        } else {
            auto energy = wmtk::function::utils::Tet_AMIPS_energy({{
                positions_double[old_tets[i][1]][0],
                positions_double[old_tets[i][1]][1],
                positions_double[old_tets[i][1]][2],
                positions_double[old_tets[i][0]][0],
                positions_double[old_tets[i][0]][1],
                positions_double[old_tets[i][0]][2],
                positions_double[old_tets[i][2]][0],
                positions_double[old_tets[i][2]][1],
                positions_double[old_tets[i][2]][2],
                positions_double[old_tets[i][3]][0],
                positions_double[old_tets[i][3]][1],
                positions_double[old_tets[i][3]][2],
            }});

            old_energy_max = std::max(energy, old_energy_max);
        }
    }

    for (int i = 0; i < 3; ++i) {
        if (utils::wmtk_orient3d(
                positions[new_tets[i][0]],
                positions[new_tets[i][1]],
                positions[new_tets[i][2]],
                positions[new_tets[i][3]]) > 0) {
            auto energy = wmtk::function::utils::Tet_AMIPS_energy({{
                positions_double[new_tets[i][0]][0],
                positions_double[new_tets[i][0]][1],
                positions_double[new_tets[i][0]][2],
                positions_double[new_tets[i][1]][0],
                positions_double[new_tets[i][1]][1],
                positions_double[new_tets[i][1]][2],
                positions_double[new_tets[i][2]][0],
                positions_double[new_tets[i][2]][1],
                positions_double[new_tets[i][2]][2],
                positions_double[new_tets[i][3]][0],
                positions_double[new_tets[i][3]][1],
                positions_double[new_tets[i][3]][2],
            }});

            new_energy_max = std::max(energy, new_energy_max);
        } else {
            auto energy = wmtk::function::utils::Tet_AMIPS_energy({{
                positions_double[new_tets[i][1]][0],
                positions_double[new_tets[i][1]][1],
                positions_double[new_tets[i][1]][2],
                positions_double[new_tets[i][0]][0],
                positions_double[new_tets[i][0]][1],
                positions_double[new_tets[i][0]][2],
                positions_double[new_tets[i][2]][0],
                positions_double[new_tets[i][2]][1],
                positions_double[new_tets[i][2]][2],
                positions_double[new_tets[i][3]][0],
                positions_double[new_tets[i][3]][1],
                positions_double[new_tets[i][3]][2],
            }});

            new_energy_max = std::max(energy, new_energy_max);
        }
    }

    return old_energy_max > new_energy_max;
}

} // namespace wmtk
