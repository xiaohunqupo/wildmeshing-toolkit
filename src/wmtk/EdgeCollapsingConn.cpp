//
// Created by Yixin Hu on 12/9/21.
//
#include <wmtk/TetMesh.h>

#include <algorithm>
#include <cstddef>
#include <type_traits>
#include <wmtk/utils/TupleUtils.hpp>
#include "spdlog/spdlog.h"
#include "wmtk/utils/VectorUtils.h"

bool wmtk::TetMesh::collapse_edge(const Tuple& loc0, std::vector<Tuple>& new_edges)
{
    if (!collapse_before(loc0)) return false;
    auto link_condition = [&VC = this->m_vertex_connectivity,
                           &TC = this->m_tet_connectivity](auto v0, auto v1) -> bool {
        auto intersects = [](const auto& vec, const auto& val) {
            for (auto& v : vec)
                for (auto& a : val)
                    if (v == a) return true;
            return false;
        };
        auto closure0 = VC[v0].m_conn_tets;
        auto closure1 = VC[v1].m_conn_tets;

        auto link = [&intersects, &conn = TC](const auto& verts, const auto& closure0) {
            auto conn_verts = std::set<size_t>();
            auto conn_edges = std::set<std::array<size_t, 2>>();
            auto conn_faces = std::set<std::array<size_t, 3>>();
            for (auto& t : closure0) {
                auto& tet = conn[t];
                for (auto j = 0; j < 4; j++) {
                    auto vj = tet[j];
                    if (intersects(std::array<size_t, 1>{vj}, verts)) continue;
                    conn_verts.insert(vj);
                }
                for (auto e : m_local_edges) {
                    auto e0 = tet[e[0]], e1 = tet[e[1]];
                    auto edge = std::array<size_t, 2>({std::min(e0, e1), std::max(e0, e1)});
                    if (intersects(edge, verts)) continue;
                    conn_edges.emplace(edge);
                }
                for (auto f : m_local_faces) {
                    auto face = std::array<size_t, 3>{tet[f[0]], tet[f[1]], tet[f[2]]};
                    if (intersects(face, verts)) continue;
                    std::sort(face.begin(), face.end());
                    conn_faces.emplace(face);
                }
            }
            return std::tuple<
                std::set<size_t>,
                std::set<std::array<size_t, 2>>,
                std::set<std::array<size_t, 3>>>{conn_verts, conn_edges, conn_faces};
        };
        auto lk0 = link(std::array<size_t, 1>{{v0}}, closure0);
        auto lk1 = link(std::array<size_t, 1>{{v1}}, closure1);

        // link of edge
        auto common_tets = set_intersection(closure0, closure1);
        auto lk_01 = link(std::array<size_t, 2>{v0, v1}, common_tets);
        if (!std::get<2>(lk_01).empty()) return false;

        auto lk_i = std::tuple<
            std::vector<size_t>,
            std::vector<std::array<size_t, 2>>,
            std::vector<std::array<size_t, 3>>>();
        auto check_inter = [](const auto& l0, const auto& l1, const auto& l01, auto& lk_i) {
            std::set_intersection(
                l0.begin(),
                l0.end(),
                l1.begin(),
                l1.end(),
                std::back_inserter(lk_i));
            if (!std::equal(lk_i.begin(), lk_i.end(), l01.begin(), l01.end())) return false;
            return true;
        };
        if (!check_inter(std::get<0>(lk0), std::get<0>(lk1), std::get<0>(lk_01), std::get<0>(lk_i)))
            return false;
        if (!check_inter(std::get<1>(lk0), std::get<1>(lk1), std::get<1>(lk_01), std::get<1>(lk_i)))
            return false;
        if (!check_inter(std::get<2>(lk0), std::get<2>(lk1), std::get<2>(lk_01), std::get<2>(lk_i)))
            return false;

        return true;
    };

    /// backup of everything
    auto v1_id = loc0.vid(*this);
    auto loc1 = switch_vertex(loc0);
    auto v2_id = loc1.vid(*this);
    logger().trace("{} {}", v1_id, v2_id);
    if (!link_condition(v1_id, v2_id)) return false;

    std::vector<size_t> n1_v_ids;
    for (size_t t_id : m_vertex_connectivity[v1_id].m_conn_tets) {
        for (int j = 0; j < 4; j++) n1_v_ids.push_back(m_tet_connectivity[t_id][j]);
    }
    vector_unique(n1_v_ids);
    std::vector<std::pair<size_t, TetrahedronConnectivity>> old_tets;
    std::vector<std::pair<size_t, VertexConnectivity>> old_vertices;
    for (size_t t_id : m_vertex_connectivity[v1_id].m_conn_tets)
        old_tets.emplace_back(t_id, m_tet_connectivity[t_id]);
    for (size_t v_id : n1_v_ids) old_vertices.emplace_back(v_id, m_vertex_connectivity[v_id]);

    /// update connectivity
    auto n1_t_ids =
        m_vertex_connectivity[v1_id].m_conn_tets; // note: conn_tets for v1 without removed tets
    auto n12_t_ids = set_intersection(
        m_vertex_connectivity[v1_id].m_conn_tets,
        m_vertex_connectivity[v2_id].m_conn_tets);
    //
    m_vertex_connectivity[v1_id].m_is_removed = true;
    for (size_t t_id : n12_t_ids) {
        m_tet_connectivity[t_id].m_is_removed = true;
        vector_erase(m_vertex_connectivity[v2_id].m_conn_tets, t_id);
        vector_erase(n1_t_ids, t_id); // to add to conn_tets for v2
        //
        for (int j = 0; j < 4; j++) {
            int v_id = m_tet_connectivity[t_id][j];
            if (v_id != v1_id && v_id != v2_id)
                vector_erase(m_vertex_connectivity[v_id].m_conn_tets, t_id);
        }
    }
    //
    for (size_t t_id : n1_t_ids) {
        m_vertex_connectivity[v2_id].m_conn_tets.push_back(t_id);
        int j = m_tet_connectivity[t_id].find(v1_id);
        assert(j >= 0);
        m_tet_connectivity[t_id][j] = v2_id;

        // update timestamp of tets
        m_tet_connectivity[t_id].timestamp++;
    }
    vector_sort(m_vertex_connectivity[v2_id].m_conn_tets);
    //
    m_vertex_connectivity[v1_id].m_conn_tets.clear(); // release mem

    Tuple new_loc = tuple_from_vertex(v2_id);
    if (!vertex_invariant(new_loc) || !edge_invariant(new_loc) || !tetrahedron_invariant(new_loc) ||
        !collapse_after(new_loc)) {
        m_vertex_connectivity[v1_id].m_is_removed = false;
        for (int t_id : n12_t_ids) m_tet_connectivity[t_id].m_is_removed = false;
        //
        for (int i = 0; i < old_tets.size(); i++) {
            int t_id = old_tets[i].first;
            m_tet_connectivity[t_id] = old_tets[i].second;
        }
        for (int i = 0; i < old_vertices.size(); i++) {
            int v_id = old_vertices[i].first;
            m_vertex_connectivity[v_id] = old_vertices[i].second;
        }

        return false;
    }

    /// return new_edges
    for (size_t t_id : n1_t_ids) {
        for (int j = 0; j < 6; j++) {
            if (m_tet_connectivity[t_id][m_local_edges[j][0]] == v2_id ||
                m_tet_connectivity[t_id][m_local_edges[j][1]] == v2_id)
                new_edges.push_back(tuple_from_edge(t_id, j));
        }
    }
    unique_edge_tuples(*this, new_edges);

    return true;
}