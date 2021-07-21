#include "private_api.h"

typedef struct ecs_bitset_storage_t {
    ecs_bitset_t bitset;
    ecs_vector_t *data;
} ecs_bitset_storage_t;

ecs_storage_plugin_t ecs_bitset_storage_plugin(void) 
{
    return (ecs_storage_plugin_t) {
        .init = ecs_bitset_storage_init,
        .fini = ecs_bitset_storage_fini,
        .push = ecs_bitset_storage_push,
        .erase = ecs_bitset_storage_erase,
        .swap = ecs_bitset_storage_swap,
        .get = ecs_bitset_storage_get,
        .has = ecs_bitset_storage_has,
    };
}

ecs_storage_t* ecs_bitset_storage_init(
    ecs_world_t *world,
    ecs_size_t size,
    ecs_size_t alignment)
{
    (void)world;
    
    ecs_bitset_storage_t *result = ecs_os_calloc_t(ecs_bitset_storage_t);
    ecs_assert(result != NULL, ECS_OUT_OF_MEMORY, NULL);

    ecs_bitset_init(&result->bitset);

    if (size) {
        result->data = ecs_vector_new_t(size, alignment, 0);
    }

    return (ecs_storage_t*)result;
}

void ecs_bitset_storage_fini(
    ecs_storage_t *storage)
{
    ecs_bitset_storage_t *ptr = (ecs_bitset_storage_t*)storage;
    ecs_bitset_fini(&ptr->bitset);
    ecs_vector_free(ptr->data);
    ecs_os_free(ptr);
}

void* ecs_bitset_storage_push(
    ecs_storage_t *storage,
    ecs_size_t size,
    ecs_size_t alignment,
    uint64_t id)
{
    (void)id;

    ecs_bitset_storage_t *ptr = (ecs_bitset_storage_t*)storage;
    ecs_bitset_addn(&ptr->bitset, 1);
    
    if (size) {
        return ecs_vector_add_t(&ptr->data, size, alignment);
    }

    return NULL;
}

void ecs_bitset_storage_erase(
    ecs_storage_t *storage,
    ecs_size_t size,
    ecs_size_t alignment,
    int32_t index,
    uint64_t id)
{
    (void)id;

    ecs_bitset_storage_t *ptr = (ecs_bitset_storage_t*)storage;
    ecs_bitset_remove(&ptr->bitset, index);
    
    if (size) {
        ecs_vector_remove_t(ptr->data, size, alignment, index);
    }
}

void ecs_bitset_storage_swap(
    ecs_storage_t *storage,
    ecs_size_t size,
    ecs_size_t alignment,
    int32_t index_a,
    int32_t index_b,
    uint64_t id_a,
    uint64_t id_b)
{
    (void)id_a;
    (void)id_b;

    ecs_bitset_storage_t *ptr = (ecs_bitset_storage_t*)storage;
    ecs_bitset_swap(&ptr->bitset, index_a, index_b);

    if (size) {
        void *tmp = ecs_os_alloca(size);
        void *arr = ecs_vector_first_t(ptr->data, size, alignment);
        void *el_a = ECS_OFFSET(arr, size * index_a);
        void *el_b = ECS_OFFSET(arr, size * index_b);

        ecs_os_memcpy(tmp, el_a, size);
        ecs_os_memcpy(el_a, el_b, size);
        ecs_os_memcpy(el_b, tmp, size);
    }
}

void* ecs_bitset_storage_get(
    const ecs_storage_t *storage,
    ecs_size_t size,
    ecs_size_t alignment,
    int32_t index,
    uint64_t id)
{
    (void)id;

    ecs_assert(size != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_bitset_storage_t *ptr = (ecs_bitset_storage_t*)storage;
    if (ecs_bitset_get(&ptr->bitset, index)) {
        return ecs_vector_get_t(ptr->data, size, alignment, index);
    }

    return NULL;
}

bool ecs_bitset_storage_has(
    const ecs_storage_t *storage,
    ecs_size_t size,
    ecs_size_t alignment,
    int32_t index,
    uint64_t id)
{
    (void)id;
    (void)size;
    (void)alignment;

    ecs_bitset_storage_t *ptr = (ecs_bitset_storage_t*)storage;
    return ecs_bitset_get(&ptr->bitset, index);
}
