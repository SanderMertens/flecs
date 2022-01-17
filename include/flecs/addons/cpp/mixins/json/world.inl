
flecs::string to_json(flecs::entity_t tid, const void* value) {
    char *json = ecs_ptr_to_json(m_world, tid, value);
    return flecs::string(json);
}

template <typename T>
flecs::string to_json(const T* value) {
    flecs::entity_t tid = _::cpp_type<T>::id(m_world);
    return to_json(tid, value);
}
