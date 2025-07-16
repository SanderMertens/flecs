/**
 * @file addons/cpp/mixins/enum/entity_view.inl
 * @brief Enum entity view mixin.
 */

/** Convert entity to enum constant.
 * 
 * @memberof flecs::entity_view
 * @ingroup cpp_entities
 */
template <typename E>
E to_constant() const;

template <typename TInt>
TInt to_constant(flecs::entity_t type_id) const;

