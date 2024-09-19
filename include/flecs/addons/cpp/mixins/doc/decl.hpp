/**
 * @file addons/cpp/mixins/doc/decl.hpp
 * @brief Doc mixin declarations.
 */

#pragma once

namespace flecs {
namespace doc {

/**
 * @defgroup cpp_addons_doc Doc
 * @ingroup cpp_addons
 * Utilities for documenting entities, components and systems.
 *
 * @{
 */

/** flecs.doc.Description component */
FLECS_API_STRUCT using Description = EcsDocDescription;

/** flecs.doc.Uuid component */
static const flecs::entity_t Uuid = EcsDocUuid;

/** flecs.doc.Brief component */
FLECS_API_GLOBAL const flecs::entity_t Brief = EcsDocBrief;

/** flecs.doc.Detail component */
FLECS_API_GLOBAL const flecs::entity_t Detail = EcsDocDetail;

/** flecs.doc.Link component */
FLECS_API_GLOBAL const flecs::entity_t Link = EcsDocLink;

/** flecs.doc.Color component */
FLECS_API_GLOBAL const flecs::entity_t Color = EcsDocColor;

/** @private */
namespace _ {
/** @private */
void init(flecs::world& world);
}

/** @} */

}
}
