#include <api.h>

static int sys_a_invoked;
static int sys_b_invoked;
static int sys_c_invoked;

void SysA(ecs_rows_t *rows) { 
    sys_a_invoked ++; 
}
void SysB(ecs_rows_t *rows) { 
    test_assert(sys_a_invoked != 0);
    sys_b_invoked ++; 
}
void SysC(ecs_rows_t *rows) { 
    test_assert(sys_b_invoked != 0);
    sys_c_invoked ++; 
}

void Pipeline_system_order_same_phase() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysA, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysC, EcsOnUpdate, Position);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    ecs_progress(world, 1);

    test_int(stats->systems_ran_frame, 3);
    test_int(stats->merge_count_total, 1);
    test_int(stats->pipeline_build_count_total, 1);

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);

    ecs_progress(world, 1);
    test_int(stats->pipeline_build_count_total, 1);

    ecs_fini(world);
}

void Pipeline_system_order_same_phase_after_disable() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysA, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysC, EcsOnUpdate, Position);

    ecs_enable(world, SysB, false);
    test_assert( ecs_has_entity(world, SysB, EcsDisabled));
    ecs_enable(world, SysB, true);
    test_assert( !ecs_has_entity(world, SysB, EcsDisabled));

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    ecs_progress(world, 1);

    test_int(stats->systems_ran_frame, 3);
    test_int(stats->merge_count_total, 1);
    test_int(stats->pipeline_build_count_total, 1);

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);

    ecs_progress(world, 1);
    test_int(stats->pipeline_build_count_total, 1);

    ecs_fini(world);
}

void Pipeline_system_order_different_phase() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysC, EcsPostUpdate, Position);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysA, EcsPreUpdate, Position);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    ecs_progress(world, 1);

    test_int(stats->systems_ran_frame, 3);
    test_int(stats->merge_count_total, 1);
    test_int(stats->pipeline_build_count_total, 1);

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);

    ecs_progress(world, 1);
    test_int(stats->pipeline_build_count_total, 1);

    ecs_fini(world);
}

void Pipeline_system_order_different_phase_after_disable() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysC, EcsPostUpdate, Position);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysA, EcsPreUpdate, Position);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    ecs_enable(world, SysB, false);
    test_assert( ecs_has_entity(world, SysB, EcsDisabled));
    ecs_enable(world, SysB, true);
    test_assert( !ecs_has_entity(world, SysB, EcsDisabled));

    ecs_progress(world, 1);

    test_int(stats->systems_ran_frame, 3);
    test_int(stats->merge_count_total, 1);
    test_int(stats->pipeline_build_count_total, 1);

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);

    ecs_progress(world, 1);
    test_int(stats->pipeline_build_count_total, 1);

    ecs_fini(world);
}

void Pipeline_system_order_same_phase_after_activate() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysA, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Velocity); /* System is deactivated */
    ECS_SYSTEM(world, SysC, EcsOnUpdate, Position);

    const ecs_world_info_t *stats = ecs_get_world_info(world);
    
    test_assert( ecs_has_entity(world, SysB, EcsInactive));
    ecs_add(world, E, Velocity);
    test_assert( !ecs_has_entity(world, SysB, EcsInactive));

    ecs_progress(world, 1);

    test_int(stats->systems_ran_frame, 3);
    test_int(stats->merge_count_total, 1);
    test_int(stats->pipeline_build_count_total, 1);

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);

    ecs_progress(world, 1);
    test_int(stats->pipeline_build_count_total, 1);

    ecs_fini(world);
}

void Pipeline_system_order_different_phase_after_activate() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysC, EcsPostUpdate, Position);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Velocity); /* System is deactivated */
    ECS_SYSTEM(world, SysA, EcsPreUpdate, Position);

    const ecs_world_info_t *stats = ecs_get_world_info(world);
    
    test_assert( ecs_has_entity(world, SysB, EcsInactive));
    ecs_add(world, E, Velocity);
    test_assert( !ecs_has_entity(world, SysB, EcsInactive));

    ecs_progress(world, 1);

    test_int(stats->systems_ran_frame, 3);
    test_int(stats->merge_count_total, 1);
    test_int(stats->pipeline_build_count_total, 1);

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);

    ecs_progress(world, 1);
    test_int(stats->pipeline_build_count_total, 1);

    ecs_fini(world);
}

void Pipeline_system_order_after_new_system_lower_id() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position);

    ECS_ENTITY(world, Sys, 0);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysC, EcsOnUpdate, Position);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    /* Create new system with SysA id */
    ecs_set(world, Sys, EcsSystem, {
        .action = SysA,
        .signature = "Position"
    });

    ecs_add_entity(world, Sys, EcsOnUpdate);
    ecs_add_entity(world, Sys, ECS_XOR | ecs_get_pipeline(world));

    ecs_progress(world, 1);

    test_int(stats->systems_ran_frame, 3);
    test_int(stats->merge_count_total, 1);
    test_int(stats->pipeline_build_count_total, 1);

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);

    ecs_progress(world, 1);
    test_int(stats->pipeline_build_count_total, 1);

    ecs_fini(world);
}

