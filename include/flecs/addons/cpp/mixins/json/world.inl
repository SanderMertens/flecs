/**
 * @file addons/cpp/mixins/json/world.inl
 * @brief JSON world mixin.
 */

/** Serialize untyped value to JSON.
 * 
 * \memberof flecs::world
 * \ingroup cpp_addons_json
 */
flecs::string to_json(flecs::entity_t tid, const void* value) {
    char *json = ecs_ptr_to_json(m_world, tid, value);
    return flecs::string(json);
}

/** Serialize value to JSON.
 * 
 * \memberof flecs::world
 * \ingroup cpp_addons_json
 */
template <typename T>
flecs::string to_json(const T* value) {
    flecs::entity_t tid = _::cpp_type<T>::id(m_world);
    return to_json(tid, value);
}

/** Serialize world to JSON.
 * 
 * \memberof flecs::world
 * \ingroup cpp_addons_json
 */
flecs::string to_json() {
    return flecs::string( ecs_world_to_json(m_world, nullptr) );
}

/** Deserialize value from JSON.
 * 
 * \memberof flecs::world
 * \ingroup cpp_addons_json
 */
template <typename T>
const char* from_json(flecs::entity_t tid, void* value, const char *json, flecs::from_json_desc_t *desc = nullptr) {
    return ecs_ptr_from_json(m_world, tid, value, json, desc);
}

/** Deserialize value from JSON.
 * 
 * \memberof flecs::world
 * \ingroup cpp_addons_json
 */
template <typename T>
const char* from_json(T* value, const char *json, flecs::from_json_desc_t *desc = nullptr) {
    return ecs_ptr_from_json(m_world, _::cpp_type<T>::id(m_world),
        value, json, desc);
}

/** Deserialize JSON into world.
 * 
 * \memberof flecs::world
 * \ingroup cpp_addons_json
 */
const char* from_json(const char *json, flecs::from_json_desc_t *desc = nullptr) {
    return ecs_world_from_json(m_world, json, desc);
}
