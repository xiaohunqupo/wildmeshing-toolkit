#pragma once
#include "autogenerated_tables.hpp"
#include <wmtk/Tuple.hpp>
#include <cassert>
#if !defined(_NDEBUG)
#include "is_ccw.hpp"
#endif

namespace wmtk::autogen::tri_mesh {
    inline Tuple get_tuple_from_simplex_local_vertex_id(int8_t local_id, int64_t global_fid) {

    assert(local_id >= 0);
    assert(local_id < 3);

            assert(autogen::tri_mesh::auto_2d_table_complete_vertex[local_id][0] == local_id);
            const int64_t leid = autogen::tri_mesh::auto_2d_table_complete_vertex[local_id][1];
            Tuple v_tuple = Tuple(local_id, leid, -1, global_fid);
            // accessor as parameter
            assert(is_ccw(v_tuple)); // is_ccw also checks for validity
            return v_tuple;
    }
    inline Tuple get_tuple_from_simplex_local_edge_id(int8_t local_id, int64_t global_fid) {
    assert(local_id >= 0);
    assert(local_id < 3);
            assert(autogen::tri_mesh::auto_2d_table_complete_edge[local_id][1] == local_id);
            const int64_t lvid = autogen::tri_mesh::auto_2d_table_complete_edge[local_id][0];


            Tuple e_tuple = Tuple(lvid, local_id, -1, global_fid);
            assert(is_ccw(e_tuple));
            return e_tuple;
    }
    inline Tuple get_tuple_from_simplex_local_id(PrimitiveType pt, int8_t local_id, int64_t global_fid) {
        switch(pt) {
            case PrimitiveType::Vertex: return get_tuple_from_simplex_local_vertex_id(local_id, global_fid);
            case PrimitiveType::Edge: return get_tuple_from_simplex_local_edge_id(local_id, global_fid);
            default:
            case PrimitiveType::Triangle:
            case PrimitiveType::Tetrahedron:
                                      assert(false);
                                      return {};
        }
    }
}
