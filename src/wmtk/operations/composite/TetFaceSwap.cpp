#include "TetFaceSwap.hpp"
#include <wmtk/Mesh.hpp>

namespace wmtk::operations::composite {
TetFaceSwap::TetFaceSwap(Mesh& m)
    : Operation(m)
    , m_split(m)
    , m_collapse(m)
{}

std::vector<Simplex> TetFaceSwap::execute(const Simplex& simplex)
{
    constexpr static PrimitiveType PV = PrimitiveType::Vertex;
    constexpr static PrimitiveType PE = PrimitiveType::Edge;
    constexpr static PrimitiveType PF = PrimitiveType::Face;
    constexpr static PrimitiveType PT = PrimitiveType::Tetrahedron;

    // first split, split the edge of the input tuple
    const auto split_simplicies_first = m_split(Simplex::edge(simplex.tuple()));
    if (split_simplicies_first.empty()) return {};
    assert(split_simplicies_first.size() == 1);

    // prepare the simplex for the second split, the rib edge generated by the first split on the
    // input face plane
    const Tuple& split_ret_first = split_simplicies_first.front().tuple();
    const auto edge_input_for_second_split = Simplex::edge(mesh().switch_edge(split_ret_first));

    // second split, split the rib edge generated by the first split on the input face plane
    const auto split_simplicies_second = m_split(edge_input_for_second_split);
    if (split_simplicies_second.empty()) return {};
    assert(split_simplicies_second.size() == 1);

    // prepare the simplex for the first collapse
    const Tuple& split_ret_second = split_simplicies_second.front().tuple();
    const auto edge_input_for_first_collapse =
        Simplex::edge(mesh().switch_tuples(split_ret_second, {PE, PF, PT, PF, PE, PV, PE}));

    // prepare the tuple for the second collapse
    const Tuple& edge_tuple_input_for_second_collapse =
        mesh().switch_tuples(split_ret_second, {PF, PE});

    // prepare the tuple for return simplex (edge)
    const Tuple& edge_tuple_for_return = mesh().switch_tuples(split_ret_second, {PT, PF, PE, PV});

    // first collapse, merge spine edges and their related primitives generated by the first split
    const auto collapse_simplicies_first = m_collapse(edge_input_for_first_collapse);
    if (collapse_simplicies_first.empty()) return {};
    assert(collapse_simplicies_first.size() == 1);

    // prepare the simplex for the second collapse
    const auto edge_input_for_second_collapse =
        Simplex::edge(resurrect_tuple(edge_tuple_input_for_second_collapse));

    // second collapse, drag the interior vertex on the ground to the "peak"
    const auto collapse_simplicies_second = m_collapse(edge_input_for_second_collapse);
    if (collapse_simplicies_second.empty()) return {};
    assert(collapse_simplicies_second.size() == 1);

    return {Simplex::edge(resurrect_tuple(edge_tuple_for_return))};
}

std::vector<Simplex> TetFaceSwap::unmodified_primitives(const Simplex& simplex) const
{
    return {simplex};
}


} // namespace wmtk::operations::composite