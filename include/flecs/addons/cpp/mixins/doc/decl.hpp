/**
 * @file addons/cpp/mixins/doc/decl.hpp
 * @brief Doc mixin declarations.
 */

#pragma once

FLECS_API_NAMESPACE namespace flecs {
namespace doc {

/**
 * @defgroup cpp_addons_doc Doc
 * @ingroup cpp_addons
 * Utilities for documenting entities, components and systems.
 *
 * @{
 */

/** flecs.doc.Description component */
using Description = EcsDocDescription;

/** flecs.doc.Uuid component */
FLECS_STATIC_IN_HEADER const flecs::entity_t Uuid = EcsDocUuid;

/** flecs.doc.Brief component */
FLECS_STATIC_IN_HEADER const flecs::entity_t Brief = EcsDocBrief;

/** flecs.doc.Detail component */
FLECS_STATIC_IN_HEADER const flecs::entity_t Detail = EcsDocDetail;

/** flecs.doc.Link component */
FLECS_STATIC_IN_HEADER const flecs::entity_t Link = EcsDocLink;

/** flecs.doc.Color component */
FLECS_STATIC_IN_HEADER const flecs::entity_t Color = EcsDocColor;
} // namespace doc
} // namespace flecs

namespace flecs {

namespace doc {

/** @private */
namespace _ {
/** @private */
void init(flecs::world& world);
}

/** @} */

} // namespace doc
} // namespace flecs
