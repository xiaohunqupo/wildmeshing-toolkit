#include <stdlib.h>
#include <catch2/catch.hpp>
#include <iostream>
//#include <igl/read_triangle_mesh.h>
#include <wmtk/utils/trimesh_topology_initialization.h>
#include <wmtk/TriMesh.hpp>
// #include <wmtk/operations/TriMeshConsolidateOperation.h>
// #include <wmtk/operations/TriMeshEdgeCollapseOperation.h>
// #include <wmtk/operations/TriMeshEdgeSplitOperation.h>
// #include <wmtk/operations/TriMeshEdgeSwapOperation.h>
// #include <wmtk/operations/TriMeshVertexSmoothOperation.h>
// #include <highfive/H5File.hpp>
// #include <wmtk/operations/TriMeshOperationShim.hpp>

// template <>
// HighFive::DataType HighFive::create_datatype<TriMesh::VertexConnectivity>() {
// }
// template <>
// HighFive::DataType HighFive::create_datatype<TriMesh::TriangleConnectivity>(){
//    {"indices", HighFive::create_datatype<size_t[3]>()},
//    {"is_removed", HighFive::create_datatype<bool>()},
//    {"hash", HighFive::create_datatype<size_t>()},

// }
// WMTK_HDF5_REGISTER_ATTRIBUTE_TYPE(wmtk::TriMesh::VertexConnectivity)
// WMTK_HDF5_REGISTER_ATTRIBUTE_TYPE(wmtk::TriMesh::TriangleConnectivity)

using namespace wmtk;

TEST_CASE("load mesh and create TriMesh", "[test_mesh_creation]")
{
    TriMesh m;
    RowVectors3l tris;
    tris.resize(1, 3);
    tris.row(0) = Eigen::Matrix<long, 3, 1>{0, 1, 2};

    SECTION("init with FV, FE, FF, VF, EF")
    {
        const auto [FE, FF, VF, EF] = trimesh_topology_initialization(tris);

        m.initialize(tris, FE, FF, VF, EF);
    }
    SECTION("init directly from RowVectors3l")
    {
        m.initialize(tris);
    }

    // TODO add all the stuff below

    // // REQUIRE(m.tri_capacity() == tris.size()); // TODO:
    // // REQUIRE(m.vert_capacity() == 3); // TODO:
    // REQUIRE(check_mesh_connectivity_validity());

    // Tuple t = m.tuple_from_cell(0);
    // REQUIRE(t.is_valid(m));
    // auto oriented_vertices = m.oriented_tri_vertices(t);
    // for(const auto& v: oriented_vertices) {
    //     CHECK(v.is_ccw(m));
    // }
    REQUIRE(false);
}

TEST_CASE("test generate tuples with 1 triangle", "[test_tuple_generation]")
{
    TriMesh m;
    {
        RowVectors3l tris;
        tris.resize(1, 3);
        tris.row(0) = Eigen::Matrix<long, 3, 1>{0, 1, 2};
        m.initialize(tris);
    }

    const std::vector<Tuple> vertices = m.get_all(PrimitiveType::Vertex);
    REQUIRE(vertices.size() == 3);
    CHECK(m._debug_id(vertices[0], PrimitiveType::Vertex) == 0);
    CHECK(m._debug_id(vertices[1], PrimitiveType::Vertex) == 1);
    CHECK(m._debug_id(vertices[2], PrimitiveType::Vertex) == 2);
    CHECK(m._debug_id(vertices[0], PrimitiveType::Face) == 0);
    CHECK(m._debug_id(vertices[1], PrimitiveType::Face) == 0);
    CHECK(m._debug_id(vertices[2], PrimitiveType::Face) == 0);

    const std::vector<Tuple> edges = m.get_all(PrimitiveType::Edge);
    REQUIRE(edges.size() == 3);
    //  TODO add test for edge ids
    CHECK(false);
    CHECK(false);
    CHECK(false);

    const std::vector<Tuple> faces = m.get_all(PrimitiveType::Face);
    REQUIRE(faces.size() == 1);
    CHECK(m._debug_id(faces[0], PrimitiveType::Face) == 0);
}

