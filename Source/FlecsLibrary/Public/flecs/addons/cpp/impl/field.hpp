/**
 * @file addons/cpp/impl/field.hpp
 * @brief Field implementation.
 */

#pragma once

namespace flecs
{

template <typename T>
inline field<T>::field(iter &iter, int32_t index) {
    *this = iter.field<T>(index);
}

template <typename T>
T& field<T>::operator[](size_t index) const {
    ecs_assert(data_ != nullptr, ECS_INVALID_OPERATION, 
        "invalid nullptr dereference of component type %s", 
            _::type_name<T>());
    ecs_assert(index < count_, ECS_COLUMN_INDEX_OUT_OF_RANGE,
        "index %d out of range for array of component type %s",
            index, _::type_name<T>());
    ecs_assert(!index || !is_shared_, ECS_INVALID_PARAMETER,
        "non-zero index invalid for shared field of component type %s",
            _::type_name<T>());
    return data_[index];
}

/** Return first element of component array.
 * This operator is typically used when the field is shared.
 *
 * @return Reference to the first element.
 */
template <typename T>
T& field<T>::operator*() const {
    ecs_assert(data_ != nullptr, ECS_INVALID_OPERATION, 
        "invalid nullptr dereference of component type %s", 
            _::type_name<T>());
    return *data_;
}

/** Return first element of component array.
 * This operator is typically used when the field is shared.
 *
 * @return Pointer to the first element.
 */
template <typename T>
T* field<T>::operator->() const {
    ecs_assert(data_ != nullptr, ECS_INVALID_OPERATION, 
        "invalid nullptr dereference of component type %s", 
            _::type_name<T>());
    ecs_assert(data_ != nullptr, ECS_INVALID_OPERATION, 
        "-> operator invalid for array with >1 element of "
        "component type %s, use [row] instead",
            _::type_name<T>());
    return data_;
}

}
