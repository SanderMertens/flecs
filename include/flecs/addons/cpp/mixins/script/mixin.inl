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

/** Run a script.
 * @see ecs_script_run()
 */
int script_run(const char *name, const char *str) const {
    return ecs_script_run(world_, name, str, nullptr);
}

/** Run a script from a file.
 * @see ecs_script_run_file()
 */
int script_run_file(const char *filename) const {
    return ecs_script_run_file(world_, filename);
}

/** Build a script.
 * @see ecs_script_init()
 */
script_builder script(const char *name = nullptr) const {
    return script_builder(world_, name);
}

parsed_script script_parse(const char *name, const char *code,
    const ecs_script_eval_desc_t *desc = nullptr,
    ecs_script_eval_result_t *result = nullptr) const
{
    return parsed_script(ecs_script_parse(world_, name, code, desc, result));
}

int script_update(flecs::entity_t script, const char *code,
    flecs::entity_t instance = 0) const
{
    return ecs_script_update(world_, script, instance, code);
}

function_builder function(const char *name) const {
    return function_builder(world_, name);
}

function_builder method(flecs::entity_t type, const char *name) const {
    return function_builder(world_, name, type, true);
}

template <typename T>
function_builder method(const char *name) const {
    return method(_::type<T>::id(world_), name);
}

template <typename T>
flecs::entity const_var(const char *name, const T& value,
    flecs::entity_t parent = 0) const;

template <typename T>
flecs::entity mut_var(const char *name, const T& value,
    flecs::entity_t parent = 0) const;

template <typename T>
int set_mut_var(const char *name, const T& value) const;

/** Convert a value to a string. */
flecs::string to_expr(flecs::entity_t tid, const void* value) {
    char *expr = ecs_ptr_to_expr(world_, tid, value);
    return flecs::string(expr);
}

/** Convert a value to a string. */
template <typename T>
flecs::string to_expr(const T* value) {
    flecs::entity_t tid = _::type<T>::id(world_);
    return to_expr(tid, value);
}

/** Get the value of an exported script variable.
 * This operation will panic if no const var with the provided name was found,
 * or if the type of the variable cannot be converted to the provided type.
 * 
 * An exported variable can be created in a script like this:
 * 
 * @code
 * export const x: f64 = 10
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

/** Get the value of an exported script variable.
 * This operation will panic if no const var with the provided name was found,
 * or if the type of the variable cannot be converted to the provided type.
 * 
 * An exported variable can be created in a script like this:
 * 
 * @code
 * export const x: f64 = 10
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

/** Get the value of an exported script mut variable.
 * Unlike a const variable, the value of a mut variable is never folded into
 * expressions that use it.
 *
 * An exported mut variable can be created in a script like this:
 *
 * @code
 * export mut x: f64 = 10
 * @endcode
 *
 * See the Flecs script manual for more details.
 *
 * @tparam T The type of the value to obtain.
 * @param name The name of the exported variable.
 * @param default_value Optional default value. Returned when mut var lookup failed.
 * @return The value of the variable.
 */
template <typename T>
T get_mut_var(const char *name, const T& default_value = {}) const;

/** Get the value of an exported script mut variable.
 * Unlike a const variable, the value of a mut variable is never folded into
 * expressions that use it.
 *
 * An exported mut variable can be created in a script like this:
 *
 * @code
 * export mut x: f64 = 10
 * @endcode
 *
 * See the Flecs script manual for more details.
 *
 * @tparam T The type of the value to obtain.
 * @param name The name of the exported variable.
 * @param out Optional pointer to out variable. Can be used to automatically deduce T.
 * @param default_value Optional default value. Returned when mut var lookup failed.
 */
template <typename T>
void get_mut_var(const char *name, T& out, const T& default_value = {}) const;


/** @} */