TEST_CASE("test generate tuples with 2 triangle", "[test_tuple_generation]")
{
    // 	   v3     /
    //     / \    /
    // 	  /f1 \   /
    // v2 -----v1 /
    // 	  \f0 /   /
    //     \ /    /
    // 	    v0    /

    TriMesh m;
    {
        RowVectors3l tris;
        tris.resize(2, 3);
        tris.row(0) = Eigen::Matrix<long, 3, 1>{0, 1, 2};
        tris.row(1) = Eigen::Matrix<long, 3, 1>{2, 1, 3};
        m.initialize(tris);
    }

    const std::vector<Tuple> vertices = m.get_all(PrimitiveType::Vertex);
    REQUIRE(vertices.size() == 4);
    CHECK(m._debug_id(vertices[0], PrimitiveType::Vertex) == 0);
    CHECK(m._debug_id(vertices[1], PrimitiveType::Vertex) == 1);
    CHECK(m._debug_id(vertices[2], PrimitiveType::Vertex) == 2);
    CHECK(m._debug_id(vertices[3], PrimitiveType::Vertex) == 3);
    CHECK(m._debug_id(vertices[0], PrimitiveType::Face) == 0);
    CHECK(m._debug_id(vertices[3], PrimitiveType::Face) == 1);

    const std::vector<Tuple> edges = m.get_all(PrimitiveType::Edge);
    REQUIRE(edges.size() == 4);
    //  TODO add test for edge ids
    CHECK(false);
    CHECK(false);
    CHECK(false);
    CHECK(false);

    const std::vector<Tuple> faces = m.get_all(PrimitiveType::Face);
    REQUIRE(faces.size() == 2);
    CHECK(m._debug_id(faces[0], PrimitiveType::Face) == 0);
    CHECK(m._debug_id(faces[1], PrimitiveType::Face) == 1);
}

// // for every quiry do a require
// TEST_CASE("random 10 switches on 2 traingles", "[tuple_operation]")
// {
//     TriMesh m;
//     std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{1, 2, 3}}};
//     m.create_mesh(4, tris);

//     SECTION("test all tuples generated using vertices")
//     {
//         auto vertices_tuples = m.get_vertices();
//         for (size_t i = 0; i < vertices_tuples.size(); i++) {
//             TriMesh::Tuple v_tuple = vertices_tuples[i];
//             for (size_t j = 0; j < 10; j++) {
//                 size_t test = rand() % 3;
//                 switch (test) {
//                 case 0: v_tuple = v_tuple.switch_vertex(m); break;
//                 case 1: v_tuple = v_tuple.switch_edge(m); break;
//                 case 2: v_tuple = v_tuple.switch_face(m).value_or(v_tuple); break;
//                 default:;
//                 }
//             }
//             REQUIRE(v_tuple.is_valid(m));
//         }
//     }

//     SECTION("test all tuples generated using edges")
//     {
//         auto edges_tuples = m.get_edges();
//         for (size_t i = 0; i < edges_tuples.size(); i++) {
//             TriMesh::Tuple e_tuple = edges_tuples[i];
//             for (size_t j = 0; j < 10; j++) {
//                 size_t test = rand() % 3;
//                 switch (test) {
//                 case 0: e_tuple = e_tuple.switch_vertex(m); break;
//                 case 1: e_tuple = e_tuple.switch_edge(m); break;
//                 case 2: e_tuple = e_tuple.switch_face(m).value_or(e_tuple); break;
//                 default:;
//                 }
//             }
//             REQUIRE(e_tuple.is_valid(m));
//         }
//     }

