#include <api.h>

#define test_delta(prev, cur, field, value)\
    test_int(value, (cur)->field - (prev)->field);\
    (prev)->field = (cur)->field

void WorldInfo_get_tick() {
    ecs_world_t *world = ecs_init();

    const ecs_world_info_t *info = ecs_get_world_info(world);
    test_assert(info != NULL);

    test_int(info->frame_count_total, 0);
    ecs_progress(world, 1);
    test_int(info->frame_count_total, 1);
    ecs_progress(world, 1);
    test_int(info->frame_count_total, 2);

    ecs_fini(world);
}

void WorldInfo_table_count() {
    ecs_world_t *world = ecs_mini();

    const ecs_world_info_t *cur = ecs_get_world_info(world);
    test_assert(cur != NULL);

    ecs_world_info_t prev = *cur;

    ecs_entity_t c = ecs_new_id(world);
    ecs_entity_t e = ecs_new_id(world);
    test_delta(&prev, cur, table_count, 0);

    ecs_add_id(world, e, c);
    test_delta(&prev, cur, table_count, 1);

    ecs_delete(world, c);
    test_delta(&prev, cur, table_count, -1);

    ecs_fini(world);
}

void WorldInfo_empty_table_count() {
    ecs_world_t *world = ecs_mini();

    const ecs_world_info_t *cur = ecs_get_world_info(world);
    test_assert(cur != NULL);

    ecs_world_info_t prev = *cur;

    ecs_entity_t c = ecs_new_id(world);
    ecs_entity_t e = ecs_new_id(world);

    ecs_add_id(world, e, c);
    ecs_run_aperiodic(world, 0);
    test_delta(&prev, cur, empty_table_count, 0);

    ecs_delete(world, e);
    ecs_run_aperiodic(world, 0);
    test_delta(&prev, cur, empty_table_count, 1);

    ecs_delete(world, c);
    ecs_run_aperiodic(world, 0);
    test_delta(&prev, cur, empty_table_count, -1);

    ecs_fini(world);
}

void WorldInfo_tag_table_count() {
    ecs_world_t *world = ecs_mini();

    const ecs_world_info_t *cur = ecs_get_world_info(world);
    test_assert(cur != NULL);

    ecs_world_info_t prev = *cur;

    ecs_entity_t c = ecs_new_id(world);
    ecs_entity_t e = ecs_new_id(world);
    test_delta(&prev, cur, tag_table_count, 0);

    ecs_add_id(world, e, c);
    test_delta(&prev, cur, tag_table_count, 1);

    ecs_fini(world);
}

void WorldInfo_trivial_table_count() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    /* Make it possible to delete components */
    ecs_remove_pair(world, ecs_id(Position), EcsOnDelete, EcsWildcard);
    ecs_remove_pair(world, ecs_id(Velocity), EcsOnDelete, EcsWildcard);

    ecs_set_hooks(world, Velocity, {
        .ctor = ecs_default_ctor
    });

    const ecs_world_info_t *cur = ecs_get_world_info(world);
    test_assert(cur != NULL);

    ecs_world_info_t prev = *cur;

    ecs_entity_t e1 = ecs_new_id(world);
    test_delta(&prev, cur, trivial_table_count, 0);

    ecs_add(world, e1, Position);
    test_delta(&prev, cur, trivial_table_count, 1);

    ecs_entity_t e2 = ecs_new_id(world);
    test_delta(&prev, cur, trivial_table_count, 0);

    ecs_add(world, e2, Velocity);
    test_delta(&prev, cur, trivial_table_count, 0);

    ecs_delete(world, ecs_id(Position));
    test_delta(&prev, cur, trivial_table_count, -1);

    ecs_delete(world, ecs_id(Velocity));
    test_delta(&prev, cur, trivial_table_count, 0);

    ecs_fini(world);
}

