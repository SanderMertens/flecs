/**
 * @file addons/cpp/mixins/enum/mixin.inl
 * @brief Enum world mixin.
 */

/** Convert enum constant to entity.
 * 
 * \memberof flecs::world
 * \ingroup cpp_entities
 */
template <typename E, if_t< is_enum<E>::value > = 0>
flecs::entity to_entity(E constant) const;
