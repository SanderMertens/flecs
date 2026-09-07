/**
 * @file addons/cpp/mixins/event/builder.hpp
 * @brief Event builder.
 */

#pragma once

#define ECS_EVENT_DESC_ID_COUNT_MAX (8)

namespace flecs {

/**
 * @ingroup cpp_addons_event
 * @{
 */

/** Event builder interface. */
template <typename Base, typename E>
struct event_builder_base {
    event_builder_base(flecs::world_t *world, flecs::entity_t event)
        : world_(world)
        , desc_{}
        , ids_{}
        , ids_array_{}
    {
        desc_.event = event;
    }

    template <typename... T, typename... Args>
    Base& id(Args... args) {
        ids_.array = ids_array_;
        ids_.array[ids_.count ++] = _::make_id<T...>(world_, args...).id;
        return *this;
    }

    /** Set the entity for which to emit the event. */
    Base& entity(flecs::entity_t e) {
        desc_.entity = e;
        return *this;
    }

    /** Set the table for which to emit the event. */
    Base& table(flecs::table_t *t, int32_t offset = 0, int32_t count = 0) {
        desc_.table = t;
        desc_.offset = offset;
        desc_.count = count;
        return *this;
    }

    /** Set event data (const). */
    Base& ctx(const E* ptr) {
        desc_.const_param = ptr;
        return *this;
    }

    /** Set event data (mutable). */
    Base& ctx(E* ptr) {
        desc_.param = ptr;
        return *this;
    }

    /** Emit the event. */
    void emit() {
        ids_.array = ids_array_;
        desc_.ids = &ids_;
        desc_.observable = const_cast<flecs::world_t*>(ecs_get_world(world_));
        ecs_emit(world_, &desc_);
    }

    /** Enqueue the event. */
    void enqueue() {
        ids_.array = ids_array_;
        desc_.ids = &ids_;
        desc_.observable = const_cast<flecs::world_t*>(ecs_get_world(world_));
        ecs_enqueue(world_, &desc_);
    }

protected:
    flecs::world_t *world_;
    ecs_event_desc_t desc_;
    flecs::type_t ids_;
    flecs::id_t ids_array_[ECS_EVENT_DESC_ID_COUNT_MAX];

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }
};

/** Untyped event builder. */
struct event_builder : event_builder_base<event_builder, void> {
    using event_builder_base::event_builder_base;
};

/** Typed event builder. */
template <typename E>
struct event_builder_typed : event_builder_base<event_builder_typed<E>, E> {
private:
    using Class = event_builder_typed<E>;

public:
    using event_builder_base<Class, E>::event_builder_base;

    /** Set event data (const reference). */
    Class& ctx(const E& ptr) {
        this->desc_.const_param = &ptr;
        return *this;
    }

    /** Set event data (rvalue reference). */
    Class& ctx(E&& ptr) {
        this->desc_.param = &ptr;
        return *this;
    }
};

/** @} */

}
