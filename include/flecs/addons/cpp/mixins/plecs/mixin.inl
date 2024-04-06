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
 * @see ecs_script_from_str
 */
int plecs_from_str(const char *name, const char *str) const {
    return ecs_script_from_str(world_, name, str);
}

/** Load plecs from file.
 * @see ecs_script_from_file
 */
int plecs_from_file(const char *filename) const {
    return ecs_script_from_file(world_, filename);
}

/** @} */
