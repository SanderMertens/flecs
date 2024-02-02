#ifndef ${id upper}_H
#define ${id upper}_H

/* This generated file contains includes for project dependencies */
#include "${id dash}/bake_config.h"

// Reflection system boilerplate
#undef ECS_META_IMPL
#ifndef ${id underscore}_EXPORTS
#define ECS_META_IMPL EXTERN // Ensure meta symbols are only defined once
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
ECS_STRUCT(MyComponent) {
    float x;
    float y;
});
*/

void ${id pascalcase}Import(
    ecs_world_t *world);

#ifdef __cplusplus
}
#endif

#endif
