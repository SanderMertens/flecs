namespace flecs {

template <typename T>
struct module_m : mixin<T> { };

/** Module mixin for flecs::world */
template <>
struct module_m<flecs::world> : mixin<flecs::world> {
  void init() { }

  /** Create a module.
   * 
   * @tparam Module module class.
   * @tparam Args arguments to pass to module constructor.
   * @return Module entity.
   */
  template <typename Module, typename... Args>
  flecs::entity module(Args &&... args) const;

  /** Import a module.
   * 
   * @tparam Module module class.
   * @return Module entity.
   */
  template <typename Module>
  flecs::entity import();
};

using module_m_world = module_m<flecs::world>;

}
