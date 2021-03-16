
namespace flecs 
{

////////////////////////////////////////////////////////////////////////////////
//// Entity range, allows for operating on a range of consecutive entities
////////////////////////////////////////////////////////////////////////////////


class ECS_DEPRECATED("do not use") entity_range final : public entity_builder<entity_range> {
public:
    entity_range(const world& world, std::int32_t count) 
        : m_world(world.c_ptr())
        , m_ids( ecs_bulk_new_w_type(m_world, nullptr, count))
        , m_count(count) { }

    entity_range(const world& world, std::int32_t count, flecs::type type) 
        : m_world(world.c_ptr())
        , m_ids( ecs_bulk_new_w_type(m_world, type.c_ptr(), count))
        , m_count(count) { }

    template <typename Func>
    void invoke(Func&& action) const {
        for (int i = 0; i < m_count; i ++) {
            action(m_world, m_ids[i]);
        }
    }

private:
    world_t *m_world;
    const entity_t *m_ids;
    std::int32_t m_count;
};

template <typename T>
flecs::entity ref<T>::entity() const {
    return flecs::entity(m_world, m_entity);
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::add(const entity& entity) const {
    return add(entity.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::add(type type) const {
    return add(type.c_ptr());
}

template <typename base>
template <typename T>
inline typename entity_builder<base>::base_type& entity_builder<base>::add_trait(flecs::entity component) const {
    return add_trait(_::component_info<T>::id(), component.id());
}

template <typename base>
template <typename C>
inline typename entity_builder<base>::base_type& entity_builder<base>::add_trait_tag(flecs::entity trait) const {
    return add_trait(trait.id(), _::component_info<C>::id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::add_trait(flecs::entity trait, flecs::entity entity) const {
    return add_trait(trait.id(), entity.id()); 
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::remove(const entity& entity) const {
    return remove(entity.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::remove(type type) const {
    return remove(type.c_ptr());
}

template <typename base>
template <typename T>
inline typename entity_builder<base>::base_type& entity_builder<base>::remove_trait(flecs::entity component) const {
    return remove_trait(_::component_info<T>::id(), component.id());
}

template <typename base>
template <typename C>
inline typename entity_builder<base>::base_type& entity_builder<base>::remove_trait_tag(flecs::entity trait) const {
    return remove_trait(trait.id(), _::component_info<C>::id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::remove_trait(flecs::entity trait, flecs::entity entity) const {
    return remove_trait(trait.id(), entity.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::add_childof(const entity& entity) const {
    return add_childof(entity.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::remove_childof(const entity& entity) const {
    return remove_childof(entity.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::add_instanceof(const entity& entity) const {
    return add_instanceof(entity.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::remove_instanceof(const entity& entity) const {
    return remove_instanceof(entity.id());
}

template <typename base>
template <typename C>
inline typename entity_builder<base>::base_type& entity_builder<base>::set_trait_tag(flecs::entity trait, const C& value) const
{
    static_cast<base_type*>(this)->invoke(
    [trait, &value](world_t *world, entity_t id) {
        ecs_set_ptr_w_entity(world, id, 
            ecs_trait(_::component_info<C>::id(world), trait.id()),
            sizeof(C), &value);
    });
    return *static_cast<base_type*>(this);
}  

template <typename base>
template <typename T>
inline typename entity_builder<base>::base_type& entity_builder<base>::set_trait(const T& value, flecs::entity tag) const
{
    static_cast<base_type*>(this)->invoke(
    [tag, &value](world_t *world, entity_t id) {
        ecs_set_ptr_w_entity(world, id, 
            ecs_trait(tag.id(), _::component_info<T>::id(world)),
            sizeof(T), &value);
    });
    return *static_cast<base_type*>(this);
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::add_owned(flecs::type type) const {
    return add_owned(type.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::add_switch(const entity& sw) const {
    return add_switch(sw.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::add_switch(const type& sw) const {
    return add_switch(sw.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::remove_switch(const entity& sw) const {
    return remove_switch(sw.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::remove_switch(const type& sw) const {
    return remove_switch(sw.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::add_case(const entity& sw_case) const {
    return add_case(sw_case.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::remove_case(const entity& sw_case) const {
    return remove_case(sw_case.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::enable(const entity& e) const {
    return enable(e.id());
}

template <typename base>
inline typename entity_builder<base>::base_type& entity_builder<base>::disable(const entity& e) const {
    return disable(e.id());
}

inline bool entity::has_switch(flecs::type type) const {
    return ecs_has_entity(m_world, m_id, flecs::Switch | type.id());
}

inline flecs::entity entity::get_case(flecs::type sw) const {
    return flecs::entity(m_world, ecs_get_case(m_world, m_id, sw.id()));
}

}
