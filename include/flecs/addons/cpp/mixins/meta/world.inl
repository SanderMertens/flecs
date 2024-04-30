/**
 * @file addons/cpp/mixins/meta/world.inl
 * @brief Meta world mixin.
 */

/**
 * @memberof flecs::world
 * @ingroup cpp_addons_meta
 * 
 * @{
 */

/** Return meta cursor to value */
flecs::cursor cursor(flecs::entity_t tid, void *ptr) {
    return flecs::cursor(world_, tid, ptr);
}

/** Return meta cursor to value */
template <typename T>
flecs::cursor cursor(void *ptr) {
    flecs::entity_t tid = _::type<T>::id(world_);
    return cursor(tid, ptr);
}

/** Create primitive type */
flecs::entity primitive(flecs::meta::primitive_kind_t kind);

/** Create array type. */
flecs::entity array(flecs::entity_t elem_id, int32_t array_count);

/** Create array type. */
template <typename T>
flecs::entity array(int32_t array_count);

/** Create vector type. */
flecs::entity vector(flecs::entity_t elem_id);

/** Create vector type. */
template <typename T>
flecs::entity vector();

/** @} */
