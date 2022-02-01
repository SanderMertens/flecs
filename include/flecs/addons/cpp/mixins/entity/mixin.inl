
/** Create an entity.
 */
template <typename... Args>
flecs::entity entity(Args &&... args) const;

/** Get id from an enum constant.
 */
template <typename E, if_t< is_enum<E>::value > = 0>
flecs::entity id(E value) const;

/** Get id from an enum constant.
 */
template <typename E, if_t< is_enum<E>::value > = 0>
flecs::entity entity(E value) const;

/** Create a prefab.
 */
template <typename... Args>
flecs::entity prefab(Args &&... args) const;

/** Create an entity that's associated with a type.
 */
template <typename T>
flecs::entity entity(const char *name = nullptr) const;

/** Create a prefab that's associated with a type.
 */
template <typename T>
flecs::entity prefab(const char *name = nullptr) const;
