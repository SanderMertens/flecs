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

/** Event builder interface */
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

    /** Add component to emit for */
    template <typename T>
    Base& id() {
        ids_.array = ids_array_;
        ids_.array[ids_.count] = _::type<T>().id(world_);
        ids_.count ++;
        return *this;
    }
    
    /** 
     * Add pair to emit for
     * @tparam First The first element of the pair.
     * @tparam Second the second element of a pair.
     */
    template <typename First, typename Second>
    Base& id() {
        return id(
            ecs_pair(_::type<First>::id(this->world_), 
                _::type<Second>::id(this->world_)));
    }

    /** 
     * Add pair to emit for
     * @tparam First The first element of the pair.
     * @param second The second element of the pair id.
     */
    template <typename First>
    Base& id(entity_t second) {
        return id(ecs_pair(_::type<First>::id(this->world_), second));
    }

    /** 
     * Add pair to emit for
     * @param first The first element of the pair type.
     * @param second The second element of the pair id.
     */
    Base& id(entity_t first, entity_t second) {
        return id(ecs_pair(first, second));
    }

    template <typename Enum, if_t<is_enum<Enum>::value> = 0>
    Base& id(Enum value) {
        const auto& et = enum_type<Enum>(this->world_);
        flecs::entity_t target = et.entity(value);
        return id(et.entity(), target);
    }

    /** Add (component) id to emit for */
    Base& id(flecs::id_t id) {
        ids_.array = ids_array_;
        ids_.array[ids_.count] = id;
        ids_.count ++;
        return *this;
    }

    /** Set entity for which to emit event */
    Base& entity(flecs::entity_t e) {
        desc_.entity = e;
        return *this;
    }

    /* Set table for which to emit event */
    Base& table(flecs::table_t *t, int32_t offset = 0, int32_t count = 0) {
        desc_.table = t;
        desc_.offset = offset;
        desc_.count = count;
        return *this;
    }

    /* Set event data */
    Base& ctx(const E* ptr) {
        desc_.const_param = ptr;
        return *this;
    }

    /* Set event data */
    Base& ctx(E* ptr) {
        desc_.param = ptr;
        return *this;
    }

    void emit() {
        ids_.array = ids_array_;
        desc_.ids = &ids_;
        desc_.observable = const_cast<flecs::world_t*>(ecs_get_world(world_));
        ecs_emit(world_, &desc_);
    }

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

struct event_builder : event_builder_base<event_builder, void> {
    using event_builder_base::event_builder_base;
};

template <typename E>
struct event_builder_typed : event_builder_base<event_builder_typed<E>, E> {
private:
    using Class = event_builder_typed<E>;

public:
    using event_builder_base<Class, E>::event_builder_base;

    /* Set event data */
    Class& ctx(const E& ptr) {
        this->desc_.const_param = &ptr;
        return *this;
    }

    /* Set event data */
    Class& ctx(E&& ptr) {
        this->desc_.param = &ptr;
        return *this;
    }
};

/** @} */

}