//     SECTION("test all tuples generated using faces")
//     {
//         auto faces_tuples = m.get_faces();
//         for (size_t i = 0; i < faces_tuples.size(); i++) {
//             TriMesh::Tuple f_tuple = faces_tuples[i];
//             for (size_t j = 0; j < 10; j++) {
//                 size_t test = rand() % 3;
//                 switch (test) {
//                 case 0: f_tuple = f_tuple.switch_vertex(m); break;
//                 case 1: f_tuple = f_tuple.switch_edge(m); break;
//                 case 2: f_tuple = f_tuple.switch_face(m).value_or(f_tuple); break;
//                 default:;
//                 }
//             }
//             REQUIRE(f_tuple.is_valid(m));
//         }
//     }
// }

// TriMesh::Tuple double_switch_vertex(TriMesh& m, TriMesh::Tuple t)
// {
//     TriMesh::Tuple t_after = t.switch_vertex(m);
//     t_after = t_after.switch_vertex(m);
//     return t_after;
// }

// TriMesh::Tuple double_switch_edge(TriMesh& m, TriMesh::Tuple t)
// {
//     TriMesh::Tuple t_after = t.switch_edge(m);
//     t_after = t_after.switch_edge(m);
//     return t_after;
// }

// TriMesh::Tuple double_switch_face(TriMesh& m, TriMesh::Tuple t)
// {
//     TriMesh::Tuple t_after = t.switch_face(m).value_or(t);
//     t_after = t_after.switch_face(m).value_or(t);
//     return t_after;
// }

// bool tuple_equal(const TriMesh& m, TriMesh::Tuple t1, TriMesh::Tuple t2)
// {
//     return (t1.fid(m) == t2.fid(m)) && (t1.eid(m) == t2.eid(m)) && (t1.vid(m) == t2.vid(m));
// }


// // checking for every tuple t:
// // (1) t.switch_vertex().switch_vertex() == t
// // (2) t.switch_edge().switch_edge() == t
// // (3) t.switch_tri().switch_tri() == t
// TEST_CASE("double switches is identity", "[tuple_operation]")
// {
//     TriMesh m;
//     std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{1, 2, 3}}};
//     m.create_mesh(4, tris);

//     SECTION("test all tuples generated using vertices")
//     {
//         TriMesh::Tuple v_tuple_after;
//         auto vertices_tuples = m.get_vertices();
//         for (size_t i = 0; i < vertices_tuples.size(); i++) {
//             TriMesh::Tuple v_tuple = vertices_tuples[i];
//             v_tuple_after = double_switch_vertex(m, v_tuple);
//             REQUIRE(tuple_equal(m, v_tuple_after, v_tuple));
//             v_tuple_after = double_switch_edge(m, v_tuple);
//             REQUIRE(tuple_equal(m, v_tuple_after, v_tuple));
//             v_tuple_after = double_switch_face(m, v_tuple);
//             REQUIRE(tuple_equal(m, v_tuple_after, v_tuple));
//         }
//     }

//     SECTION("test all tuples generated using edges")
//     {
//         TriMesh::Tuple e_tuple_after;
//         auto edges_tuples = m.get_edges();
//         for (size_t i = 0; i < edges_tuples.size(); i++) {
//             TriMesh::Tuple e_tuple = edges_tuples[i];
//             e_tuple_after = double_switch_vertex(m, e_tuple);
//             REQUIRE(tuple_equal(m, e_tuple_after, e_tuple));
//             e_tuple_after = double_switch_edge(m, e_tuple);
//             REQUIRE(tuple_equal(m, e_tuple_after, e_tuple));
//             e_tuple_after = double_switch_face(m, e_tuple);
//             REQUIRE(tuple_equal(m, e_tuple_after, e_tuple));
//         }
//     }