void WorldInfo_table_record_count() {
    ecs_world_t *world = ecs_mini();

    const ecs_world_info_t *cur = ecs_get_world_info(world);
    test_assert(cur != NULL);

    ecs_world_info_t prev = *cur;

    ecs_entity_t c_1 = ecs_new_id(world);
    ecs_entity_t c_2 = ecs_new_id(world);
    ecs_entity_t e = ecs_new_id(world);
    test_delta(&prev, cur, table_record_count, 0);

    ecs_add_id(world, e, c_1);
    test_delta(&prev, cur, table_record_count, 4);

    ecs_add_id(world, e, c_2);
    test_delta(&prev, cur, table_record_count, 5);

    ecs_delete(world, c_1);
    test_delta(&prev, cur, table_record_count, -5);

    ecs_delete(world, c_2);
    test_delta(&prev, cur, table_record_count, -4);

    ecs_fini(world);
}

void WorldInfo_table_storage_count() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    /* Make it possible to delete components */
    ecs_remove_pair(world, ecs_id(Position), EcsOnDelete, EcsWildcard);

    const ecs_world_info_t *cur = ecs_get_world_info(world);
    test_assert(cur != NULL);

    ecs_world_info_t prev = *cur;

    ecs_entity_t c_1 = ecs_new_id(world);
    ecs_entity_t e = ecs_new_id(world);
    ecs_add_id(world, e, c_1);
    test_delta(&prev, cur, table_storage_count, 0);

    ecs_add(world, e, Position);
    test_delta(&prev, cur, table_storage_count, 2);

    ecs_delete(world, c_1);
    test_delta(&prev, cur, table_storage_count, -1);

    ecs_delete(world, ecs_id(Position));
    test_delta(&prev, cur, table_storage_count, -1);

    ecs_fini(world);
}

void WorldInfo_table_create_count() {
    ecs_world_t *world = ecs_mini();

    const ecs_world_info_t *cur = ecs_get_world_info(world);
    test_assert(cur != NULL);

    ecs_world_info_t prev = *cur;

    ecs_entity_t c_1 = ecs_new_id(world);
    ecs_entity_t c_2 = ecs_new_id(world);
    ecs_entity_t e = ecs_new_id(world);
    test_delta(&prev, cur, table_create_total, 0);

    ecs_add_id(world, e, c_1);
    test_delta(&prev, cur, table_create_total, 1);

    ecs_add_id(world, e, c_2);
    test_delta(&prev, cur, table_create_total, 1);

    ecs_delete(world, c_2);
    ecs_delete(world, c_1);
    test_delta(&prev, cur, table_create_total, 0);

    ecs_fini(world);
}

void WorldInfo_table_delete_count() {
    ecs_world_t *world = ecs_mini();

    const ecs_world_info_t *cur = ecs_get_world_info(world);
    test_assert(cur != NULL);

    ecs_world_info_t prev = *cur;

    ecs_entity_t c_1 = ecs_new_id(world);
    ecs_entity_t c_2 = ecs_new_id(world);
    ecs_entity_t e = ecs_new_id(world);
    ecs_add_id(world, e, c_1);
    test_delta(&prev, cur, table_delete_total, 0);

    ecs_add_id(world, e, c_2);
    test_delta(&prev, cur, table_delete_total, 0);

    ecs_entity_t e2 = ecs_new_id(world);
    ecs_add_id(world, e2, c_2);
    test_delta(&prev, cur, table_delete_total, 0);

    ecs_delete(world, c_1);
    test_delta(&prev, cur, table_delete_total, 2);
    ecs_delete(world, c_2);
    test_delta(&prev, cur, table_delete_total, 1);

    ecs_fini(world);
}

void WorldInfo_id_count() {
    ecs_world_t *world = ecs_mini();

    const ecs_world_info_t *cur = ecs_get_world_info(world);
    test_assert(cur != NULL);

    ecs_world_info_t prev = *cur;

    ecs_entity_t c_1 = ecs_new_id(world);
    ecs_entity_t c_2 = ecs_new_id(world);
    ecs_entity_t e = ecs_new_id(world);
    test_delta(&prev, cur, id_count, 0);

    ecs_add_id(world, e, c_1);
    test_delta(&prev, cur, id_count, 1);

    ecs_add_id(world, e, c_2);
    test_delta(&prev, cur, id_count, 1);

    ecs_delete(world, c_1);
    test_delta(&prev, cur, id_count, -1);

    ecs_delete(world, c_2);
    test_delta(&prev, cur, id_count, -1);

    ecs_fini(world);
}

