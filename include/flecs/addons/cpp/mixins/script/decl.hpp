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

struct parsed_script {
    explicit parsed_script(ecs_script_t *script = nullptr)
        : script_(script) { }

    parsed_script(const parsed_script&) = delete;
    parsed_script& operator=(const parsed_script&) = delete;

    parsed_script(parsed_script&& other) noexcept
        : script_(other.script_)
    {
        other.script_ = nullptr;
    }

    parsed_script& operator=(parsed_script&& other) noexcept {
        if (this != &other) {
            if (script_) {
                ecs_script_free(script_);
            }
            script_ = other.script_;
            other.script_ = nullptr;
        }
        return *this;
    }

    ~parsed_script() {
        if (script_) {
            ecs_script_free(script_);
        }
    }

    explicit operator bool() const {
        return script_ != nullptr;
    }

    int eval(const ecs_script_eval_desc_t *desc = nullptr,
        ecs_script_eval_result_t *result = nullptr) const
    {
        if (!script_) {
            return -1;
        }
        return ecs_script_eval(script_, desc, result);
    }

private:
    ecs_script_t *script_;
};

namespace script {
namespace _ {

void init(flecs::world& world);

} // namespace _
}

/** @} */

}
