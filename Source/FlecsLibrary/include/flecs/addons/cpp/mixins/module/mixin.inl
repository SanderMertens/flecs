/**
 * @file addons/cpp/mixins/module/mixin.inl
 * @brief Module world mixin.
 */

/** 
 * \memberof flecs::world
 * \ingroup cpp_addons_modules
 * 
 * @{
 */

/** Define a module.
 * This operation is not mandatory, but can be called inside the module ctor to
 * obtain the entity associated with the module, or override the module name.
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

/** @} */
