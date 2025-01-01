/**
 * @file addons/cpp/mixins/json/entity_view.inl
 * @brief JSON entity mixin.
 */

/** Serialize entity to JSON.
 * 
 * @memberof flecs::entity_view
 * @ingroup cpp_addons_json
 */
flecs::string to_json(const flecs::entity_to_json_desc_t *desc = nullptr) const {
    char *json = ecs_entity_to_json(world_, id_, desc);
    return flecs::string(json);
}