void Pipeline_system_order_after_new_system_inbetween_id() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysA, EcsOnUpdate, Position);
    ECS_ENTITY(world, Sys, 0);
    ECS_SYSTEM(world, SysC, EcsOnUpdate, Position);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    /* Create new system with SysA id */
    ecs_set(world, Sys, EcsSystem, {
        .action = SysB,
        .signature = "Position"
    });

    ecs_add_entity(world, Sys, EcsOnUpdate);
    ecs_add_entity(world, Sys, ECS_XOR | ecs_get_pipeline(world));

    ecs_progress(world, 1);

    test_int(stats->systems_ran_frame, 3);
    test_int(stats->merge_count_total, 1);
    test_int(stats->pipeline_build_count_total, 1);

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);

    ecs_progress(world, 1);
    test_int(stats->pipeline_build_count_total, 1);

    ecs_fini(world);
}

void Pipeline_system_order_after_new_system_higher_id() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysA, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Position);
    ECS_ENTITY(world, Sys, 0);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    /* Create new system with SysA id */
    ecs_set(world, Sys, EcsSystem, {
        .action = SysC,
        .signature = "Position"
    });

    ecs_add_entity(world, Sys, EcsOnUpdate);
    ecs_add_entity(world, Sys, ECS_XOR | ecs_get_pipeline(world));

    ecs_progress(world, 1);

    test_int(stats->systems_ran_frame, 3);
    test_int(stats->merge_count_total, 1);
    test_int(stats->pipeline_build_count_total, 1);

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);

    ecs_progress(world, 1);
    test_int(stats->pipeline_build_count_total, 1);

    ecs_fini(world);
}

static int sys_out_invoked;
static int sys_in_invoked;

static void SysOut(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Velocity, v, 2);

    sys_out_invoked ++;

    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_set(rows->world, rows->entities[i], Velocity, {10, 20});
    }
}

static void SysOutMain(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Velocity, v, 2);

    sys_out_invoked ++;

    int i;
    for (i = 0; i < rows->count; i ++) {
        v[i].x = 10;
        v[i].y = 20;
    }
}

static void SysIn(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Velocity, v, 1);

    test_assert(sys_out_invoked != 0);
    sys_in_invoked ++;
    
    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t e = rows->entities[i];
        test_assert( ecs_has(rows->world, e, Velocity));

        const Velocity *v_ptr = ecs_get_ptr(rows->world, e, Velocity);
        test_int(v_ptr->x, 10);
        test_int(v_ptr->y, 20);
    }
}

static void SysInMain(ecs_rows_t *rows) {
    ECS_COLUMN(rows, Velocity, v, 1);

    test_assert(sys_out_invoked != 0);
    sys_in_invoked ++;
    
    int i;
    for (i = 0; i < rows->count; i ++) {
        ecs_entity_t e = rows->entities[i];
        test_assert( ecs_has(rows->world, e, Velocity));

        test_int(v[i].x, 10);
        test_int(v[i].y, 20);
    }
}

void Pipeline_merge_after_staged_out() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysOut, EcsOnUpdate, Position, .Velocity);
    ECS_SYSTEM(world, SysInMain, EcsOnUpdate, Velocity);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    ecs_progress(world, 1);

    test_int(stats->systems_ran_frame, 2);
    test_int(stats->merge_count_total, 2);
    test_int(stats->pipeline_build_count_total, 1);

    test_int(sys_out_invoked, 1);
    test_int(sys_in_invoked, 1);

    ecs_progress(world, 1);
    test_int(stats->pipeline_build_count_total, 1);

    ecs_fini(world);
}

void Pipeline_merge_after_not_out() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysOut, EcsOnUpdate, Position, !Velocity);
    ECS_SYSTEM(world, SysInMain, EcsOnUpdate, Velocity);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    ecs_progress(world, 1);

    test_int(stats->systems_ran_frame, 2);
    test_int(stats->merge_count_total, 2);
    test_int(stats->pipeline_build_count_total, 1);

    test_int(sys_out_invoked, 1);
    test_int(sys_in_invoked, 1);

    ecs_progress(world, 1);
    test_int(stats->pipeline_build_count_total, 1);

    ecs_fini(world);
}

void Pipeline_no_merge_after_main_out() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position, Velocity);

    ECS_SYSTEM(world, SysOutMain, EcsOnUpdate, Position, Velocity);
    ECS_SYSTEM(world, SysInMain, EcsOnUpdate, Velocity);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    ecs_progress(world, 1);

    test_int(stats->systems_ran_frame, 2);
    test_int(stats->merge_count_total, 1);
    test_int(stats->pipeline_build_count_total, 1);

    test_int(sys_out_invoked, 1);
    test_int(sys_in_invoked, 1);

    ecs_progress(world, 1);
    test_int(stats->pipeline_build_count_total, 1);

    ecs_fini(world);
}

void Pipeline_no_merge_after_staged_in_out() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position, Velocity);

    ECS_SYSTEM(world, SysOut, EcsOnUpdate, Position, .Velocity);
    ECS_SYSTEM(world, SysIn, EcsOnUpdate, .Velocity);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    ecs_progress(world, 1);

    test_int(stats->systems_ran_frame, 2);
    test_int(stats->merge_count_total, 1);
    test_int(stats->pipeline_build_count_total, 1);

    test_int(sys_out_invoked, 1);
    test_int(sys_in_invoked, 1);

    ecs_progress(world, 1);
    test_int(stats->pipeline_build_count_total, 1);

    ecs_fini(world);
}
