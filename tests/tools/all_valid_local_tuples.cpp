#include "all_valid_local_tuples.hpp"
#include <cassert>
#include <wmtk/autogen/tet_mesh/autogenerated_tables.hpp>
#include <wmtk/autogen/tet_mesh/local_id_table_offset.hpp>
#include <wmtk/autogen/tri_mesh/autogenerated_tables.hpp>
#include <wmtk/autogen/tri_mesh/local_id_table_offset.hpp>

#include <algorithm>
#include <wmtk/autogen/is_ccw.hpp>
#include <wmtk/autogen/tet_mesh/is_ccw.hpp>
#include <wmtk/autogen/tri_mesh/is_ccw.hpp>
using namespace wmtk::autogen;
namespace wmtk::tests {
std::vector<PrimitiveType> primitives_up_to(PrimitiveType pt)
{
    std::vector<PrimitiveType> r;

    switch (pt) {
    case PrimitiveType::Tetrahedron: r.emplace_back(PrimitiveType::Face); [[fallthrough]];
    case PrimitiveType::Face: r.emplace_back(PrimitiveType::Edge); [[fallthrough]];
    case PrimitiveType::Edge: r.emplace_back(PrimitiveType::Vertex); [[fallthrough]];
    case PrimitiveType::Vertex:
    default: break;
    }
    return r;
}


long max_tuple_count(PrimitiveType pt)
{
    switch (pt) {
    case PrimitiveType::Face: return long(std::size(wmtk::autogen::tri_mesh::auto_2d_table_ccw));
    case PrimitiveType::Tetrahedron:
        return long(std::size(wmtk::autogen::tet_mesh::auto_3d_table_ccw));
    case PrimitiveType::Edge: return 2;
    case PrimitiveType::Vertex: break;
    case PrimitiveType::HalfEdge: break;
    }
    return -1;
}

Tuple tuple_from_offset_id(PrimitiveType pt, int offset)
{
    long lvid = 0, leid = 0, lfid = 0, gcid = 0, hash = 0;

    switch (pt) {
    case PrimitiveType::Face: {
        // bug in the standard? tie should work :-(
        auto r = tri_mesh::lvid_leid_from_table_offset(offset);
        lvid = r[0];
        leid = r[1];
    } break;
    case PrimitiveType::Tetrahedron: {
        auto r = tet_mesh::lvid_leid_lfid_from_table_offset(offset);
        lvid = r[0];
        leid = r[1];
        lfid = r[2];
    } break;
    case PrimitiveType::Edge: lvid = offset;
    case PrimitiveType::Vertex: break;
    case PrimitiveType::HalfEdge: break;
    }

    Tuple r(lvid, leid, lfid, gcid, hash);
    if (!tuple_is_valid_for_ccw(pt, r)) {
        r = Tuple();
    }
    return r;
}

std::vector<Tuple> all_valid_local_tuples(PrimitiveType pt)
{
    std::vector<Tuple> tups;
    long size = max_tuple_count(pt);
    assert(size > 0);
    tups.reserve(size);
    for (long idx = 0; idx < max_tuple_count(pt); ++idx) {
        tups.emplace_back(tuple_from_offset_id(pt, idx));
    }

    tups.erase(
        std::remove_if(
            tups.begin(),
            tups.end(),
            [](const Tuple& t) -> bool { return t.is_null(); }),
        tups.end());
    return tups;
}
} // namespace wmtk::tests
