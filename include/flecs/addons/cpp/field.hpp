
/**
 * @file addons/cpp/field.hpp
 * @brief Wrapper classes for fields returned by flecs::iter.
 */

#pragma once

/**
 * @defgroup cpp_field Fields
 * @ingroup cpp_core
 * Field helper types.
 *
 * @{
 */

namespace flecs
{

/** Unsafe wrapper class around a field.
 * This class can be used when a system does not know the type of a field at
 * compile time.
 *
 * @ingroup cpp_iterator
 */
struct untyped_field {
    untyped_field(void* array, size_t size, size_t count, bool is_shared = false)
        : data_(array)
        , size_(size)
        , count_(count)
        , is_shared_(is_shared) {}

    /** Return an element in the component array.
     * This operator may only be used if the field is not shared.
     *
     * @param index Index of element.
     * @return Reference to element.
     */
    void* operator[](size_t index) const {
        ecs_assert(!is_shared_ || !index, ECS_INVALID_PARAMETER,
            "invalid usage of [] operator for shared component field");
        ecs_assert(index < count_, ECS_COLUMN_INDEX_OUT_OF_RANGE,
            "index %d out of range for field", index);
        return ECS_OFFSET(data_, size_ * index);
    }

protected:
    void* data_;
    size_t size_;
    size_t count_;
    bool is_shared_;
};

/** Wrapper class around a field.
 *
 * @tparam T Component type of the field.
 *
 * @ingroup cpp_iterator
 */
template <typename T>
struct field {
    static_assert(std::is_empty<T>::value == false,
        "invalid type for field, cannot iterate empty type");

    /** Create a field from a component array.
     *
     * @param array Pointer to the component array.
     * @param count Number of elements in the component array.
     * @param is_shared Whether the component is shared.
     */
    field(T* array, size_t count, bool is_shared = false)
        : data_(array)
        , count_(count)
        , is_shared_(is_shared) {}

    /** Create a field from an iterator.
     *
     * @param iter Iterator object.
     * @param field Index of the field in the query being iterated over.
     */
    field(iter &iter, int field);

    /** Return an element in the component array.
     * This operator may only be used if the field is not shared.
     *
     * @param index Index of element.
     * @return Reference to element.
     */
    T& operator[](size_t index) const;

    /** Return the first element of the component array.
     * This operator is typically used when the field is shared.
     *
     * @return Reference to the first element.
     */
    T& operator*() const;

    /** Return the first element of the component array.
     * This operator is typically used when the field is shared.
     *
     * @return Pointer to the first element.
     */
    T* operator->() const;

protected:
    T* data_;
    size_t count_;
    bool is_shared_;
};

/** Wrapper class around a field matched through component inheritance.
 *
 * Same interface as field, but uses a runtime stride so it can iterate a field
 * whose stored (derived) component is larger than T. Unlike field, this does
 * not assert when the matched id is a subtype of T.
 *
 * @tparam T Base component type of the field.
 *
 * @ingroup cpp_iterator
 */
template <typename T>
struct base_field {
    static_assert(std::is_empty<T>::value == false,
        "invalid type for field, cannot iterate empty type");

    base_field(T* array, size_t stride, size_t count, bool is_shared = false)
        : data_(array)
        , stride_(stride)
        , count_(count)
        , is_shared_(is_shared) {}

    base_field(iter &iter, int field);

    T& operator[](size_t index) const;
    T& operator*() const;
    T* operator->() const;

protected:
    T* data_;
    size_t stride_;
    size_t count_;
    bool is_shared_;
};

} // namespace flecs

/** @} */
