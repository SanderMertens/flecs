/**
 * @file addons/cpp/mixins/doc/impl.hpp
 * @brief Doc mixin implementation.
 */

#pragma once

namespace flecs {
namespace doc {

/** Get human readable name for an entity.
 *
 * @see ecs_doc_get_name()
 * @see flecs::doc::set_name()
 * @see flecs::entity_view::doc_name()
 *
 * @ingroup cpp_addons_doc
 */
inline const char* get_name(const flecs::entity_view& e) {
    return ecs_doc_get_name(e.world(), e);
}

/** Get brief description for an entity.
 *
 * @see ecs_doc_get_brief()
 * @see flecs::doc::set_brief()
 * @see flecs::entity_view::doc_brief()
 *
 * @ingroup cpp_addons_doc
 */
inline const char* get_brief(const flecs::entity_view& e) {
    return ecs_doc_get_brief(e.world(), e);
}

/** Get detailed description for an entity.
 *
 * @see ecs_doc_get_detail()
 * @see flecs::doc::set_detail()
 * @see flecs::entity_view::doc_detail()
 *
 * @ingroup cpp_addons_doc
 */
inline const char* get_detail(const flecs::entity_view& e) {
    return ecs_doc_get_detail(e.world(), e);
}

/** Get link to external documentation for an entity.
 *
 * @see ecs_doc_get_link()
 * @see flecs::doc::set_link()
 * @see flecs::entity_view::doc_link()
 *
 * @ingroup cpp_addons_doc
 */
inline const char* get_link(const flecs::entity_view& e) {
    return ecs_doc_get_link(e.world(), e);
}

/** Get color for an entity.
 *
 * @see ecs_doc_get_color()
 * @see flecs::doc::set_color()
 * @see flecs::entity_view::doc_color()
 *
 * @ingroup cpp_addons_doc
 */
inline const char* get_color(const flecs::entity_view& e) {
    return ecs_doc_get_color(e.world(), e);
}

/** Set human readable name for an entity.
 *
 * @see ecs_doc_set_name()
 * @see flecs::doc::get_name()
 * @see flecs::entity_builder::set_doc_name()
 *
 * @ingroup cpp_addons_doc
 */
inline void set_name(flecs::entity& e, const char *name) {
    ecs_doc_set_name(e.world(), e, name);
}

/** Set brief description for an entity.
 *
 * @see ecs_doc_set_brief()
 * @see flecs::doc::get_brief()
 * @see flecs::entity_builder::set_doc_brief()
 *
 * @ingroup cpp_addons_doc
 */
inline void set_brief(flecs::entity& e, const char *description) {
    ecs_doc_set_brief(e.world(), e, description);
}

/** Set detailed description for an entity.
 *
 * @see ecs_doc_set_detail()
 * @see flecs::doc::get_detail()
 * @see flecs::entity_builder::set_doc_detail()
 *
 * @ingroup cpp_addons_doc
 */
inline void set_detail(flecs::entity& e, const char *description) {
    ecs_doc_set_detail(e.world(), e, description);
}

/** Set link to external documentation for an entity.
 *
 * @see ecs_doc_set_link()
 * @see flecs::doc::get_link()
 * @see flecs::entity_builder::set_doc_link()
 *
 * @ingroup cpp_addons_doc
 */
inline void set_link(flecs::entity& e, const char *link) {
    ecs_doc_set_link(e.world(), e, link);
}

/** Set color for an entity.
 *
 * @see ecs_doc_set_color()
 * @see flecs::doc::get_color()
 * @see flecs::entity_builder::set_doc_color()
 *
 * @ingroup cpp_addons_doc
 */
inline void set_color(flecs::entity& e, const char *color) {
    ecs_doc_set_color(e.world(), e, color);
}

/** @private */
namespace _ {

/** @private */
inline void init(flecs::world& world) {
    world.component<doc::Description>("flecs::doc::Description");
}

} // namespace _
} // namespace doc
} // namespace flecs
