/**
 * @file addons/cpp/mixins/doc/decl.hpp
 * @brief Doc mixin declarations.
 */

#pragma once

namespace flecs {
namespace doc {

/**
 * @defgroup cpp_addons_doc Doc
 * @brief Utilities for documenting entities, components and systems.
 * 
 * \ingroup cpp_addons
 * @{
 */

/** flecs.doc.Description component */
using Description = EcsDocDescription;

/** flecs.doc.Brief component */
static const flecs::entity_t Brief = EcsDocBrief;

/** flecs.doc.Detail component */
static const flecs::entity_t Detail = EcsDocDetail;

/** flecs.doc.Link component */
static const flecs::entity_t Link = EcsDocLink;

/** flecs.doc.Color component */
static const flecs::entity_t Color = EcsDocColor;

namespace _ {
void init(flecs::world& world);
}

/** @} */

}
}
