/**
 * @file addons/cpp/type.hpp
 * @brief Utility functions for ID vector.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_types Types
 * @ingroup cpp_core
 * @brief Type operations.
 *
 * @{
 */

/** Type class.
 * A type is a vector of component IDs that can be requested from entities or tables.
 */
struct type {
    /** Default constructor. Creates an empty type. */
    type() : world_(nullptr), type_(nullptr) { }

    /** Construct a type from a world and C type pointer.
     *
     * @param world The world.
     * @param t Pointer to the C type.
     */
    type(world_t *world, const type_t *t)
        : world_(world)
        , type_(t) { }

    /** Convert the type to a comma-separated string.
     *
     * @return String representation of the type.
     */
    flecs::string str() const {
        return flecs::string(ecs_type_str(world_, type_));
    }

    /** Return the number of IDs in the type.
     *
     * @return The number of IDs.
     */
    int32_t count() const {
        if (!type_) {
            return 0;
        }
        return type_->count;
    }

    /** Return a pointer to the ID array.
     *
     * @return Pointer to the array of IDs, or nullptr if empty.
     */
    flecs::id_t* array() const {
        if (!type_) {
            return nullptr;
        }
        return type_->array;
    }

    /** Get the ID at a specified index in the type.
     *
     * @param index The index of the ID to get.
     * @return The ID at the specified index.
     */
    flecs::id get(int32_t index) const {
        ecs_assert(type_ != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(type_->count > index, ECS_OUT_OF_RANGE, NULL);
        if (!type_) {
            return flecs::id();
        }
        return flecs::id(world_, type_->array[index]);
    }

    /** Get an iterator to the beginning of the type's ID array. */
    const flecs::id_t* begin() const {
        if (type_ && type_->count) {
            return type_->array;
        } else {
            return &empty_;
        }
    }

    /** Get an iterator to the end of the type's ID array. */
    const flecs::id_t* end() const {
        if (type_ && type_->count) {
            return &type_->array[type_->count];
        } else {
            return &empty_;
        }
    }

    /** Implicit conversion to type_t. */
    operator const type_t*() const {
        return type_;
    }
private:
    world_t *world_;
    const type_t *type_;
    flecs::id_t empty_;
};

/** @} */

}
