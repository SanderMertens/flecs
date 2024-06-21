/**
 * @file addons/cpp/mixins/json/entity_builder.inl
 * @brief JSON entity mixin.
 */

/** Set component from JSON.
 * 
 * @memberof flecs::entity_builder
 * @ingroup cpp_addons_json
 */
const Self& set_json(
    flecs::id_t e, 
    const char *json, 
    flecs::from_json_desc_t *desc = nullptr) const
{
    flecs::entity_t type = ecs_get_typeid(world_, e);
    if (!type) {
        ecs_err("id is not a type");
        return to_base();
    }

    void *ptr = ecs_ensure_id(world_, id_, e);
    ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);
    ecs_ptr_from_json(world_, type, ptr, json, desc);
    ecs_modified_id(world_, id_, e);

    return to_base();
}

/** Set pair from JSON.
 * 
 * @memberof flecs::entity_builder
 * @ingroup cpp_addons_json
 */
const Self& set_json(
    flecs::entity_t r, 
    flecs::entity_t t,
    const char *json, 
    flecs::from_json_desc_t *desc = nullptr) const
{
    return set_json(ecs_pair(r, t), json, desc);
}

/** Set component from JSON.
 * 
 * @memberof flecs::entity_builder
 * @ingroup cpp_addons_json
 */
template <typename T>
const Self& set_json(
    const char *json, 
    flecs::from_json_desc_t *desc = nullptr) const
{
    return set_json(_::type<T>::id(world_), json, desc);
}

/** Set pair from JSON.
 * 
 * @memberof flecs::entity_builder
 * @ingroup cpp_addons_json
 */
template <typename R, typename T>
const Self& set_json(
    const char *json, 
    flecs::from_json_desc_t *desc = nullptr) const
{
    return set_json(
        _::type<R>::id(world_), 
        _::type<T>::id(world_),
        json, desc);
}

/** Set pair from JSON.
 * 
 * @memberof flecs::entity_builder
 * @ingroup cpp_addons_json
 */
template <typename R>
const Self& set_json(
    flecs::entity_t t,
    const char *json, 
    flecs::from_json_desc_t *desc = nullptr) const
{
    return set_json(
        _::type<R>::id(world_), t,
        json, desc);
}

/** Set pair from JSON.
 * 
 * @memberof flecs::entity_builder
 * @ingroup cpp_addons_json
 */
template <typename T>
const Self& set_json_second(
    flecs::entity_t r,
    const char *json, 
    flecs::from_json_desc_t *desc = nullptr) const
{
    return set_json(
        r, _::type<T>::id(world_),
        json, desc);
}
