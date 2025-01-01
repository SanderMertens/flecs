
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

    /** Return element in component array.
     * This operator may only be used if the field is not shared.
     *
     * @param index Index of element.
     * @return Reference to element.
     */
    void* operator[](size_t index) const {
        ecs_assert(!is_shared_, ECS_INVALID_PARAMETER,
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
 * @tparam T component type of the field.
 *
 * @ingroup cpp_iterator
 */
template <typename T>
struct field {
    static_assert(std::is_empty<T>::value == false,
        "invalid type for field, cannot iterate empty type");

    /** Create field from component array.
     *
     * @param array Pointer to the component array.
     * @param count Number of elements in component array.
     * @param is_shared Is the component shared or not.
     */
    field(T* array, size_t count, bool is_shared = false)
        : data_(array)
        , count_(count)
        , is_shared_(is_shared) {}

    /** Create field from iterator.
     *
     * @param iter Iterator object.
     * @param field Index of the signature of the query being iterated over.
     */
    field(iter &iter, int field);

    /** Return element in component array.
     * This operator may only be used if the field is not shared.
     *
     * @param index Index of element.
     * @return Reference to element.
     */
    T& operator[](size_t index) const;

    /** Return first element of component array.
     * This operator is typically used when the field is shared.
     *
     * @return Reference to the first element.
     */
    T& operator*() const;

    /** Return first element of component array.
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

} // namespace flecs

/** @} */
