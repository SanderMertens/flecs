/**
 * @file addons/cpp/mixins/doc/entity_builder.inl
 * @brief Doc entity builder mixin.
 */

/** Set human readable name.
 * This adds `(flecs.doc.Description, flecs.Name)` to the entity.
 *
 * @see ecs_doc_set_name()
 * @see flecs::doc::set_name()
 * @see flecs::entity_view::doc_name()
 *
 * @memberof flecs::entity_builder
 * @ingroup cpp_addons_doc
 */
const Self& set_doc_name(const char *name) const {
    ecs_doc_set_name(world_, id_, name);
    return to_base();
}

/** Set brief description.
 * This adds `(flecs.doc.Description, flecs.doc.Brief)` to the entity.
 *
 * @see ecs_doc_set_brief()
 * @see flecs::doc::set_brief()
 * @see flecs::entity_view::doc_brief()
 *
 * @memberof flecs::entity_builder
 * @ingroup cpp_addons_doc
 */
const Self& set_doc_brief(const char *brief) const {
    ecs_doc_set_brief(world_, id_, brief);
    return to_base();
}

/** Set detailed description.
 * This adds `(flecs.doc.Description, flecs.doc.Detail)` to the entity.
 *
 * @see ecs_doc_set_detail()
 * @see flecs::doc::set_detail()
 * @see flecs::entity_view::doc_detail()
 *
 * @memberof flecs::entity_builder
 * @ingroup cpp_addons_doc
 */
const Self& set_doc_detail(const char *detail) const {
    ecs_doc_set_detail(world_, id_, detail);
    return to_base();
}

/** Set link to external documentation.
 * This adds `(flecs.doc.Description, flecs.doc.Link)` to the entity.
 *
 * @see ecs_doc_set_link()
 * @see flecs::doc::set_link()
 * @see flecs::entity_view::doc_link()
 *
 * @memberof flecs::entity_builder
 * @ingroup cpp_addons_doc
 */
const Self& set_doc_link(const char *link) const {
    ecs_doc_set_link(world_, id_, link);
    return to_base();
}

/** Set doc color.
 * This adds `(flecs.doc.Description, flecs.doc.Color)` to the entity.
 *
 * @see ecs_doc_set_color()
 * @see flecs::doc::set_color()
 * @see flecs::entity_view::doc_color()
 *
 * @memberof flecs::entity_builder
 * @ingroup cpp_addons_doc
 */
const Self& set_doc_color(const char *link) const {
    ecs_doc_set_color(world_, id_, link);
    return to_base();
}
