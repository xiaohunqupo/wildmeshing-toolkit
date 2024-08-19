#pragma once

#include <wmtk/utils/TupleInspector.hpp>
#include "autogenerated_tables.hpp"
namespace wmtk::autogen::edge_mesh {
// computes the offset of a tuple's local ids in the tables
inline int64_t local_id_table_offset(const Tuple& tuple)
{
    return wmtk::utils::TupleInspector::local_vid(tuple);
}

inline std::array<int64_t, 1> lvid_from_table_offset(int64_t table_offset)
{
    std::array<int64_t, 1> r;
    auto& [lvid] = r;

    lvid = table_offset;
    return r;
}

} // namespace wmtk::autogen::edge_mesh
