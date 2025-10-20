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

/** Untyped component reference.
 * Reference to a component from a specific entity.
 */
struct untyped_ref {

    untyped_ref () : world_(nullptr), ref_{} {}

    untyped_ref(world_t *world, entity_t entity, flecs::id_t id)
        : ref_() {
        ecs_assert(id != 0, ECS_INVALID_PARAMETER,
            "invalid id");
        // the world we were called with may be a stage; convert it to a world
        // here if that is the case
        world_ = world ? const_cast<flecs::world_t *>(ecs_get_world(world))
            : nullptr;

#ifdef FLECS_DEBUG
        flecs::entity_t type = ecs_get_typeid(world, id);
        const flecs::type_info_t *ti = ecs_get_type_info(world, type);
        ecs_assert(ti && ti->size != 0, ECS_INVALID_PARAMETER,
            "cannot create ref to empty type");
#endif
        ref_ = ecs_ref_init_id(world_, entity, id);
    }

    untyped_ref(flecs::entity entity, flecs::id_t id);

    /** Return entity associated with reference. */
    flecs::entity entity() const;

    /** Return component associated with reference. */
    flecs::id component() const {
        return flecs::id(world_, ref_.id);
    }

    void* get() {
        return ecs_ref_get_id(world_, &ref_, this->ref_.id);
    }

    bool has() {
        return !!try_get();
    }

    flecs::world world() const {
        return flecs::world(world_);
    }

    /** implicit conversion to bool.  return true if there is a valid 
     * component instance being referred to **/
    operator bool() {
        return has();
    }

    void* try_get() {
        if (!world_ || !ref_.entity) {
            return nullptr;
        }

        return get();
    }

private:
    world_t *world_;
    flecs::ref_t ref_;
};

/** Component reference.
 * Reference to a component from a specific entity.
 */
template <typename T>
struct ref : public untyped_ref {
    ref() : untyped_ref() { }

    ref(world_t *world, entity_t entity, flecs::id_t id = 0)
        : untyped_ref(world, entity, id ? id : _::type<T>::id(world))
    {    }

    ref(flecs::entity entity, flecs::id_t id = 0);

    T* operator->() {
        T* result = static_cast<T*>(get());

        ecs_assert(result != NULL, ECS_INVALID_PARAMETER,
            "nullptr dereference by flecs::ref");

        return result;
    }

    T* get() {
        return static_cast<T*>(untyped_ref::get());
    }

    T* try_get() {
        return static_cast<T*>(untyped_ref::try_get());
    }
};

/** @} */

}
