/**
 * @file addons/cpp/mixins/script/builder.hpp
 * @brief Script builder.
 */

#pragma once

namespace flecs {

struct function_builder {
    function_builder(flecs::world_t *world, const char *name,
        flecs::entity_t parent = 0, bool is_method = false)
        : world_(world)
        , desc_{}
        , param_count_(0)
        , is_method_(is_method)
    {
        desc_.name = name;
        desc_.parent = parent ? parent : ecs_get_scope(world);
    }

    function_builder& parent(flecs::entity_t value) {
        desc_.parent = value;
        return *this;
    }

    function_builder& return_type(flecs::entity_t type) {
        desc_.return_type = type;
        return *this;
    }

    template <typename T>
    function_builder& return_type() {
        return return_type(_::type<T>::id(world_));
    }

    function_builder& param(const char *name, flecs::entity_t type) {
        ecs_assert(param_count_ < FLECS_SCRIPT_FUNCTION_ARGS_MAX - is_method_,
            ECS_INVALID_PARAMETER, "too many function parameters");
        desc_.params[param_count_++] = {name, type};
        return *this;
    }

    template <typename T>
    function_builder& param(const char *name) {
        return param(name, _::type<T>::id(world_));
    }

    function_builder& callback(ecs_function_callback_t value) {
        desc_.callback = value;
        return *this;
    }

    function_builder& vector_callback(ecs_primitive_kind_t kind,
        ecs_vector_function_callback_t value)
    {
        ecs_assert(kind > 0 && kind < FLECS_SCRIPT_VECTOR_FUNCTION_COUNT,
            ECS_INVALID_PARAMETER, "invalid vector element kind");
        desc_.vector_callbacks[kind] = value;
        return *this;
    }

    function_builder& vector_callback(flecs::entity_t type,
        ecs_vector_function_callback_t value)
    {
        const EcsPrimitive *primitive = ecs_get(world_, type, EcsPrimitive);
        ecs_assert(primitive != nullptr, ECS_INVALID_PARAMETER,
            "vector element type is not a primitive");
        return vector_callback(primitive->kind, value);
    }

    function_builder& ctx(void *value) {
        desc_.ctx = value;
        return *this;
    }

    flecs::entity build() const;

private:
    flecs::world_t *world_;
    ecs_function_desc_t desc_;
    int32_t param_count_;
    bool is_method_;
};

/**
 * @ingroup cpp_addons_script
 * @{
 */

/** Script builder interface. */
struct script_builder {
    /** Construct a script builder.
     * @param world The world.
     * @param name Optional name for the script entity.
     */
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

    /** Set the script code. */
    script_builder& code(const char *str) {
        desc_.code = str;
        return *this;
    }

    /** Set the script filename. */
    script_builder& filename(const char *str) {
        desc_.filename = str;
        return *this;
    }

    /** Run the script and return the script entity. */
    flecs::entity run() const;

protected:
    flecs::world_t *world_;
    ecs_script_desc_t desc_;
};

}
