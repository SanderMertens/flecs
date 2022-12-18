/**
 * @file addons/cpp/mixins/snapshot/mixin.inl
 * @brief Snapshot world mixin.
 */

/** Create a snapshot.
 */
template <typename... Args>
flecs::snapshot snapshot(Args &&... args) const;
