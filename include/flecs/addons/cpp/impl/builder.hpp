
namespace flecs 
{

template<typename Base>
inline Base& term_builder_i<Base>::id(const flecs::type& type) {
    ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
    m_term->pred.entity = type.id();
    return *this;
}

template<typename Base>
inline Base& term_builder_i<Base>::id(const flecs::type& type, id_t o) {
    ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
    m_term->pred.entity = type.id();
    m_term->obj.entity = o;
    m_term->role = ECS_PAIR;
    return *this;
}

template <typename ... Components>
inline filter_builder_base<Components...>::operator filter<Components ...>() const {
    ecs_filter_t filter = *this;
    return flecs::filter<Components...>(m_world, &filter);
}

template <typename ... Components>
inline filter_builder<Components ...>::operator filter<>() const {
    ecs_filter_t filter = *this;
    return flecs::filter<>(this->m_world, &filter);
}

template <typename ... Components>
inline filter<Components ...> filter_builder_base<Components...>::build() const {
    ecs_filter_t filter = *this;
    return flecs::filter<Components...>(m_world, &filter);
}

template <typename ... Components>    
template <typename Func>
inline observer<Components ...> observer_builder<Components...>::iter(Func&& func) const {
    using Invoker = typename _::iter_invoker<
        typename std::decay<Func>::type, Components...>;
    flecs::entity_t observer = build<Invoker>(std::forward<Func>(func), false);
    return flecs::observer<Components...>(m_world, observer);
}

template <typename ... Components>    
template <typename Func>
inline observer<Components ...> observer_builder<Components...>::each(Func&& func) const {
    using Invoker = typename _::each_invoker<
        typename std::decay<Func>::type, Components...>;
    flecs::entity_t observer = build<Invoker>(std::forward<Func>(func), true);
    return flecs::observer<Components...>(m_world, observer);
}

template <typename ... Components>    
template <typename Func>
inline trigger<Components ...> trigger_builder<Components...>::iter(Func&& func) {
    using Invoker = typename _::iter_invoker<
        typename std::decay<Func>::type, Components...>;
    flecs::entity_t trigger = build<Invoker>(std::forward<Func>(func));
    return flecs::trigger<Components...>(m_world, trigger);
}

template <typename ... Components>    
template <typename Func>
inline trigger<Components ...> trigger_builder<Components...>::each(Func&& func) {
    using Invoker = typename _::each_invoker<
        typename std::decay<Func>::type, Components...>;
    flecs::entity_t trigger = build<Invoker>(std::forward<Func>(func));
    return flecs::trigger<Components...>(m_world, trigger);
}

}
