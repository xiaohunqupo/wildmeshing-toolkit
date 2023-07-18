#include "AttributeScopeStack.hpp"
#include <wmtk/utils/Rational.hpp>
#include "AttributeScope.hpp"
#include "wmtk/Attribute.hpp"

namespace wmtk {

template <typename T>
AttributeScopeStack<T>::AttributeScopeStack() = default;
template <typename T>
AttributeScopeStack<T>::~AttributeScopeStack() = default;
template <typename T>
void AttributeScopeStack<T>::emplace()
{
    // create a new leaf that points to the current stack and
    //
    std::unique_ptr<AttributeScope<T>> new_leaf(new AttributeScope<T>(std::move(m_leaf)));
    m_leaf = std::move(new_leaf);
}
template <typename T>
void AttributeScopeStack<T>::pop(Attribute<T>& attribute)
{
    // delete myself by setting my parent to be the leaf
    assert(bool(m_leaf));
    m_leaf->flush(attribute);
    m_leaf = std::move(m_leaf->pop_parent());
}

template <typename T>
bool AttributeScopeStack<T>::empty() const
{
    return !bool(m_leaf);
}

template <typename T>
AttributeScope<T>* AttributeScopeStack<T>::current_scope_ptr()
{
    if (bool(m_leaf)) {
        return m_leaf.get();
    } else {
        return nullptr;
    }
}

template <typename T>
const AttributeScope<T>* AttributeScopeStack<T>::current_scope_ptr() const
{
    if (bool(m_leaf)) {
        return m_leaf.get();
    } else {
        return nullptr;
    }
}

template class AttributeScopeStack<long>;
template class AttributeScopeStack<double>;
template class AttributeScopeStack<char>;
template class AttributeScopeStack<Rational>;
} // namespace wmtk
