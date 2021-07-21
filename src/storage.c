#include "private_api.h"

ecs_storage_t* ecs_storage_init(
    const ecs_storage_plugin_t *plugin,
    ecs_world_t *world,
    ecs_size_t size,
    ecs_size_t alignment)
{
    ecs_assert(plugin != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_storage_t *storage = plugin->init(world, size, alignment);
    ecs_assert(storage->plugin == plugin, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->size == size, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->alignment == alignment, ECS_INVALID_PARAMETER, NULL);
    return storage;
}

void ecs_storage_fini(
    ecs_storage_t *storage)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->plugin != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->plugin->fini != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
    storage->plugin->fini();
}

void* ecs_storage_push(
    ecs_storage_t *storage)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->plugin != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->plugin->push != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
    return storage->plugin->push(storage);
}

void ecs_storage_push_n(
    ecs_storage_t *storage
    int32_t count)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->plugin != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->plugin->push_n != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
    storage->plugin->push_n(storage, count);
}

void ecs_storage_erase(
    ecs_storage_t *storage,
    int32_t index,
    uint64_t id)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->plugin != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->plugin->erase != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
    storage->plugin->erase(storage, index, id);
}

void ecs_storage_ensure(
    ecs_storage_t *storage,
    int32_t index,
    uint64_t id)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->plugin != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->plugin->ensure != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
    storage->plugin->ensure(storage, index, id);
}

void ecs_storage_swap(
    ecs_storage_t *storage,
    int32_t index_a,
    int32_t index_b,
    uint64_t id_a,
    uint64_t id_b)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->plugin != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->plugin->swap != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
    storage->plugin->swap(storage, index_a, index_b, id_a, id_b);
}

void* ecs_storage_get(
    const ecs_storage_t *storage,
    int32_t index,
    uint64_t id)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->plugin != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->plugin->get != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
    return storage->plugin->get(storage, index, id);
}

void ecs_storage_copy(
    const ecs_storage_t *storage,
    int32_t index,
    uint64_t id,
    const void *src)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->plugin != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->plugin->copy != NULL, ECS_MISSING_IMPLEMENTATION, NULL);
    return storage->plugin->copy(storage, index, id, src);
}

bool ecs_storage_has(
    const ecs_storage_t *storage,
    int32_t index,
    uint64_t id)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->plugin != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->plugin->has != NULL, ECS_INVALID_PARAMETER, NULL);
    return storage->plugin->has(storage, index, id);
}

void* ecs_storage_count(
    ecs_storage_t *storage)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->plugin != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->plugin->count != NULL, ECS_INVALID_PARAMETER, NULL);
    return storage->plugin->count(storage);
}  

ecs_storage_iter_t ecs_storage_iter(
    const ecs_storage_t *storage)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->plugin != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->plugin->iter != NULL, ECS_INVALID_PARAMETER, NULL);
    return storage->plugin->iter(storage);
}

bool ecs_storage_next(
    const ecs_storage_t *storage,
    ecs_storage_iter_t *iter)
{
    ecs_assert(storage != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->plugin != NULL, ECS_INVALID_PARAMETER, NULL);
    ecs_assert(storage->plugin->next != NULL, ECS_INVALID_PARAMETER, NULL);
    return storage->plugin->next(storage, iter);
}
