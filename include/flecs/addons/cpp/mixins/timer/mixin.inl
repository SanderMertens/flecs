/**
 * @file addons/cpp/mixins/timer/mixin.inl
 * @brief Timer module mixin.
 */

/**
 * \memberof flecs::world
 * \ingroup cpp_addons_timer
 */

/** Find or register a timer. */
template <typename... Args>
flecs::timer timer(Args &&... args) const;
