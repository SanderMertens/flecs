
/** Deserialize entity to JSON.
 * 
 * @memberof flecs::entity
 * @ingroup cpp_addons_json
 */
const char* from_json(const char *json) {
    return ecs_entity_from_json(world_, id_, json, nullptr);
}
