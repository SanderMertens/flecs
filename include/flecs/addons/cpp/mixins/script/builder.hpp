/**
 * @file addons/cpp/mixins/script/builder.hpp
 * @brief Script builder.
 */

#pragma once

namespace flecs {

/**
 * @ingroup cpp_addons_script
 * @{
 */

/** Script builder interface */
struct script_builder {
    script_builder(flecs::world_t *world, const char *name = nullptr)
        : world_(world)
        , desc_{}
    {
        if (name != nullptr) {
            ecs_entity_desc_t entity_desc = {};
            entity_desc.name = name;
            entity_desc.sep = "::";
            entity_desc.root_sep = "::";
            this->desc_.entity = ecs_entity_init(world, &entity_desc);
        }
    }

    script_builder& code(const char *str) {
        desc_.code = str;
        return *this;
    }

    script_builder& filename(const char *str) {
        desc_.filename = str;
        return *this;
    }

    flecs::entity run() const;

protected:
    flecs::world_t *world_;
    ecs_script_desc_t desc_;
};

}
