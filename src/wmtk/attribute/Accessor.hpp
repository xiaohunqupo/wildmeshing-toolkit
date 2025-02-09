#pragma once
#include <algorithm>
#if defined(AWEFWAEF)
#include <wmtk/simplex/IdSimplex.hpp>
#include <wmtk/simplex/Simplex.hpp>
#include "CachingAccessor.hpp"

namespace wmtk {
class Mesh;
class TriMesh;
class TetMesh;
class TriMeshOperationExecutor;
class EdgeMesh;
namespace tests {
class DEBUG_TriMesh;
class DEBUG_EdgeMesh;
} // namespace tests
} // namespace wmtk
namespace wmtk::attribute {
/**
 * A CachingAccessor that uses tuples for accessing attributes instead of indices.
 * As global simplex ids should not be publicly available, this accessor uses the Mesh.id() function
 * to map from a tuple to the global simplex id.
 */
template <typename T, typename MeshType = Mesh, int Dim = Eigen::Dynamic>
class Accessor : protected CachingAccessor<T, Dim>
{
public:
    friend class wmtk::Mesh;
    friend class wmtk::TetMesh;
    friend class wmtk::TriMesh;
    friend class wmtk::EdgeMesh;
    friend class wmtk::PointMesh;
    using Scalar = T;

    using BaseType = AccessorBase<T, Dim>;
    using CachingBaseType = CachingAccessor<T, Dim>;

    // Eigen::Map<VectorX<T>>
    template <int D = Dim>
    using MapResult = typename BaseType::MapResult<D>;
    // Eigen::Map<const VectorX<T>>
    template <int D = Dim>
    using ConstMapResult = typename BaseType::ConstMapResult<D>;


    Accessor(MeshType& m, const TypedAttributeHandle<T>& handle);
    Accessor(const MeshType& m, const TypedAttributeHandle<T>& handle);

    template <typename OMType, int D>
    Accessor(const Accessor<T, OMType, D>& o);


    // =============
    // Access methods
    // =============
    // NOTE: If any compilation warnings occur check that there is an overload for an index method

    const T& const_topological_scalar_attribute(const Tuple& t, PrimitiveType pt) const;
    template <typename ArgType>
    T& topological_scalar_attribute(const ArgType& t);
    template <typename ArgType>
    const T& const_scalar_attribute(const ArgType& t) const;
    template <typename ArgType>
    T& scalar_attribute(const ArgType& t);

    template <int D = Dim, typename ArgType = wmtk::Tuple>
    ConstMapResult<D> const_vector_attribute(const ArgType& t) const;
    template <int D = Dim, typename ArgType = wmtk::Tuple>
    MapResult<D> vector_attribute(const ArgType& t);

    using BaseType::dimension; // const() -> int64_t
    using BaseType::reserved_size; // const() -> int64_t

    using BaseType::attribute; // access to Attribute object being used here
    using BaseType::handle;
    using BaseType::primitive_type;
    using BaseType::typed_handle;
    using CachingBaseType::has_stack;
    using CachingBaseType::mesh;
    using CachingBaseType::stack_depth;

    MeshType& mesh() { return static_cast<MeshType&>(BaseType::mesh()); }
    const MeshType& mesh() const { return static_cast<const MeshType&>(BaseType::mesh()); }

protected:
    int64_t index(const Tuple& t) const;
    int64_t index(const simplex::Simplex& t) const;
    int64_t index(const simplex::IdSimplex& t) const;

    using CachingBaseType::base_type;
    CachingBaseType& caching_base_type() { return *this; }
    const CachingBaseType& caching_base_type() const { return *this; }

public:
    CachingBaseType& index_access() { return caching_base_type(); }
    const CachingBaseType& index_access() const { return caching_base_type(); }
};
} // namespace wmtk::attribute
#include "Accessor.hxx"


#endif

#pragma once
#include <wmtk/simplex/IdSimplex.hpp>
#include <wmtk/simplex/Simplex.hpp>
#include "CachingAttribute.hpp"
#include "MeshAttributeHandle.hpp"

namespace wmtk::attribute {
/**
 * A CachingAccessor that uses tuples for accessing attributes instead of indices.
 * As global simplex ids should not be publicly available, this accessor uses the Mesh.id() function
 * to map from a tuple to the global simplex id.
 */
template <typename T, typename MeshType = Mesh, typename AttributeType_ = CachingAttribute<T>, int Dim = Eigen::Dynamic>
class Accessor
{
public:
    friend class wmtk::Mesh;
    friend class wmtk::TetMesh;
    friend class wmtk::TriMesh;
    friend class wmtk::EdgeMesh;
    friend class wmtk::PointMesh;
    using Scalar = T;

    using AttributeType = AttributeType_;//CachingAttribute<T>;
    // using CachingBaseType = CachingAccessor<T, Dim>;

    // Eigen::Map<VectorX<T>>
    template <int D = Dim>
    using MapResult = typename AttributeType::MapResult<D>;
    // Eigen::Map<const VectorX<T>>
    template <int D = Dim>
    using ConstMapResult = typename AttributeType::ConstMapResult<D>;


    Accessor(MeshType& m, const TypedAttributeHandle<T>& handle);
    Accessor(const MeshType& m, const TypedAttributeHandle<T>& handle);

    template <typename OMType, typename OAT, int D>
    Accessor(const Accessor<T, OMType, OAT, D>& o);
    auto dimension() const -> int64_t;
    auto reserved_size() const -> int64_t;
    auto default_value() const -> const Scalar&;


    AttributeType& attribute();
    const AttributeType& attribute() const;
    // =============
    // Access methods
    // =============
    // NOTE: If any compilation warnings occur check that there is an overload for an index method

    const T& const_topological_scalar_attribute(const Tuple& t, PrimitiveType pt) const;
    // template <typename ArgType>
    // Scalar& topological_scalar_attribute(const ArgType& t);
    template <typename ArgType>
    const Scalar& const_scalar_attribute(const ArgType& t) const;
    template <typename ArgType>
    Scalar& scalar_attribute(const ArgType& t);

    template <int D = Dim, typename ArgType = wmtk::Tuple>
    ConstMapResult<std::max(D, Dim)> const_vector_attribute(const ArgType& t) const;
    template <int D = Dim, typename ArgType = wmtk::Tuple>
    MapResult<std::max(D, Dim)> vector_attribute(const ArgType& t);


    MeshType& mesh();
    const MeshType& mesh() const;

protected:
    int64_t index(const Tuple& t) const;
    int64_t index(const simplex::Simplex& t) const;
    int64_t index(const simplex::IdSimplex& t) const;

public:
    AttributeType& index_access() { return attribute(); }
    const AttributeType& index_access() const { return attribute(); }
    TypedAttributeHandle<T> m_handle;
    MeshType& m_mesh;
    AttributeType& m_attribute;
    MeshAttributeHandle handle() const;
    const TypedAttributeHandle<T>& typed_handle() const;
    PrimitiveType primitive_type() const;
};
} // namespace wmtk::attribute
#include "Accessor.hxx"
