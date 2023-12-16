/**
 * @file addons/cpp/mixins/json/iterable.inl
 * @brief JSON iterable mixin.
 */

/** Serialize iterator result to JSON.
 * 
 * \memberof flecs::iter
 * \ingroup cpp_addons_json
 */
flecs::string to_json(flecs::iter_to_json_desc_t *desc = nullptr) {
    char *json = ecs_iter_to_json(m_it.real_world, &m_it, desc);
    return flecs::string(json);
}
