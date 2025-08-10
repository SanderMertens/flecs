/**
 * @file addons/cpp/mixins/script/decl.hpp
 * @brief Script declarations.
 */

#pragma once

#include "builder.hpp"

namespace flecs {

/**
 * @defgroup cpp_addons_script Script
 * @ingroup cpp_addons
 *
 * @{
 */

struct script_builder;

using Script = EcsScript;

namespace script {
namespace _ {

void init(flecs::world& world);

} // namespace _
}

/** @} */

}
