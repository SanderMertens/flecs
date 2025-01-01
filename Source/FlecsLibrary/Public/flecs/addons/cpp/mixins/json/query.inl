
/** Serialize query to JSON.
 * 
 * @memberof flecs::query_base
 * @ingroup cpp_addons_json
 */
flecs::string to_json(flecs::iter_to_json_desc_t *desc = nullptr) {
    ecs_iter_t it = ecs_query_iter(ecs_get_world(query_), query_);
    char *json = ecs_iter_to_json(&it, desc);
    return flecs::string(json);
}