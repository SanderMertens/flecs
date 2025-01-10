/**
 * @file addons/cpp/ref.hpp
 * @brief Class that caches data to speedup get operations.
 */

#pragma once

namespace flecs
{

/**
 * @defgroup cpp_ref Refs
 * @ingroup cpp_core
 * Refs are a fast mechanism for referring to a specific entity/component.
 *
 * @{
 */

/** Component reference.
 * Reference to a component from a specific entity.
 */
template <typename T>
struct ref {
    ref() : world_(nullptr), ref_{} { }

    ref(world_t *world, entity_t entity, flecs::id_t id = 0)
        : ref_()
    {
        // the world we were called with may be a stage; convert it to a world
        // here if that is the case
        world_ = world ? const_cast<flecs::world_t *>(ecs_get_world(world))
            : nullptr;
        if (!id) {
            id = _::type<T>::id(world);
        }

#ifdef FLECS_DEBUG
        flecs::entity_t type = ecs_get_typeid(world, id);
        const flecs::type_info_t *ti = ecs_get_type_info(world, type);
        ecs_assert(ti && ti->size != 0, ECS_INVALID_PARAMETER,
            "cannot create ref to empty type");
#endif
        ref_ = ecs_ref_init_id(world_, entity, id);
    }

    ref(flecs::entity entity, flecs::id_t id = 0)
        : ref(entity.world(), entity.id(), id) { }

    T* operator->() {
        T* result = static_cast<T*>(ecs_ref_get_id(
            world_, &ref_, this->ref_.id));

        ecs_assert(result != NULL, ECS_INVALID_PARAMETER,
            "nullptr dereference by flecs::ref");

        return result;
    }

    T* get() {
        return static_cast<T*>(ecs_ref_get_id(
            world_, &ref_, this->ref_.id));
    }

    T* try_get() {
        if (!world_ || !ref_.entity) {
            return nullptr;
        }

        return get();
    }

    bool has() {
        return !!try_get();
    }

    /** implicit conversion to bool.  return true if there is a valid T* being referred to **/
    operator bool() {
        return has();
    }

    /** Return entity associated with reference. */
    flecs::entity entity() const;

    /** Return component associated with reference. */
    flecs::id component() const;

private:
    world_t *world_;
    flecs::ref_t ref_;
};

/** @} */

}