//     SECTION("test all tuples generated using faces")
//     {
//         TriMesh::Tuple f_tuple_after;
//         auto faces_tuples = m.get_faces();
//         for (size_t i = 0; i < faces_tuples.size(); i++) {
//             TriMesh::Tuple f_tuple = faces_tuples[i];
//             f_tuple_after = double_switch_vertex(m, f_tuple);
//             REQUIRE(tuple_equal(m, f_tuple_after, f_tuple));
//             f_tuple_after = double_switch_edge(m, f_tuple);
//             REQUIRE(tuple_equal(m, f_tuple_after, f_tuple));
//             f_tuple_after = double_switch_face(m, f_tuple);
//             REQUIRE(tuple_equal(m, f_tuple_after, f_tuple));
//         }
//     }
// }
// // check for every t
// // t.switch_vertex().switchedge().switchvertex().switchedge().switchvertex().switchedge() == t
// TEST_CASE("vertex_edge switches equals indentity", "[tuple_operation]")
// {
//     TriMesh m;
//     std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{1, 2, 3}}};
//     m.create_mesh(4, tris);

//     SECTION("test all tuples generated using vertices")
//     {
//         TriMesh::Tuple v_tuple_after;
//         auto vertices_tuples = m.get_vertices();
//         for (size_t i = 0; i < vertices_tuples.size(); i++) {
//             TriMesh::Tuple v_tuple = vertices_tuples[i];
//             v_tuple_after = v_tuple.switch_vertex(m);
//             v_tuple_after = v_tuple_after.switch_edge(m);
//             v_tuple_after = v_tuple_after.switch_vertex(m);
//             v_tuple_after = v_tuple_after.switch_edge(m);
//             v_tuple_after = v_tuple_after.switch_vertex(m);
//             v_tuple_after = v_tuple_after.switch_edge(m);
//             REQUIRE(tuple_equal(m, v_tuple, v_tuple_after));
//         }
//     }

//     SECTION("test all tuples generated using edges")
//     {
//         TriMesh::Tuple e_tuple_after;
//         auto edges_tuples = m.get_edges();
//         for (size_t i = 0; i < edges_tuples.size(); i++) {
//             TriMesh::Tuple e_tuple = edges_tuples[i];
//             e_tuple_after = e_tuple.switch_vertex(m);
//             e_tuple_after = e_tuple_after.switch_edge(m);
//             e_tuple_after = e_tuple_after.switch_vertex(m);
//             e_tuple_after = e_tuple_after.switch_edge(m);
//             e_tuple_after = e_tuple_after.switch_vertex(m);
//             e_tuple_after = e_tuple_after.switch_edge(m);
//             REQUIRE(tuple_equal(m, e_tuple, e_tuple_after));
//         }
//     }

//     SECTION("test all tuples generated using faces")
//     {
//         TriMesh::Tuple f_tuple_after;
//         auto faces_tuples = m.get_faces();
//         for (size_t i = 0; i < faces_tuples.size(); i++) {
//             TriMesh::Tuple f_tuple = faces_tuples[i];
//             f_tuple_after = f_tuple.switch_vertex(m);
//             f_tuple_after = f_tuple_after.switch_edge(m);
//             f_tuple_after = f_tuple_after.switch_vertex(m);
//             f_tuple_after = f_tuple_after.switch_edge(m);
//             f_tuple_after = f_tuple_after.switch_vertex(m);
//             f_tuple_after = f_tuple_after.switch_edge(m);
//             REQUIRE(tuple_equal(m, f_tuple, f_tuple_after));
//         }
//     }
// }

// TEST_CASE("test_link_check", "[test_pre_check]")
// {
//     TriMesh m;
//     SECTION("extra_face_after_collapse")
//     {
//         std::vector<std::array<size_t, 3>> tris =
//             {{{1, 2, 3}}, {{0, 1, 4}}, {{0, 2, 5}}, {{0, 1, 6}}, {{0, 2, 6}}, {{1, 2, 6}}};
//         m.create_mesh(7, tris);
//         TriMesh::Tuple edge(1, 2, 0, m);

