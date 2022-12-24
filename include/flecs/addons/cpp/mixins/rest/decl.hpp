/**
 * @file addons/cpp/mixins/rest/decl.hpp
 * @brief Rest module declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_rest Rest
 * @brief REST API for querying and mutating entities.
 * 
 * \ingroup cpp_addons
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
