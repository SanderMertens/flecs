/**
 * @file storage/sparse_id_record_locks.c
 * @brief Sparse component record lock implementation for mutable alias detection.
 */

#include "../private_api.h"

#ifdef FLECS_MUT_ALIAS_LOCKS

bool flecs_sparse_id_record_read_begin(
    ecs_component_record_t *cr)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    return ++cr->sparse_lock <= 0;
}

bool flecs_sparse_id_record_read_begin_multithreaded(
    ecs_component_record_t *cr)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    return ecs_os_ainc(&cr->sparse_lock) <= 0;
}

bool flecs_sparse_id_record_read_end(
    ecs_component_record_t *cr)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    return --cr->sparse_lock < 0;
}

bool flecs_sparse_id_record_read_end_multithreaded(
    ecs_component_record_t *cr)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    return ecs_os_adec(&cr->sparse_lock) < 0;
}

bool flecs_sparse_id_record_write_begin(
    ecs_component_record_t *cr)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    return --cr->sparse_lock != -1;
}

bool flecs_sparse_id_record_write_begin_multithreaded(
    ecs_component_record_t *cr)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    return ecs_os_adec(&cr->sparse_lock) != -1;
}

bool flecs_sparse_id_record_write_end(
    ecs_component_record_t *cr)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    return ++cr->sparse_lock != 0;
}

bool flecs_sparse_id_record_write_end_multithreaded(
    ecs_component_record_t *cr)
{
    ecs_assert(cr != NULL, ECS_INTERNAL_ERROR, NULL);
    return ecs_os_ainc(&cr->sparse_lock) != 0;
}

#endif
