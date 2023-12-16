/**
 * @file addons/cpp/mixins/json/entity_builder.inl
 * @brief JSON entity mixin.
 */

/** Set component from JSON.
 * 
 * \memberof flecs::entity_builder
 * \ingroup cpp_addons_json
 */
Self& set_json(
    flecs::id_t e, 
    const char *json, 
    flecs::from_json_desc_t *desc = nullptr) 
{
    flecs::entity_t type = ecs_get_typeid(m_world, e);
    if (!type) {
        ecs_err("id is not a type");
        return to_base();
    }

    void *ptr = ecs_get_mut_id(m_world, m_id, e);
    ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_ptr_from_json(m_world, type, ptr, json, desc);
    ecs_modified_id(m_world, m_id, e);

    return to_base();
}

/** Set pair from JSON.
 * 
 * \memberof flecs::entity_builder
 * \ingroup cpp_addons_json
 */
Self& set_json(
    flecs::entity_t r, 
    flecs::entity_t t,
    const char *json, 
    flecs::from_json_desc_t *desc = nullptr) 
{
    return set_json(ecs_pair(r, t), json, desc);
}

/** Set component from JSON.
 * 
 * \memberof flecs::entity_builder
 * \ingroup cpp_addons_json
 */
template <typename T>
Self& set_json(
    const char *json, 
    flecs::from_json_desc_t *desc = nullptr) 
{
    return set_json(_::cpp_type<T>::id(m_world), json, desc);
}

/** Set pair from JSON.
 * 
 * \memberof flecs::entity_builder
 * \ingroup cpp_addons_json
 */
template <typename R, typename T>
Self& set_json(
    const char *json, 
    flecs::from_json_desc_t *desc = nullptr) 
{
    return set_json(
        _::cpp_type<R>::id(m_world), 
        _::cpp_type<T>::id(m_world),
        json, desc);
}

/** Set pair from JSON.
 * 
 * \memberof flecs::entity_builder
 * \ingroup cpp_addons_json
 */
template <typename R>
Self& set_json(
    flecs::entity_t t,
    const char *json, 
    flecs::from_json_desc_t *desc = nullptr) 
{
    return set_json(
        _::cpp_type<R>::id(m_world), t,
        json, desc);
}

/** Set pair from JSON.
 * 
 * \memberof flecs::entity_builder
 * \ingroup cpp_addons_json
 */
template <typename T>
Self& set_json_second(
    flecs::entity_t r,
    const char *json, 
    flecs::from_json_desc_t *desc = nullptr) 
{
    return set_json(
        r, _::cpp_type<T>::id(m_world),
        json, desc);
}
