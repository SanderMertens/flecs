/**
 * @file addons/cpp/mixins/snapshot/mixin.inl
 * @brief Snapshot world mixin.
 */

/**
 * \memberof flecs::world
 * \ingroup cpp_addons_snapshot
 */

/** Create a snapshot.
 */
template <typename... Args>
flecs::snapshot snapshot(Args &&... args) const;

/** @} */
