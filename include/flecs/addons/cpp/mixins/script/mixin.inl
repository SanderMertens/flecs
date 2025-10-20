/**
 * @file addons/cpp/mixins/script/mixin.inl
 * @brief Script world mixin.
 */

/**
 * @defgroup cpp_addons_script Script
 * @ingroup cpp_addons
 * Data definition format for loading entity data.
 *
 * @{
 */

/** Run script.
 * @see ecs_script_run
 */
int script_run(const char *name, const char *str) const {
    return ecs_script_run(world_, name, str, nullptr);
}

/** Run script from file.
 * @see ecs_script_run_file
 */
int script_run_file(const char *filename) const {
    return ecs_script_run_file(world_, filename);
}

/** Build script.
 * @see ecs_script_init
 */
script_builder script(const char *name = nullptr) const {
    return script_builder(world_, name);
}

/** Convert value to string */
flecs::string to_expr(flecs::entity_t tid, const void* value) {
    char *expr = ecs_ptr_to_expr(world_, tid, value);
    return flecs::string(expr);
}

/** Convert value to string */
template <typename T>
flecs::string to_expr(const T* value) {
    flecs::entity_t tid = _::type<T>::id(world_);
    return to_expr(tid, value);
}

/** Get value of exported script variable.
 * This operation will panic if no const var with the provided name was found,
 * or if the type of the variable cannot be converted to the provided type.
 * 
 * An exported variable can be created in a script like this:
 * 
 * @code
 * export const x = f64: 10
 * @endcode
 * 
 * See the Flecs script manual for more details.
 * 
 * @tparam T The type of the value to obtain.
 * @param name The name of the exported variable.
 * @param default_value Optional default value. Returned when const var lookup failed.
 * @return The value of the variable.
 */
template <typename T>
T get_const_var(const char *name, const T& default_value = {}) const;

/** Get value of exported script variable.
 * This operation will panic if no const var with the provided name was found,
 * or if the type of the variable cannot be converted to the provided type.
 * 
 * An exported variable can be created in a script like this:
 * 
 * @code
 * export const x = f64: 10
 * @endcode
 * 
 * See the Flecs script manual for more details.
 * 
 * @tparam T The type of the value to obtain.
 * @param name The name of the exported variable.
 * @param out Optional pointer to out variable. Can be used to automatically deduce T.
 * @param default_value Optional default value. Returned when const var lookup failed.
 */
template <typename T>
void get_const_var(const char *name, T& out, const T& default_value = {}) const;


/** @} */
