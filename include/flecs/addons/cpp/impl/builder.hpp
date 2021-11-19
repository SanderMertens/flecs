
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

}