//         REQUIRE(edge.vid(m) == 1);
//         REQUIRE(edge.switch_vertex(m).vid(m) == 2);

//         {
//             // short test for tris_bounded_by_edge
//             std::vector<TriMeshTuple> faces = m.tris_bounded_by_edge(edge);
//             for (const auto& f : faces) {
//                 REQUIRE(f.is_valid(m));
//             }
//             std::vector<size_t> fids;
//             std::transform(
//                 faces.begin(),
//                 faces.end(),
//                 std::back_inserter(fids),
//                 [&](const TriMeshTuple& t) -> size_t { return t.fid(m); });
//             REQUIRE(std::is_sorted(fids.begin(), fids.end()));
//             CHECK(fids[0] == 0);
//             CHECK(fids[1] == 5);
//         }

//         REQUIRE_FALSE(TriMeshEdgeCollapseOperation::check_link_condition(m, edge));
//     }
//     SECTION("one_triangle")
//     {
//         std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}};
//         m.create_mesh(3, tris);

//         TriMesh::Tuple edge(0, 2, 0, m);
//         assert(edge.is_valid(m));
//         REQUIRE_FALSE(TriMeshEdgeCollapseOperation::check_link_condition(m, edge));
//     }
//     SECTION("one_tet")
//     {
//         std::vector<std::array<size_t, 3>> tris = {
//             {{0, 1, 2}},
//             {{1, 3, 2}},
//             {{0, 2, 3}},
//             {{3, 0, 1}}};
//         m.create_mesh(4, tris);

//         TriMesh::Tuple edge(1, 0, 0, m);
//         assert(edge.is_valid(m));
//         REQUIRE_FALSE(TriMeshEdgeCollapseOperation::check_link_condition(m, edge));
//     }
//     SECTION("non_manifold_after_collapse")
//     {
//         std::vector<std::array<size_t, 3>> tris = {
//             {{0, 1, 5}},
//             {{1, 2, 5}},
//             {{2, 3, 5}},
//             {{5, 3, 4}}};
//         m.create_mesh(6, tris);

//         TriMesh::Tuple fail_edge(5, 0, 1, m);
//         REQUIRE_FALSE(TriMeshEdgeCollapseOperation::check_link_condition(m, fail_edge));
//         TriMesh::Tuple pass_edge(0, 2, 0, m);
//         REQUIRE(TriMeshEdgeCollapseOperation::check_link_condition(m, pass_edge));
//     }
// }
// // test manifold (eid uniqueness)
// TEST_CASE("test unique edge", "[test_2d_operation]")
// {
//     std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{1, 3, 2}}, {{4, 1, 0}}, {{0, 2,
//     5}}}; auto m = TriMesh(); m.create_mesh(6, tris); REQUIRE(m.check_edge_manifold());
// }

// TEST_CASE("edge_collapse", "[test_2d_operation]")
// {
//     std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{1, 3, 2}}, {{4, 1, 0}}, {{0, 2,
//     5}}}; SECTION("rollback")
//     {
//         class NoCollapseCollapseOperation : public wmtk::TriMeshEdgeCollapseOperation
//         {
//             bool before(TriMesh&, const TriMesh::Tuple&) override { return true; };
//             bool after(TriMesh&) override { return false; };
//         } collapse_op;
//         TriMesh m;
//         // auto m = NoCollapseMesh();
//         m.create_mesh(6, tris);
//         const auto tuple = TriMesh::Tuple(1, 0, 0, m);
//         REQUIRE(tuple.is_valid(m));
//         std::vector<TriMesh::Tuple> dummy;
//         REQUIRE_FALSE(collapse_op(m, tuple));
//         REQUIRE(tuple.is_valid(m));
//     }
//     SECTION("collapse")
//     {
//         class AllCollapseCollapseOperation : public wmtk::TriMeshEdgeCollapseOperation
//         {
//             bool before(TriMesh&, const TriMesh::Tuple&) override { return true; };
//             bool after(TriMesh&) override { return true; };
//         } collapse_op;
//         TriMesh m;

