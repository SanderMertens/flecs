
namespace flecs 
{

template <typename ... Components>
inline query_builder<Components...>::operator query<Components ...>() const {
    ecs_query_t *query = *this;
    return flecs::query<Components...>(m_world, query);
}

template <typename ... Components>
inline query<Components ...> query_builder<Components...>::build() const {
    ecs_query_t *query = *this;
    return flecs::query<Components...>(m_world, query);
}

template <typename Base, typename ... Components>
inline Base& query_builder_i<Base, Components ...>::parent(const query_base& parent) {
    m_desc->parent = parent.c_ptr();
    return *static_cast<Base*>(this);
}

}
