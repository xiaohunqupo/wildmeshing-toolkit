#pragma once

#include <tbb/concurrent_queue.h>
#include <tbb/concurrent_vector.h>
#include <tbb/enumerable_thread_specific.h>
#include <wmtk/ConcurrentTetMesh.h>
#include <wmtk/utils/PartitionMesh.h>
#include <atomic>

#include <Eigen/Core>
#include <memory>

namespace harmonic_tet {

class ParallelHarmonicTet : public wmtk::ConcurrentTetMesh
{
public:
    using VertexAttributes = Eigen::Vector3d;
    struct TetAttributes
    {
    };

    ParallelHarmonicTet(
        const tbb::concurrent_vector<VertexAttributes>& _vertex_attribute,
        const std::vector<std::array<size_t, 4>>& tets,
        int num_threads = 1)
    {
        m_vertex_attribute = _vertex_attribute;
        m_tet_attribute = tbb::concurrent_vector<TetAttributes>(tets.size());
        NUM_THREADS = num_threads;
        init(m_vertex_attribute.size(), tets);
        m_vertex_partition_id = partition_TetMesh(*this, NUM_THREADS);
    }
    ParallelHarmonicTet(){};
    ~ParallelHarmonicTet(){};

    ////// Attributes related
    // Stores the attributes attached to simplices
    tbb::concurrent_vector<VertexAttributes> m_vertex_attribute;
    tbb::concurrent_vector<TetAttributes> m_tet_attribute;
    tbb::concurrent_vector<size_t> m_vertex_partition_id;

    void resize_vertex_attributes(size_t v) override
    {
        ConcurrentTetMesh::resize_vertex_attributes(v);
        m_vertex_attribute.grow_to_at_least(v);
        m_vertex_partition_id.grow_to_at_least(v);
    }
    void resize_tet_attributes(size_t t) override { m_tet_attribute.grow_to_at_least(t); }

    void move_tet_attribute(size_t from, size_t to) override
    {
        m_tet_attribute[to] = std::move(m_tet_attribute[from]);
    }
    void move_vertex_attribute(size_t from, size_t to) override
    {
        m_vertex_attribute[to] = std::move(m_vertex_attribute[from]);
    }

    void output_mesh(std::string file) const;

    ////// Operations

    // parallel containers
    int NUM_THREADS;
    // std::vector<tbb::concurrent_queue<std::tuple<double, wmtk::TetMesh::Tuple>>> queues;

    struct SwapInfoCache
    {
        double max_energy = 0.;
    };
    tbb::enumerable_thread_specific<SwapInfoCache> edgeswap_cache, faceswap_cache;

    void smooth_all_vertices();
    bool smooth_before(const Tuple& t) override;
    bool smooth_after(const Tuple& t) override;

    void swap_all_edges();
    void swap_all_edges_stuff(
        std::vector<tbb::concurrent_queue<std::tuple<double, wmtk::TetMesh::Tuple>>> queues,
        std::atomic_int& cnt_suc,
        int task_id);
    bool swap_edge_before(const Tuple& t) override;
    bool swap_edge_after(const Tuple& t) override;

    void swap_all_faces();
    void swap_all_faces_stuff();
    bool swap_face_before(const Tuple& t) override;
    bool swap_face_after(const Tuple& t) override;

    bool is_inverted(const Tuple& loc);
    double get_quality(const Tuple& loc);
};

} // namespace harmonic_tet