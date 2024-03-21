/**
 * @file addons/cpp/mixins/doc/entity_view.inl
 * @brief Doc entity view mixin.
 */

/** Get human readable name.
 *
 * @see ecs_doc_get_name()
 * @see flecs::doc::get_name()
 * @see flecs::entity_builder::set_doc_name()
 *
 * @memberof flecs::entity_view
 * @ingroup cpp_addons_doc
 */
const char* doc_name() const {
    return ecs_doc_get_name(m_world, m_id);
}

/** Get brief description.
 *
 * @see ecs_doc_get_brief()
 * @see flecs::doc::get_brief()
 * @see flecs::entity_builder::set_doc_brief()
 *
 * @memberof flecs::entity_view
 * @ingroup cpp_addons_doc
 */
const char* doc_brief() const {
    return ecs_doc_get_brief(m_world, m_id);
}

/** Get detailed description.
 *
 * @see ecs_doc_get_detail()
 * @see flecs::doc::get_detail()
 * @see flecs::entity_builder::set_doc_detail()
 *
 * @memberof flecs::entity_view
 * @ingroup cpp_addons_doc
 */
const char* doc_detail() const {
    return ecs_doc_get_detail(m_world, m_id);
}

/** Get link to external documentation.
 *
 * @see ecs_doc_get_link()
 * @see flecs::doc::get_link()
 * @see flecs::entity_builder::set_doc_link()
 *
 * @memberof flecs::entity_view
 * @ingroup cpp_addons_doc
 */
const char* doc_link() const {
    return ecs_doc_get_link(m_world, m_id);
}

/** Get color.
 *
 * @see ecs_doc_get_color()
 * @see flecs::doc::get_color()
 * @see flecs::entity_builder::set_doc_color()
 *
 * @memberof flecs::entity_view
 * @ingroup cpp_addons_doc
 */
const char* doc_color() const {
    return ecs_doc_get_color(m_world, m_id);
}
