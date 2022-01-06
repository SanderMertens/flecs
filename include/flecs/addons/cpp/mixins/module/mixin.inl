
/** Create a module.
 * 
 * @tparam Module module class.
 * @return Module entity.
 */
template <typename Module>
flecs::entity module() const;

/** Import a module.
 * 
 * @tparam Module module class.
 * @return Module entity.
 */
template <typename Module>
flecs::entity import();
