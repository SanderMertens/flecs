/**
 * @file addons/cpp/type.hpp
 * @brief Utility functions for id vector.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_types Types
 * @brief Type operations.
 * 
 * \ingroup cpp_core
 * @{
 */

/** Type class.
 * A type is a vector of component ids which can be requested from entities or tables.
 */
struct type {
    type() : m_world(nullptr), m_type(nullptr) { }

    type(world_t *world, const type_t *t)
        : m_world(world)
        , m_type(t) { }

    /** Convert type to comma-separated string */
    flecs::string str() const {
        return flecs::string(ecs_type_str(m_world, m_type));
    }

    /** Return number of ids in type */
    int32_t count() const {
        if (!m_type) {
            return 0;
        }
        return m_type->count;
    }

    /** Return pointer to array. */
    flecs::id_t* array() const {
        if (!m_type) {
            return nullptr;
        }
        return m_type->array;
    }

    /** Get id at specified index in type */
    flecs::id get(int32_t index) const {
        ecs_assert(m_type != NULL, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_type->count > index, ECS_OUT_OF_RANGE, NULL);
        if (!m_type) {
            return flecs::id();
        }
        return flecs::id(m_world, m_type->array[index]);
    }

    /** Implicit conversion to type_t */
    operator const type_t*() const {
        return m_type;
    }
private:
    world_t *m_world;
    const type_t *m_type;
};

/** #} */

}
