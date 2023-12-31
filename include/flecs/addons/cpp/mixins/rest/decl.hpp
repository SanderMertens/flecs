/**
 * @file addons/cpp/mixins/rest/decl.hpp
 * @brief Rest module declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_rest Rest
 * @ingroup cpp_addons
 * REST API for querying and mutating entities.
 *
 * @{
 */

using Rest = EcsRest;

namespace rest {

namespace _ {

void init(flecs::world& world);

}
}

/** @} */

}
