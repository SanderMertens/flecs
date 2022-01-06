/** Create a term.
 * 
 */
template<typename... Args>
flecs::term term(Args &&... args) const;

/** Create a term for a (component) type.
 */
template<typename T, typename... Args>
flecs::term term(Args &&... args) const;  

/** Create a term for a pair.
 */
template<typename R, typename O, typename... Args>
flecs::term term(Args &&... args) const;
