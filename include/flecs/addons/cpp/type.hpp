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
    type() : world_(nullptr), type_(nullptr) { }

    type(world_t *world, const type_t *t)
        : world_(world)
        , type_(t) { }

    /** Convert type to comma-separated string */
    flecs::string str() const {
        return flecs::string(ecs_type_str(world_, type_));
    }

    /** Return number of ids in type */
    int32_t count() const {
        if (!type_) {
            return 0;
        }
        return type_->count;
    }

    /** Return pointer to array. */
    flecs::id_t* array() const {
        if (!type_) {
            return nullptr;
        }
        return type_->array;
    }

    /** Get id at specified index in type */
    flecs::id get(int32_t index) const {
        ecs_assert(type_ != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(type_->count > index, ECS_OUT_OF_RANGE, NULL);
        if (!type_) {
            return flecs::id();
        }
        return flecs::id(world_, type_->array[index]);
    }

    flecs::id_t* begin() const {
        return type_->array;
    }

    flecs::id_t* end() const {
        return &type_->array[type_->count];
    }

    /** Implicit conversion to type_t */
    operator const type_t*() const {
        return type_;
    }
private:
    world_t *world_;
    const type_t *type_;
};

/** #} */

}