//         m.create_mesh(6, tris);
//         const auto tuple = TriMesh::Tuple(1, 0, 0, m);

//         REQUIRE(tuple.is_valid(m));

//         REQUIRE(collapse_op(m, tuple)); // fail at check manifold
//         REQUIRE_FALSE(tuple.is_valid(m));
//     }
// }

// TEST_CASE("swap_operation", "[test_2d_operation]")
// {
//     wmtk::TriMeshEdgeSwapOperation swap_op;
//     SECTION("swap")
//     {
//         TriMesh m;
//         std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{3, 1, 0}}};
//         m.create_mesh(4, tris);
//         TriMesh::Tuple edge(0, 2, 0, m);
//         assert(edge.is_valid(m));

//         REQUIRE(swap_op(m, edge));
//     }
//     SECTION("swap_boundary")
//     {
//         TriMesh m;
//         std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{3, 1, 0}}};
//         m.create_mesh(4, tris);
//         TriMesh::Tuple edge(0, 1, 0, m);
//         assert(edge.is_valid(m));

//         REQUIRE_FALSE(swap_op(m, edge));
//     }

//     SECTION("swap_on_connected_vertices")
//     {
//         TriMesh m2;
//         std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{1, 0, 3}}, {{1, 3, 2}}};
//         m2.create_mesh(4, tris);
//         TriMesh::Tuple edge(0, 2, 0, m2);
//         assert(edge.is_valid(m2));
//         REQUIRE_FALSE(swap_op(m2, edge));
//     }

//     SECTION("swap 4 times retain start tuple")
//     {
//         TriMesh m3;
//         std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{3, 1, 0}}};
//         m3.create_mesh(4, tris);
//         TriMesh::Tuple edge(0, 2, 0, m3);
//         assert(edge.is_valid(m3));


//         bool success;

//         success = swap_op(m3, edge);
//         REQUIRE(success);
//         auto new_t = swap_op.modified_triangles(m3)[0];
//         assert(new_t.is_valid(m3));

//         success = swap_op(m3, new_t);
//         REQUIRE(success);
//         new_t = swap_op.modified_triangles(m3)[0];
//         assert(new_t.is_valid(m3));

//         success = swap_op(m3, new_t);
//         REQUIRE(success);
//         new_t = swap_op.modified_triangles(m3)[0];
//         assert(new_t.is_valid(m3));

//         success = swap_op(m3, new_t);
//         REQUIRE(success);
//         new_t = swap_op.modified_triangles(m3)[0];
//         REQUIRE(new_t.vid(m3) == 0);
//         REQUIRE(new_t.switch_vertex(m3).vid(m3) == 1);
//         REQUIRE(new_t.switch_edge(m3).switch_vertex(m3).vid(m3) == 2);
//     }
// }

// TEST_CASE("split_operation", "[test_2d_operation]")
// {
//     wmtk::TriMeshEdgeSplitOperation split_op;
//     TriMesh m;
//     SECTION("1_tri_split")
//     {
//         std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}};
//         m.create_mesh(3, tris);
//         auto edges = m.get_edges();
//         TriMesh::Tuple edge(0, 1, 0, m);
//         assert(edge.is_valid(m));
//         REQUIRE(split_op(m, edge));
//         REQUIRE_FALSE(edges[0].is_valid(m));
//     }
//     SECTION("2_tris_split")
//     {
//         std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{1, 3, 2}}};
//         m.create_mesh(4, tris);
//         auto edges = m.get_edges();
//         TriMesh::Tuple edge(1, 0, 0, m);
//         assert(edge.is_valid(m));
//         REQUIRE(split_op(m, edge));
//         for (auto e : edges) REQUIRE_FALSE(e.is_valid(m));
//     }
// }
