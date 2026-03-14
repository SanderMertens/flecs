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

/** Return a meta cursor to a value. */
flecs::cursor cursor(flecs::entity_t tid, void *ptr) {
    return flecs::cursor(world_, tid, ptr);
}

/** Return a meta cursor to a value. */
template <typename T>
flecs::cursor cursor(void *ptr) {
    flecs::entity_t tid = _::type<T>::id(world_);
    return cursor(tid, ptr);
}

/** Create a primitive type. */
flecs::entity primitive(flecs::meta::primitive_kind_t kind);

/** Create an array type. */
flecs::entity array(flecs::entity_t elem_id, int32_t array_count);

/** Create an array type. */
template <typename T>
flecs::entity array(int32_t array_count);

/** Create a vector type. */
flecs::entity vector(flecs::entity_t elem_id);

/** Create a vector type. */
template <typename T>
flecs::entity vector();

/** @} */
