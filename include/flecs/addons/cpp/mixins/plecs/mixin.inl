/**
 * @file addons/cpp/mixins/plecs/mixin.inl
 * @brief Plecs world mixin.
 */

/**
 * @defgroup cpp_addons_plecs Plecs
 * @brief Data definition format for loading entity data.
 * 
 * \ingroup cpp_addons
 * @{
 */

/** Load plecs string.
 * @see ecs_plecs_from_str
 */
int plecs_from_str(const char *name, const char *str) const {
    return ecs_plecs_from_str(m_world, name, str);
}

/** Load plecs from file.
 * @see ecs_plecs_from_file
 */
int plecs_from_file(const char *filename) const {
    return ecs_plecs_from_file(m_world, filename);
}

/** @} */
