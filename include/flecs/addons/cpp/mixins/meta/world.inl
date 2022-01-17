
flecs::string to_expr(flecs::entity_t tid, const void* value) {
    char *expr = ecs_ptr_to_expr(m_world, tid, value);
    return flecs::string(expr);
}

template <typename T>
flecs::string to_expr(const T* value) {
    flecs::entity_t tid = _::cpp_type<T>::id(m_world);
    return to_expr(tid, value);
}

flecs::meta::cursor cursor(flecs::entity_t tid, void *ptr) {
    return flecs::meta::cursor(m_world, tid, ptr);
}

template <typename T>
flecs::meta::cursor cursor(void *ptr) {
    flecs::entity_t tid = _::cpp_type<T>::id(m_world);
    return cursor(tid, ptr);
}
