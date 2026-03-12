/**
 * @file addons/cpp/type.hpp
 * @brief Utility functions for id vector.
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
 * A type is a vector of component ids which can be requested from entities or tables.
 */
struct type {
    /** Default constructor. Creates an empty type. */
    type() : world_(nullptr), type_(nullptr) { }

    /** Construct type from world and C type pointer.
     *
     * @param world The world.
     * @param t Pointer to the C type.
     */
    type(world_t *world, const type_t *t)
        : world_(world)
        , type_(t) { }

    /** Convert type to comma-separated string.
     *
     * @return String representation of the type.
     */
    flecs::string str() const {
        return flecs::string(ecs_type_str(world_, type_));
    }

    /** Return number of ids in type.
     *
     * @return The number of ids.
     */
    int32_t count() const {
        if (!type_) {
            return 0;
        }
        return type_->count;
    }

    /** Return pointer to the id array.
     *
     * @return Pointer to the array of ids, or nullptr if empty.
     */
    flecs::id_t* array() const {
        if (!type_) {
            return nullptr;
        }
        return type_->array;
    }

    /** Get id at specified index in type.
     *
     * @param index The index of the id to get.
     * @return The id at the specified index.
     */
    flecs::id get(int32_t index) const {
        ecs_assert(type_ != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(type_->count > index, ECS_OUT_OF_RANGE, NULL);
        if (!type_) {
            return flecs::id();
        }
        return flecs::id(world_, type_->array[index]);
    }

    /** Get iterator to the beginning of the type's id array. */
    const flecs::id_t* begin() const {
        if (type_ && type_->count) {
            return type_->array;
        } else {
            return &empty_;
        }
    }

    /** Get iterator to the end of the type's id array. */
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
