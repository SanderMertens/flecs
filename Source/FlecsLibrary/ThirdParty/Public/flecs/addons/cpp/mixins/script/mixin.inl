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
    return ecs_script_run(world_, name, str);
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


/** @} */
