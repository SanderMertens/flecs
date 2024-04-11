/**
 * @file addons/cpp/mixins/plecs/mixin.inl
 * @brief Plecs world mixin.
 */

/**
 * @defgroup cpp_addons_plecs Plecs
 * @ingroup cpp_addons
 * Data definition format for loading entity data.
 *
 * @{
 */

/** Load plecs string.
 * @see ecs_script_run
 */
int plecs_from_str(const char *name, const char *str) const {
    return ecs_script_run(world_, name, str);
}

/** Load plecs from file.
 * @see ecs_script_run_file
 */
int plecs_from_file(const char *filename) const {
    return ecs_script_run_file(world_, filename);
}

/** @} */
