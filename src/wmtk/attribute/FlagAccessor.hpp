#pragma once

#include "Accessor.hpp"
namespace wmtk::attribute {


template <typename MeshType = wmtk::Mesh>
class IndexFlagAccessor
{
public:
    enum class FlagBit : char { Active = 0x1 };
    static bool _is_active(char value) { return value & static_cast<char>(FlagBit::Active); }
    static void _activate(char& value) { value |= static_cast<char>(FlagBit::Active); }
    static void _deactivate(char& value) { value &= inverse_mask(FlagBit::Active); }
    using BaseAccessor = Accessor<char, MeshType, 1>;


    IndexFlagAccessor(BaseAccessor acc)
        : m_base_accessor{std::move(acc)}
    {}
    template <typename MeshType2>
    IndexFlagAccessor(const IndexFlagAccessor<MeshType2>& o)
        : m_base_accessor{o.m_base_accessor}
    {}

    bool is_active(int64_t t) const
    {
        return _is_active(m_base_accessor.index_access().const_scalar_attribute(t));
    }
    void activate(int64_t t)
    {
        return _activate(m_base_accessor.index_access().scalar_attribute(t));
    }
    void deactivate(int64_t t)
    {
        return _deactivate(m_base_accessor.index_access().scalar_attribute(t));
    }

    constexpr static char inverse_mask(FlagBit bit) { return 0xFF ^ static_cast<char>(bit); }


    BaseAccessor& base_accessor() { return m_base_accessor; }
    const BaseAccessor& base_accessor() const { return m_base_accessor; }
    operator BaseAccessor() const { return m_base_accessor; }

protected:
    BaseAccessor m_base_accessor;


    IndexFlagAccessor index_access() const { return IndexAccessor(*this); }
};

template <typename MeshType = wmtk::Mesh>
class FlagAccessor : private IndexFlagAccessor<MeshType>
{
public:
    enum class FlagBit : char { Active = 1 };
    using IndexBaseType = IndexFlagAccessor<MeshType>;

    using IndexBaseType::base_accessor;
    using IndexBaseType::IndexBaseType;

    template <typename MeshType2>
    FlagAccessor(const FlagAccessor<MeshType2>& o)
        : IndexBaseType{o.base_accessor()}
    {}

    template <typename T>
    bool is_active(const T& t) const
    {
        return IndexBaseType::_is_active(IndexBaseType::m_base_accessor.const_scalar_attribute(t));
    }
    template <typename T>
    void activate(const T& t)
    {
        IndexBaseType::_activate(IndexBaseType::m_base_accessor.scalar_attribute(t));
    }
    template <typename T>
    void deactivate(const T& t)
    {
        IndexBaseType::_deactivate(IndexBaseType::m_base_accessor.scalar_attribute(t));
    }


    const IndexBaseType& index_access() const { return *this; }
    IndexBaseType& index_access() { return *this; }
};


} // namespace wmtk::attribute