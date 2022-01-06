
/** Create a type.
 */
template <typename... Args>
flecs::type type(Args &&... args) const;  

/** Create a type associated with a component.
 */
template <typename T>
flecs::type type(const char *name = nullptr) const;
