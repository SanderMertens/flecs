/**
 * @file addons/cpp/mixins/module/mixin.inl
 * @brief Module world mixin.
 */

/** Create a module.
 * 
 * @tparam Module module class.
 * @return Module entity.
 */
template <typename Module>
flecs::entity module(const char *name = nullptr) const;

/** Import a module.
 * 
 * @tparam Module module class.
 * @return Module entity.
 */
template <typename Module>
flecs::entity import();