void WorldInfo_id_tag_component_count() {
    ecs_world_t *world = ecs_mini();

    const ecs_world_info_t *cur = ecs_get_world_info(world);
    test_assert(cur != NULL);

    ECS_COMPONENT(world, Position);

    /* Make it possible to delete components */
    ecs_remove_pair(world, ecs_id(Position), EcsOnDelete, EcsWildcard);

    ecs_world_info_t prev_1 = *cur;
    ecs_world_info_t prev_2 = *cur;

    ecs_entity_t c_1 = ecs_new_id(world);
    ecs_entity_t c_2 = ecs_new_id(world);
    ecs_entity_t e = ecs_new_id(world);
    test_delta(&prev_1, cur, tag_id_count, 0);
    test_delta(&prev_2, cur, component_id_count, 0);

    ecs_add_id(world, e, c_1);
    test_delta(&prev_1, cur, tag_id_count, 1);
    test_delta(&prev_2, cur, component_id_count, 0);

    ecs_add_id(world, e, c_2);
    test_delta(&prev_1, cur, tag_id_count, 1);
    test_delta(&prev_2, cur, component_id_count, 0);

    ecs_add(world, e, Position);
    test_delta(&prev_1, cur, tag_id_count, 0);
    test_delta(&prev_2, cur, component_id_count, 0);

    ecs_delete(world, c_1);
    test_delta(&prev_1, cur, tag_id_count, -1);
    test_delta(&prev_2, cur, component_id_count, 0);

    ecs_delete(world, c_2);
    test_delta(&prev_1, cur, tag_id_count, -1);
    test_delta(&prev_2, cur, component_id_count, 0);

    ecs_delete(world, ecs_id(Position));
    test_delta(&prev_1, cur, tag_id_count, 0);
    test_delta(&prev_2, cur, component_id_count, -1);

    ecs_fini(world);
}

void WorldInfo_id_pair_count() {
    ecs_world_t *world = ecs_mini();

    const ecs_world_info_t *cur = ecs_get_world_info(world);
    test_assert(cur != NULL);

    ecs_world_info_t prev = *cur;

    ecs_entity_t c_1 = ecs_new_id(world);
    ecs_entity_t c_2 = ecs_new_id(world);
    ecs_entity_t e = ecs_new_id(world);
    test_delta(&prev, cur, pair_id_count, 0);

    ecs_add_id(world, e, c_1);
    test_delta(&prev, cur, pair_id_count, 0);

    ecs_add_id(world, e, c_2);
    test_delta(&prev, cur, pair_id_count, 0);

    ecs_add_pair(world, e, c_1, c_2);
    test_delta(&prev, cur, pair_id_count, 1);

    ecs_delete(world, c_1);
    test_delta(&prev, cur, pair_id_count, -1);

    ecs_delete(world, c_2);
    test_delta(&prev, cur, pair_id_count, 0);

    ecs_fini(world);
}

void WorldInfo_id_wildcard_count() {
    ecs_world_t *world = ecs_mini();

    const ecs_world_info_t *cur = ecs_get_world_info(world);
    test_assert(cur != NULL);

    ecs_world_info_t prev = *cur;

    ecs_entity_t c_1 = ecs_new_id(world);
    ecs_entity_t c_2 = ecs_new_id(world);
    ecs_entity_t c_3 = ecs_new_id(world);
    ecs_entity_t e = ecs_new_id(world);
    test_delta(&prev, cur, wildcard_id_count, 0);

    ecs_add_id(world, e, c_1);
    test_delta(&prev, cur, wildcard_id_count, 0);

    ecs_add_id(world, e, c_2);
    test_delta(&prev, cur, wildcard_id_count, 0);

    ecs_add_pair(world, e, c_1, c_2);
    test_delta(&prev, cur, wildcard_id_count, 2);

    ecs_add_pair(world, e, c_3, c_2);
    test_delta(&prev, cur, wildcard_id_count, 1);

    ecs_delete(world, c_1);
    test_delta(&prev, cur, wildcard_id_count, -1);

    ecs_delete(world, c_3);
    test_delta(&prev, cur, wildcard_id_count, -1);

    ecs_delete(world, c_2);
    test_delta(&prev, cur, wildcard_id_count, -1);

    ecs_fini(world);
}
