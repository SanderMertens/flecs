#include "../private_api.h"

void flecs_allocator_init(
    ecs_allocator_t *a)
{
    ecs_map_init(&a->sizes, ecs_block_allocator_t, NULL, 0);
}

void flecs_allocator_fini(
    ecs_allocator_t *a)
{
    ecs_map_iter_t it = ecs_map_iter(&a->sizes);
    ecs_block_allocator_t *ba;
    while ((ba = ecs_map_next(&it, ecs_block_allocator_t, NULL))) {
        flecs_ballocator_fini(ba);
    }
    ecs_map_fini(&a->sizes);
}

ecs_block_allocator_t* flecs_allocator_get(
    ecs_allocator_t *a, 
    ecs_size_t size)
{
    if (!size) {
        return NULL;
    }

    ecs_block_allocator_t *result = ecs_map_get(&a->sizes, 
        ecs_block_allocator_t, size);
    if (!result) {
        result = ecs_map_ensure(&a->sizes, ecs_block_allocator_t, size);
        flecs_ballocator_init(result, size);
    }

    return result;
}
