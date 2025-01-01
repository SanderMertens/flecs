/**
 * @file addons/cpp/mixins/json/decl.hpp
 * @brief JSON addon declarations.
 */

#pragma once

namespace flecs {

/**
 * @defgroup cpp_addons_json Json
 * @ingroup cpp_addons
 * Functions for serializing to/from JSON.
 *
 * @{
 */

using from_json_desc_t = ecs_from_json_desc_t;
using entity_to_json_desc_t = ecs_entity_to_json_desc_t;
using iter_to_json_desc_t = ecs_iter_to_json_desc_t;

/** @} */

}
