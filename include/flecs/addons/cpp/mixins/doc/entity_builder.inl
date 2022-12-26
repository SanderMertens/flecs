/**
 * @file addons/cpp/mixins/doc/entity_builder.inl
 * @brief Doc entity builder mixin.
 */

/** Set doc name.
 * This adds (flecs.doc.Description, flecs.Name) to the entity.
 * 
 * \memberof flecs::entity_builder
 * \ingroup cpp_addons_doc
 */
Self& set_doc_name(const char *name) {
    ecs_doc_set_name(m_world, m_id, name);
    return to_base();
}

/** Set doc brief.
 * This adds (flecs.doc.Description, flecs.doc.Brief) to the entity.
 * 
 * \memberof flecs::entity_builder
 * \ingroup cpp_addons_doc
 */
Self& set_doc_brief(const char *brief) {
    ecs_doc_set_brief(m_world, m_id, brief);
    return to_base();
}

/** Set doc detailed description.
 * This adds (flecs.doc.Description, flecs.doc.Detail) to the entity.
 * 
 * \memberof flecs::entity_builder
 * \ingroup cpp_addons_doc
 */
Self& set_doc_detail(const char *detail) {
    ecs_doc_set_detail(m_world, m_id, detail);
    return to_base();
}

/** Set doc link.
 * This adds (flecs.doc.Description, flecs.doc.Link) to the entity.
 * 
 * \memberof flecs::entity_builder
 * \ingroup cpp_addons_doc
 */
Self& set_doc_link(const char *link) {
    ecs_doc_set_link(m_world, m_id, link);
    return to_base();
}

/** Set doc color.
 * This adds (flecs.doc.Description, flecs.doc.Color) to the entity.
 * 
 * \memberof flecs::entity_builder
 * \ingroup cpp_addons_doc
 */
Self& set_doc_color(const char *link) {
    ecs_doc_set_color(m_world, m_id, link);
    return to_base();
}
