/**
 * @file addons/cpp/mixins/term/mixin.inl
 * @brief Term world mixin.
 */

/**
 * \memberof flecs::world
 * \ingroup cpp_core_filters
 */

/** Create a term.
 * 
 */
template<typename... Args>
flecs::term term(Args &&... args) const;

/** Create a term for a (component) type.
 */
template<typename T>
flecs::term term() const;  

/** Create a term for a pair.
 */
template<typename First, typename Second>
flecs::term term() const;

/** @} */
