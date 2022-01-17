
flecs::string to_json(const flecs::entity_to_json_desc_t *desc = nullptr) {
    char *json = ecs_entity_to_json(m_world, m_id, desc);
    return flecs::string(json);
}
