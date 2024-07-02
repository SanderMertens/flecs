#include <addons.h>

static ECS_DECLARE(TagA);
static ECS_DECLARE(TagB);
ECS_COMPONENT_DECLARE(Position);
ECS_COMPONENT_DECLARE(Velocity);

static int sys_a_invoked;
static int sys_b_invoked;
static int sys_c_invoked;
static int sys_d_invoked;
static int sys_e_invoked;
static int sys_f_invoked;

static bool sys_a_real_world;
static bool sys_b_real_world;
static bool sys_c_real_world;
static bool sys_d_real_world;
static bool sys_e_real_world;
static bool sys_f_real_world;

static bool sys_a_world_readonly;
static bool sys_b_world_readonly;
static bool sys_c_world_readonly;
static bool sys_d_world_readonly;
static bool sys_e_world_readonly;
static bool sys_f_world_readonly;

static ecs_ftime_t sys_a_delta_time = 0;
static ecs_ftime_t sys_b_delta_time = 0;

void SysA(ecs_iter_t *it) { 
    ecs_os_ainc(&sys_a_invoked);
    sys_a_real_world = it->world == it->real_world;
    sys_a_world_readonly = ecs_stage_is_readonly(it->real_world);
    sys_a_delta_time = it->delta_time;
}
void SysB(ecs_iter_t *it) { 
    test_assert(sys_a_invoked != 0);
    ecs_os_ainc(&sys_b_invoked);
    sys_b_real_world = it->world == it->real_world;
    sys_b_world_readonly = ecs_stage_is_readonly(it->real_world);
    sys_b_delta_time = it->delta_time;
}
void SysC(ecs_iter_t *it) { 
    test_assert(sys_b_invoked != 0);
    ecs_os_ainc(&sys_c_invoked);
    sys_c_real_world = it->world == it->real_world;
    sys_c_world_readonly = ecs_stage_is_readonly(it->real_world);
}
void SysD(ecs_iter_t *it) { 
    test_assert(sys_c_invoked != 0);
    ecs_os_ainc(&sys_d_invoked);
    sys_d_real_world = it->world == it->real_world;
    sys_d_world_readonly = ecs_stage_is_readonly(it->real_world);
}
void SysE(ecs_iter_t *it) { 
    test_assert(sys_d_invoked != 0);
    ecs_os_ainc(&sys_e_invoked);
    sys_e_real_world = it->world == it->real_world;
    sys_e_world_readonly = ecs_stage_is_readonly(it->real_world);
}
void SysF(ecs_iter_t *it) { 
    test_assert(sys_d_invoked != 0);
    ecs_os_ainc(&sys_f_invoked);
    sys_f_real_world = it->world == it->real_world;
    sys_f_world_readonly = ecs_stage_is_readonly(it->real_world);
}

void Pipeline_system_order_same_phase(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysA, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysC, EcsOnUpdate, Position);

    const ecs_world_info_t *stats = ecs_get_world_info(world);
    
    test_int(stats->pipeline_build_count_total, 0);

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

void Pipeline_system_order_same_phase_after_disable(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysA, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysC, EcsOnUpdate, Position);

    ecs_enable(world, SysB, false);
    test_assert( ecs_has_id(world, SysB, EcsDisabled));
    ecs_enable(world, SysB, true);
    test_assert( !ecs_has_id(world, SysB, EcsDisabled));

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

void Pipeline_system_order_different_phase(void) {
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

void Pipeline_system_order_different_phase_after_disable(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysC, EcsPostUpdate, Position);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysA, EcsPreUpdate, Position);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    ecs_enable(world, SysB, false);
    test_assert( ecs_has_id(world, SysB, EcsDisabled));
    ecs_enable(world, SysB, true);
    test_assert( !ecs_has_id(world, SysB, EcsDisabled));

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

void Pipeline_system_order_same_phase_after_activate(void) {    
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysA, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Velocity); /* System is deactivated */
    ECS_SYSTEM(world, SysC, EcsOnUpdate, Position);

    const ecs_world_info_t *stats = ecs_get_world_info(world);
    
    test_assert( ecs_has_id(world, SysB, EcsEmpty));
    ecs_add(world, E, Velocity);

    ecs_progress(world, 1);

    test_int(stats->systems_ran_frame, 3);
    test_int(stats->merge_count_total, 1);
    test_int(stats->pipeline_build_count_total, 1);

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);

    test_assert( !ecs_has_id(world, SysB, EcsEmpty));

    ecs_progress(world, 1);
    test_int(stats->pipeline_build_count_total, 1);

    ecs_fini(world);
}

void Pipeline_system_order_different_phase_after_activate(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysC, EcsPostUpdate, Position);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Velocity); /* System is deactivated */
    ECS_SYSTEM(world, SysA, EcsPreUpdate, Position);

    const ecs_world_info_t *stats = ecs_get_world_info(world);
    
    test_assert( ecs_has_id(world, SysB, EcsEmpty));
    ecs_add(world, E, Velocity);

    ecs_progress(world, 1);

    test_assert( !ecs_has_id(world, SysB, EcsEmpty));

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

void Pipeline_system_order_after_new_system_lower_id(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position);

    ecs_entity_t Sys = ecs_new(world);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysC, EcsOnUpdate, Position);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    /* Create new system with Sys id */
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, {.id = Sys, .name = "SysA", .add = ecs_ids(ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysA
    });

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

void Pipeline_system_order_after_new_system_inbetween_id(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysA, EcsOnUpdate, Position);
    ecs_entity_t Sys = ecs_new(world);
    ECS_SYSTEM(world, SysC, EcsOnUpdate, Position);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    /* Create new system with Sys id */
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, {.id = Sys, .name = "SysB", .add = ecs_ids(ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysB
    });

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

void Pipeline_system_order_after_new_system_higher_id(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysA, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Position);
    ecs_entity_t Sys = ecs_new(world);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    /* Create new system with Sys id */
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, {.id = Sys, .name = "SysC", .add = ecs_ids(ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysC
    });

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

static void SysOut(ecs_iter_t *it) {
    ecs_id_t ecs_id(Velocity) = ecs_field_id(it, 1);

    sys_out_invoked ++;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], Velocity, {10, 20});
    }
}

static void SysOutMain(ecs_iter_t *it) {
    Velocity *v = ecs_field(it, Velocity, 1);

    sys_out_invoked ++;

    int i;
    for (i = 0; i < it->count; i ++) {
        v[i].x = 10;
        v[i].y = 20;
    }
}

static void SysIn(ecs_iter_t *it) {
    ecs_id_t ecs_id(Velocity) = ecs_field_id(it, 0);

    test_assert(sys_out_invoked != 0);
    sys_in_invoked ++;
    
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        test_assert( ecs_has(it->world, e, Velocity));

        const Velocity *v_ptr = ecs_get(it->world, e, Velocity);
        test_int(v_ptr->x, 10);
        test_int(v_ptr->y, 20);
    }
}

static void SysInMain(ecs_iter_t *it) {
    Velocity *v = ecs_field(it, Velocity, 0);
    ecs_id_t ecs_id(Velocity) = ecs_field_id(it, 0);

    test_assert(sys_out_invoked != 0);
    sys_in_invoked ++;
    
    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_entity_t e = it->entities[i];
        test_assert( ecs_has(it->world, e, Velocity));

        test_int(v[i].x, 10);
        test_int(v[i].y, 20);
    }
}

void Pipeline_merge_after_staged_out(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysOut, EcsOnUpdate, Position, [out] Velocity());
    ECS_SYSTEM(world, SysInMain, EcsOnUpdate, Velocity);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    ecs_progress(world, 1);

    test_int(stats->systems_ran_frame, 2);
    test_int(stats->merge_count_total, 2);
    test_int(stats->pipeline_build_count_total, 2);

    test_int(sys_out_invoked, 1);
    test_int(sys_in_invoked, 1);

    ecs_progress(world, 1);
    test_int(stats->pipeline_build_count_total, 2);

    ecs_fini(world);
}

void Pipeline_merge_after_not_out(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysOut, EcsOnUpdate, Position, [out] !Velocity);
    ECS_SYSTEM(world, SysInMain, EcsOnUpdate, Velocity);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    ecs_progress(world, 1);

    test_int(stats->systems_ran_frame, 2);
    test_int(stats->merge_count_total, 2);
    test_int(stats->pipeline_build_count_total, 2);

    test_int(sys_out_invoked, 1);
    test_int(sys_in_invoked, 1);

    ecs_progress(world, 1);
    test_int(stats->pipeline_build_count_total, 2);

    ecs_fini(world);
}

void Pipeline_no_merge_after_main_out(void) {
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

void Pipeline_merge_after_staged_in_out(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position, Velocity);

    /* Requires merge, because getting value in 2nd system cannot access data
     * written to stage from first system */
    ECS_SYSTEM(world, SysOut, EcsOnUpdate, Position, [out] Velocity());
    ECS_SYSTEM(world, SysIn, EcsOnUpdate, [in] Velocity());

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

void Pipeline_merge_after_staged_inout_main_implicit_inout(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysA, EcsOnUpdate, Position, Velocity());
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Velocity);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    ecs_progress(world, 1);

    test_int(stats->systems_ran_frame, 1);
    test_int(stats->merge_count_total, 1);
    test_int(stats->pipeline_build_count_total, 1);

    ecs_fini(world);
}

void Pipeline_merge_after_staged_inout_main_inout(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position, Velocity);

    ECS_SYSTEM(world, SysOut, EcsOnUpdate, Position, [inout] Velocity());
    ECS_SYSTEM(world, SysIn, EcsOnUpdate, Velocity);

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

void Pipeline_merge_after_staged_out_before_owned(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysOut, EcsOnUpdate, Position, [out] Velocity());
    ECS_SYSTEM(world, SysInMain, EcsOnUpdate, Velocity);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    ecs_progress(world, 1);

    test_int(stats->systems_ran_frame, 2);
    test_int(stats->merge_count_total, 2);
    test_int(stats->pipeline_build_count_total, 2);

    test_int(sys_out_invoked, 1);
    test_int(sys_in_invoked, 1);

    ecs_progress(world, 1);
    test_int(stats->pipeline_build_count_total, 2);

    ecs_fini(world);
}

void Pipeline_switch_pipeline(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysA, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysC, EcsOnUpdate, Position);

    ecs_add(world, ecs_id(SysC), Tag);

    ECS_PIPELINE(world, P1, flecs.system.System, flecs.pipeline.Phase(cascade DependsOn), !Tag);

    ecs_progress(world, 1);

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);

    ecs_set_pipeline(world, P1);

    ecs_progress(world, 1);

    test_int(sys_a_invoked, 2);
    test_int(sys_b_invoked, 2);
    test_int(sys_c_invoked, 1);

    ecs_fini(world);
}

void Pipeline_run_pipeline(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysA, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysC, EcsPostUpdate, Position);

    ecs_add(world, ecs_id(SysC), Tag);

    ECS_PIPELINE(world, P1, flecs.system.System, flecs.pipeline.Phase(cascade DependsOn), !Tag);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    ecs_progress(world, 0);

    test_int(stats->systems_ran_frame, 3);
    test_int(stats->merge_count_total, 1);
    test_int(stats->pipeline_build_count_total, 1);

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);

    ecs_run_pipeline(world, P1, 1);

    test_int(stats->pipeline_build_count_total, 2);

    test_int(sys_a_invoked, 2);
    test_int(sys_b_invoked, 2);
    test_int(sys_c_invoked, 1);

    ecs_fini(world);
}

void Pipeline_get_pipeline_from_stage(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t pipeline = ecs_get_pipeline(world);
    test_assert(pipeline != 0);

    /* Get default stage */
    ecs_world_t *stage = ecs_get_stage(world, 0);
    test_assert(stage != NULL);

    test_assert(pipeline == ecs_get_pipeline(stage));

    ecs_fini(world);
}

void Pipeline_3_systems_3_types(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_entity_t s1 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysA", .add = ecs_ids(ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysA
    });
    ecs_entity_t s2 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = NULL, .add = ecs_ids(ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysB
    });
    ecs_entity_t s3 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysC", .add = ecs_ids(ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position()",
        .callback = SysC
    });

    test_assert(s1 != 0);
    test_assert(s2 != 0);
    test_assert(s3 != 0);

    ecs_add_id(world, s3, Tag);

    ecs_new_w(world, Position);

    ecs_progress(world, 1);  

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);

    ecs_fini(world);
}

static
void RandomWrite(ecs_iter_t *it) {
    ecs_entity_t ecs_id(Position) = ecs_field_id(it, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], Position, {1, 2});
    }
}

static
void RandomRead(ecs_iter_t *it) {
    ecs_entity_t ecs_id(Position) = ecs_lookup(it->world, "Position");

    int i;
    for (i = 0; i < it->count; i ++) {
        const Position *p = ecs_get(it->world, it->entities[i], Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);
    }
}

static
void RandomReadWrite(ecs_iter_t *it) {
    ecs_entity_t ecs_id(Position) = ecs_lookup(it->world, "Position");

    int i;
    for (i = 0; i < it->count; i ++) {
        const Position *p = ecs_get(it->world, it->entities[i], Position);
        test_assert(p != NULL);
        test_int(p->x, 1);
        test_int(p->y, 2);

        ecs_set(it->world, it->entities[i], Position, {p->x + 1, p->y + 1});
    }
}

static
void RandomReadAfterRW(ecs_iter_t *it) {
    ecs_entity_t ecs_id(Position) = ecs_lookup(it->world, "Position");

    int i;
    for (i = 0; i < it->count; i ++) {
        const Position *p = ecs_get(it->world, it->entities[i], Position);
        test_assert(p != NULL);
        test_int(p->x, 2);
        test_int(p->y, 3);
    }
}

static
void RandomRead_Not(ecs_iter_t *it) {
    ecs_entity_t ecs_id(Position) = ecs_field_id(it, 1);

    int i;
    for (i = 0; i < it->count; i ++) {
        test_assert(!ecs_has(it->world, it->entities[i], Position));
        const Position *p = ecs_get(it->world, it->entities[i], Position);
        test_assert(p == NULL);
    }
}

void Pipeline_random_read_after_random_write_out_in(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ECS_SYSTEM(world, RandomWrite, EcsOnUpdate, Tag, [out] Position());
    ECS_SYSTEM(world, RandomRead, EcsOnUpdate, Tag, [in] Position());
    
    ecs_entity_t e = ecs_new_w(world, Tag);

    ecs_progress(world, 1);  

    test_assert(ecs_has(world, e, Position));
    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Pipeline_random_read_after_random_write_inout_in(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ECS_SYSTEM(world, RandomWrite, EcsOnUpdate, Tag, [inout] Position());
    ECS_SYSTEM(world, RandomRead, EcsOnUpdate, Tag, [in] Position());
    
    ecs_entity_t e = ecs_new_w(world, Tag);

    ecs_progress(world, 1);  

    test_assert(ecs_has(world, e, Position));
    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Pipeline_random_read_after_random_write_out_inout(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ECS_SYSTEM(world, RandomWrite, EcsOnUpdate, Tag, [out] Position());
    ECS_SYSTEM(world, RandomRead, EcsOnUpdate, Tag, [inout] Position());
    
    ecs_entity_t e = ecs_new_w(world, Tag);

    ecs_progress(world, 1);  

    test_assert(ecs_has(world, e, Position));
    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Pipeline_random_read_after_random_write_inout_inout(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ECS_SYSTEM(world, RandomWrite, EcsOnUpdate, Tag, [inout] Position());
    ECS_SYSTEM(world, RandomRead, EcsOnUpdate, Tag, [inout] Position());
    
    ecs_entity_t e = ecs_new_w(world, Tag);

    ecs_progress(world, 1);  

    test_assert(ecs_has(world, e, Position));
    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Pipeline_random_read_after_random_write_w_not_write(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ECS_SYSTEM(world, RandomWrite, EcsOnUpdate, Tag, [out] !Position);
    ECS_SYSTEM(world, RandomRead, EcsOnUpdate, Tag, [in] Position());
    
    ecs_entity_t e = ecs_new_w(world, Tag);

    ecs_progress(world, 1);  

    test_assert(ecs_has(world, e, Position));
    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Pipeline_random_read_after_random_write_w_not_read(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ECS_SYSTEM(world, RandomWrite, EcsOnUpdate, Tag, [out] Position());
    ECS_SYSTEM(world, RandomRead_Not, EcsOnUpdate, Tag, [in] !Position);
    
    ecs_entity_t e = ecs_new_w(world, Tag);

    ecs_progress(world, 1);

    test_assert(ecs_has(world, e, Position));
    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Pipeline_random_read_after_random_write_w_wildcard(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ECS_SYSTEM(world, RandomWrite, EcsOnUpdate, Tag, [out] Position());
    ECS_SYSTEM(world, RandomRead, EcsOnUpdate, Tag, [in] *());
    
    ecs_entity_t e = ecs_new_w(world, Tag);

    ecs_progress(world, 1);

    test_assert(ecs_has(world, e, Position));
    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Pipeline_random_in_after_random_inout_after_random_out(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ECS_SYSTEM(world, RandomWrite, EcsOnUpdate, Tag, [out] Position());
    ECS_SYSTEM(world, RandomReadWrite, EcsOnUpdate, Tag, [inout] Position());
    ECS_SYSTEM(world, RandomReadAfterRW, EcsOnUpdate, Tag, [in] Position());
    
    ecs_entity_t e = ecs_new_w(world, Tag);

    ecs_progress(world, 1);

    test_assert(ecs_has(world, e, Position));
    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 2);
    test_int(p->y, 3);

    ecs_fini(world);
}

static
void cb_first(ecs_iter_t *it) {
    int32_t *count = it->ctx;
    test_int(count[0], 0);
    count[0] ++;
}

static
void cb_second(ecs_iter_t *it) {
    int32_t *count = it->ctx;
    test_int(count[0], 1);
    count[0] ++;
}

static
void cb_third(ecs_iter_t *it) {
    int32_t *count = it->ctx;
    test_int(count[0], 2);
    count[0] ++;
}

void Pipeline_system_reverse_order_by_phase_custom_pipeline(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t PreFrame = ecs_new_w_id(world, EcsPhase);
    ecs_entity_t OnFrame = ecs_new_w_id(world, EcsPhase);
    ecs_entity_t PostFrame = ecs_new_w_id(world, EcsPhase);
    ecs_add_pair(world, OnFrame, EcsDependsOn, PreFrame);
    ecs_add_pair(world, PostFrame, EcsDependsOn, OnFrame);

    ECS_PIPELINE(world, P, flecs.system.System, flecs.pipeline.Phase(cascade DependsOn), Tag);

    int count = 0;

    ecs_system_init(world, &(ecs_system_desc_t){
        .callback = cb_third,
        .ctx = &count,
        .entity = ecs_entity(world, {.add = ecs_ids(Tag, ecs_pair(EcsDependsOn, PostFrame))})
    });

    ecs_system_init(world, &(ecs_system_desc_t){
        .callback = cb_second,
        .ctx = &count,
        .entity = ecs_entity(world, {.add = ecs_ids(Tag, ecs_pair(EcsDependsOn, OnFrame))})
    });

    ecs_system_init(world, &(ecs_system_desc_t){
        .callback = cb_first,
        .ctx = &count,
        .entity = ecs_entity(world, {.add = ecs_ids(Tag, ecs_pair(EcsDependsOn, PreFrame))})
    });

    test_int(count, 0);

    ecs_set_pipeline(world, P);

    ecs_progress(world, 0);

    test_int(count, 3);

    ecs_fini(world);
}

void Pipeline_stage_write_before_read(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ecs_entity_t s1 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysA", .add = ecs_ids(Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysA
    });
    ecs_entity_t s2 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysB", .add = ecs_ids(Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "[out] Position(), Position",
        .callback = SysB
    });
    ecs_entity_t s3 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysC", .add = ecs_ids(Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysC
    });

    ECS_PIPELINE(world, P, flecs.system.System, flecs.pipeline.Phase(cascade DependsOn), Tag);
    ecs_set_pipeline(world, P);

    test_assert(s1 != 0);
    test_assert(s2 != 0);
    test_assert(s3 != 0);

    ecs_new_w(world, Position);

    ecs_progress(world, 1);  

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);

    ecs_pipeline_stats_t stats = {0};
    test_bool(ecs_pipeline_stats_get(world, 
        ecs_get_pipeline(world), &stats), true);

    test_int(ecs_vec_count(&stats.systems), 5);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[0], s1);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[1], s2);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[2], 0); /* merge */
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[3], s3);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[4], 0); /* merge */

    ecs_pipeline_stats_fini(&stats);

    ecs_fini(world);
}

static
void Pipeline_mixed_multithreaded_internal(bool task_threads) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ecs_entity_t s1 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysA", .add = ecs_ids(Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysA,
        .multi_threaded = true
    });
    ecs_entity_t s2 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysB", .add = ecs_ids(Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysB
    });
    ecs_entity_t s3 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysC", .add = ecs_ids(Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysC
    });
    ecs_entity_t s4 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysD", .add = ecs_ids(Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysD,
        .multi_threaded = true
    });
    ecs_entity_t s5 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysE", .add = ecs_ids(Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysE,
        .multi_threaded = true
    });
    ecs_entity_t s6 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysF", .add = ecs_ids(Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysF
    });

    ECS_PIPELINE(world, P, flecs.system.System, flecs.pipeline.Phase(cascade DependsOn), Tag);
    ecs_set_pipeline(world, P);

    test_assert(s1 != 0);
    test_assert(s2 != 0);
    test_assert(s3 != 0);
    test_assert(s4 != 0);
    test_assert(s5 != 0);
    test_assert(s6 != 0);

    ecs_new_w(world, Position);
    ecs_new_w(world, Position);

    if (task_threads)
    {
        ecs_set_task_threads(world, 2);
    }
    else
    {
    	ecs_set_threads(world, 2);
    }

    ecs_progress(world, 1);

    test_int(sys_a_invoked, 2);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);
    test_int(sys_d_invoked, 2);
    test_int(sys_e_invoked, 2);
    test_int(sys_f_invoked, 1);

    ecs_pipeline_stats_t stats = {0};
    test_bool(ecs_pipeline_stats_get(world, 
        ecs_get_pipeline(world), &stats), true);

    test_int(ecs_vec_count(&stats.systems), 10);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[0], s1);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[1], 0); /* merge */
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[2], s2);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[3], s3);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[4], 0); /* merge */
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[5], s4);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[6], s5);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[7], 0); /* merge */
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[8], s6);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[9], 0); /* merge */

    ecs_pipeline_stats_fini(&stats);

    ecs_fini(world);
}

void Pipeline_mixed_multithreaded(void) {
    Pipeline_mixed_multithreaded_internal(false);
}

void Pipeline_mixed_multithreaded_tasks(void) {
    Pipeline_mixed_multithreaded_internal(true);
}

void Pipeline_mixed_staging(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ecs_entity_t s1 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysA", .add = ecs_ids(Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysA,
        .immediate = true
    });
    ecs_entity_t s2 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysB", .add = ecs_ids(Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysB
    });
    ecs_entity_t s3 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysC", .add = ecs_ids(Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysC
    });
    ecs_entity_t s4 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysD", .add = ecs_ids(Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysD,
        .immediate = true
    });
    ecs_entity_t s5 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysE", .add = ecs_ids(Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysE,
        .immediate = true
    });
    ecs_entity_t s6 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysF", .add = ecs_ids(Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysF
    });

    ECS_PIPELINE(world, P, flecs.system.System, flecs.pipeline.Phase(cascade DependsOn), Tag);
    ecs_set_pipeline(world, P);

    test_assert(s1 != 0);
    test_assert(s2 != 0);
    test_assert(s3 != 0);
    test_assert(s4 != 0);
    test_assert(s5 != 0);
    test_assert(s6 != 0);

    ecs_new_w(world, Position);
    ecs_new_w(world, Position);

    ecs_progress(world, 1);
    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);
    test_int(sys_d_invoked, 1);
    test_int(sys_e_invoked, 1);
    test_int(sys_f_invoked, 1);
    test_int(sys_a_real_world, true);
    test_int(sys_b_real_world, false);
    test_int(sys_c_real_world, false);
    test_int(sys_d_real_world, true);
    test_int(sys_e_real_world, true);
    test_int(sys_f_real_world, false);
    test_int(sys_a_world_readonly, false);
    test_int(sys_b_world_readonly, true);
    test_int(sys_c_world_readonly, true);
    test_int(sys_d_world_readonly, false);
    test_int(sys_e_world_readonly, false);
    test_int(sys_f_world_readonly, true);

    ecs_progress(world, 1);
    test_int(sys_a_invoked, 2);
    test_int(sys_b_invoked, 2);
    test_int(sys_c_invoked, 2);
    test_int(sys_d_invoked, 2);
    test_int(sys_e_invoked, 2);
    test_int(sys_f_invoked, 2);
    test_int(sys_a_real_world, true);
    test_int(sys_b_real_world, false);
    test_int(sys_c_real_world, false);
    test_int(sys_d_real_world, true);
    test_int(sys_e_real_world, true);
    test_int(sys_f_real_world, false);
    test_int(sys_a_world_readonly, false);
    test_int(sys_b_world_readonly, true);
    test_int(sys_c_world_readonly, true);
    test_int(sys_d_world_readonly, false);
    test_int(sys_e_world_readonly, false);
    test_int(sys_f_world_readonly, true);

    ecs_pipeline_stats_t stats = {0};
    test_bool(ecs_pipeline_stats_get(world, 
        ecs_get_pipeline(world), &stats), true);

    test_int(ecs_vec_count(&stats.systems), 11);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[0], s1);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[1], 0); /* merge */
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[2], s2);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[3], s3);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[4], 0); /* merge */
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[5], s4);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[6], 0); /* merge */
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[7], s5);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[8], 0); /* merge */
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[9], s6);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[10], 0); /* merge */

    ecs_pipeline_stats_fini(&stats);

    ecs_fini(world);
}

static
void WritePosition(ecs_iter_t *it) {
    if (*(bool*)it->ctx) {
        ecs_entity_t ecs_id(Position) = ecs_field_id(it, 1);
        for (int i = 0; i < it->count; i ++) {
            ecs_add(it->world, it->entities[i], Position);
        }
    }
}

void Pipeline_single_threaded_pipeline_change(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    bool write_position = false;

    ecs_entity_t s1 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysA", .add = ecs_ids(ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Tag, !Position",
        .callback = SysA
    });

    ecs_entity_t s2 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysB", .add = ecs_ids(ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Tag, Position",
        .callback = SysB
    });

    ecs_entity_t s3 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "WritePosition", .add = ecs_ids(ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Tag, [out] Position()",
        .callback = WritePosition,
        .ctx = &write_position
    });

    ecs_entity_t s4 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysC", .add = ecs_ids(ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Tag, Position",
        .callback = SysC
    });

    ecs_entity_t s5 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysD", .add = ecs_ids(ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Tag, !Position",
        .callback = SysD
    });

    /* Initialize to 1 so asserts inside system won't trigger */
    sys_a_invoked = 1;
    sys_b_invoked = 1;
    sys_c_invoked = 1;
    sys_d_invoked = 1;

    ecs_new_w(world, Tag);
    ecs_new_w(world, Tag);

    test_assert(s1 != 0);
    test_assert(s2 != 0);
    test_assert(s3 != 0);
    test_assert(s4 != 0);
    test_assert(s5 != 0);

    ecs_progress(world, 0);
    ecs_progress(world, 0);
    ecs_progress(world, 0);

    test_int(sys_a_invoked, 1 + 3);
    test_int(sys_b_invoked, 1 + 0);
    test_int(sys_c_invoked, 1 + 0);
    test_int(sys_d_invoked, 1 + 3);

    write_position = true;

    ecs_progress(world, 0);

    test_int(sys_a_invoked, 1 + 4);
    test_int(sys_b_invoked, 1 + 0);
    test_int(sys_c_invoked, 1 + 1);
    test_int(sys_d_invoked, 1 + 3);

    ecs_progress(world, 0);

    test_int(sys_a_invoked, 1 + 4);
    test_int(sys_b_invoked, 1 + 1);
    test_int(sys_c_invoked, 1 + 2);
    test_int(sys_d_invoked, 1 + 3);

    ecs_fini(world);
}

static
void Pipeline_multi_threaded_pipeline_change_internal(bool task_threads) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    bool write_position = false;

    ecs_entity_t s1 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysA", .add = ecs_ids(ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Tag, !Position",
        .callback = SysA,
        .multi_threaded = true
    });

    ecs_entity_t s2 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysB", .add = ecs_ids(ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Tag, Position, [out] Position()",
        .callback = SysB,
        .multi_threaded = true
    });

    ecs_entity_t s3 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "WritePosition", .add = ecs_ids(ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Tag, [out] Position(), [in] ?Position",
        .callback = WritePosition,
        .ctx = &write_position,
        .multi_threaded = true
    });

    ecs_entity_t s4 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysC", .add = ecs_ids(ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Tag, Position",
        .callback = SysC,
        .multi_threaded = true
    });

    ecs_entity_t s5 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysD", .add = ecs_ids(ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Tag, !Position",
        .callback = SysD,
        .multi_threaded = true
    });

    if (task_threads) {
        ecs_set_task_threads(world, 2);
    } else {
        ecs_set_threads(world, 2);
    }

    /* Initialize to 1 so asserts inside system won't trigger */
    sys_a_invoked = 1;
    sys_b_invoked = 1;
    sys_c_invoked = 1;
    sys_d_invoked = 1;

    ecs_new_w(world, Tag);
    ecs_new_w(world, Tag);

    test_assert(s1 != 0);
    test_assert(s2 != 0);
    test_assert(s3 != 0);
    test_assert(s4 != 0);
    test_assert(s5 != 0);

    ecs_progress(world, 0);
    ecs_progress(world, 0);
    ecs_progress(world, 0);

    test_int(sys_a_invoked, 1 + 3 * 2);
    test_int(sys_b_invoked, 1 + 0 * 2);
    test_int(sys_c_invoked, 1 + 0 * 2);
    test_int(sys_d_invoked, 1 + 3 * 2);

    write_position = true;

    ecs_progress(world, 0);

    test_int(sys_a_invoked, 1 + 4 * 2);
    test_int(sys_b_invoked, 1 + 0 * 2);
    test_int(sys_c_invoked, 1 + 1 * 2);
    test_int(sys_d_invoked, 1 + 3 * 2);

    ecs_progress(world, 0);

    test_int(sys_a_invoked, 1 + 4 * 2);
    test_int(sys_b_invoked, 1 + 1 * 2);
    test_int(sys_c_invoked, 1 + 2 * 2);
    test_int(sys_d_invoked, 1 + 3 * 2);

    ecs_fini(world);
}

void Pipeline_multi_threaded_pipeline_change(void) {
    Pipeline_multi_threaded_pipeline_change_internal(false);
}

void Pipeline_multi_threaded_pipeline_change_tasks(void) {
    Pipeline_multi_threaded_pipeline_change_internal(true);
}

void Pipeline_activate_after_add(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    bool write_position = false;

    ecs_entity_t s1 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "WritePosition", .add = ecs_ids(ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Tag, [out] Position()",
        .callback = WritePosition,
        .ctx = &write_position
    });

    ecs_entity_t s2 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysA", .add = ecs_ids(ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysA
    });

    ecs_new_w(world, Tag);

    test_assert(s1 != 0);
    test_assert(s2 != 0);

    ecs_progress(world, 0);
    ecs_progress(world, 0);
    ecs_progress(world, 0);

    test_int(sys_a_invoked, 0);

    write_position = true;

    ecs_progress(world, 0);

    test_int(sys_a_invoked, 1);

    ecs_fini(world);
}

static ecs_query_t *q_result;

static
void CreateQuery(ecs_iter_t *it) {
    test_assert(it->real_world == it->world);
    q_result = ecs_query(it->world, { .expr = "Position" });
    ecs_query_fini(q_result);
}

void Pipeline_no_staging_system_create_query(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ecs_entity_t s = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "CreateQuery", .add = ecs_ids(Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .callback = CreateQuery,
        .immediate = true
    });

    ECS_PIPELINE(world, P, flecs.system.System, flecs.pipeline.Phase(cascade DependsOn), Tag);
    ecs_set_pipeline(world, P);

    test_assert(s != 0);

    ecs_progress(world, 1.0);

    test_assert(q_result != NULL);

    ecs_pipeline_stats_t stats = {0};
    test_bool(ecs_pipeline_stats_get(world, 
        ecs_get_pipeline(world), &stats), true);

    test_int(ecs_vec_count(&stats.systems), 2);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[0], s);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[1], 0); /* merge */

    ecs_pipeline_stats_fini(&stats);

    ecs_fini(world);
}

static int set_singleton_invoked = 0;
static int match_singleton_invoked = 0;
static int match_all_invoked = 0;

static void set_singleton(ecs_iter_t *it) {
    ecs_singleton_add(it->world, TagB);
    set_singleton_invoked ++;
}

static void match_singleton(ecs_iter_t *it) {
    match_singleton_invoked ++;
}

static void match_all(ecs_iter_t *it) {
    match_all_invoked ++;
}

void Pipeline_match_all_after_pipeline_rebuild(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG_DEFINE(world, TagA);
    ECS_TAG_DEFINE(world, TagB);

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .add = ecs_ids(ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "[out] TagB()",
        .callback = set_singleton
    });

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .add = ecs_ids(ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "TagB",
        .callback = match_singleton
    });

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .add = ecs_ids(ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "?TagA",
        .callback = match_all
    });

    ecs_progress(world, 0);

    test_int(set_singleton_invoked, 1);
    test_int(match_singleton_invoked, 1);
    test_assert(match_all_invoked >= 1);

    ecs_fini(world);
}

void Pipeline_empty_pipeline(void) {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsPipeline);

    const ecs_world_info_t *info = ecs_get_world_info(world);
    test_int(info->frame_count_total, 0);

    ecs_progress(world, 0);

    test_int(info->frame_count_total, 1);

    ecs_fini(world);
}

void Pipeline_custom_pipeline_w_system_macro(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, E, Position);

    ECS_PIPELINE(world, P, flecs.system.System, Tag);

    ECS_SYSTEM(world, SysA, Tag, Position);
    ECS_SYSTEM(world, SysB, Tag, Position);
    ECS_SYSTEM(world, SysC, Tag, Position);

    ecs_set_pipeline(world, P);
    ecs_progress(world, 0);

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);

    ecs_fini(world);
}

void Pipeline_pipeline_w_short_notation(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t p = ecs_pipeline(world, {
        .query.expr = "flecs.system.System, Tag"
    });

    test_assert(p != 0);

    ecs_fini(world);
}

void Pipeline_stack_allocator_after_progress(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysA, EcsOnUpdate, Position);

    ecs_query_t *f = ecs_query(world, {
        .terms = {{ ecs_id(Position) }}
    });

    ecs_iter_t it = ecs_query_iter(world, f);
    ecs_stack_cursor_t cursor = *it.priv_.cache.stack_cursor;
    ecs_iter_fini(&it);

    ecs_progress(world, 1);
    test_int(sys_a_invoked, 1);

    it = ecs_query_iter(world, f);
    test_assert(it.priv_.cache.stack_cursor->page == cursor.page);
    test_assert(it.priv_.cache.stack_cursor->sp == cursor.sp);
    ecs_iter_fini(&it);

    ecs_query_fini(f);

    ecs_fini(world);
}

void Pipeline_stack_allocator_after_progress_w_pipeline_change(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysA, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Position);

    ecs_query_t *f = ecs_query(world, {
        .terms = {{ ecs_id(Position) }}
    });

    ecs_iter_t it = ecs_query_iter(world, f);
    ecs_stack_cursor_t cursor = *it.priv_.cache.stack_cursor;
    ecs_iter_fini(&it);

    ecs_progress(world, 1);
    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);

    it = ecs_query_iter(world, f);
    test_assert(it.priv_.cache.stack_cursor->page == cursor.page);
    test_assert(it.priv_.cache.stack_cursor->sp == cursor.sp);
    ecs_iter_fini(&it);

    ecs_enable(world, SysB, false);

    ecs_progress(world, 1);
    test_int(sys_a_invoked, 2);
    test_int(sys_b_invoked, 1);

    it = ecs_query_iter(world, f);
    test_assert(it.priv_.cache.stack_cursor->page == cursor.page);
    test_assert(it.priv_.cache.stack_cursor->sp == cursor.sp);
    ecs_iter_fini(&it);

    ecs_query_fini(f);

    ecs_fini(world);
}

static
void Sys_w_MainWorldIter(ecs_iter_t *it) {
    ecs_id_t ecs_id(Position) = ecs_field_id(it, 0);
    ecs_query_t *f = ecs_query(it->real_world, {
        .terms = {{ ecs_id(Position) }}
    });

    ecs_iter_t fit = ecs_query_iter(it->real_world, f);
    test_bool(true, ecs_query_next(&fit));
    test_int(1, fit.count);
    test_bool(false, ecs_query_next(&fit));
    ecs_query_fini(f);
}

static
void Pipeline_iter_from_world_in_singlethread_system_multitead_app_internal(bool task_threads) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_new_w(world, Position);

    ECS_SYSTEM(world, Sys_w_MainWorldIter, EcsOnUpdate, Position());

    if (task_threads)
    {
        ecs_set_task_threads(world, 2);
    }
    else
    {
        ecs_set_threads(world, 2);
    }

    ecs_progress(world, 0);

    ecs_fini(world);
}

void Pipeline_iter_from_world_in_singlethread_system_multitead_app(void) {
    Pipeline_iter_from_world_in_singlethread_system_multitead_app_internal(false);
}

void Pipeline_iter_from_world_in_singlethread_system_multitead_app_tasks(void) {
    Pipeline_iter_from_world_in_singlethread_system_multitead_app_internal(true);
}

static int staging_system_invoked = 0;
static void StagingSystem(ecs_iter_t *it) {
    test_assert( ecs_stage_is_readonly(it->real_world));
    staging_system_invoked ++;
}

static int no_staging_system_invoked = 0;
static void NoStagingSystem(ecs_iter_t *it) {
    test_assert( !ecs_stage_is_readonly(it->real_world));
    no_staging_system_invoked ++;
}

void Pipeline_no_staging_after_inactive_system(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate ))
        }),
        .query.terms = {{ Tag, .oper = EcsNot }},
        .callback = StagingSystem
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate ))
        }),
        .query.terms = {{ ecs_id(Position) }, { Tag, .oper = EcsNot }},
        .callback = StagingSystem
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate ))
        }),
        .callback = NoStagingSystem,
        .immediate = true
    });

    ecs_progress(world, 0);
    test_assert(staging_system_invoked != 0);
    test_int(no_staging_system_invoked, 1);

    staging_system_invoked = 0;
    no_staging_system_invoked = 0;

    ecs_progress(world, 0);
    test_assert(staging_system_invoked != 0);
    test_int(no_staging_system_invoked, 1);

    ecs_fini(world);
}

static ecs_entity_t create_position_e = 0;
static ecs_entity_t create_velocity_e = 0;

static int no_staging_create_position_invoked = 0;
static int no_staging_create_velocity_invoked = 0;

static void NoStagingSystemCreatePosition(ecs_iter_t *it) {
    ecs_defer_end(it->world);
    
    create_position_e = ecs_new(it->world);
    ecs_set(it->world, create_position_e, Position, {0, 0});
    
    ecs_query_t *f = ecs_query(it->world, {
        .terms = {{ ecs_id(Position) }}
    });

    ecs_iter_t fit = ecs_query_iter(it->world, f);
    test_bool(true, ecs_query_next(&fit));
    test_int(fit.count, 1);
    test_uint(fit.entities[0], create_position_e);
    test_bool(false, ecs_query_next(&fit));

    ecs_query_fini(f);

    ecs_defer_begin(it->world);
    no_staging_create_position_invoked ++;
}

static void NoStagingSystemCreateVelocity(ecs_iter_t *it) {
    ecs_defer_end(it->world);
    
    create_velocity_e = ecs_new(it->world);
    ecs_set(it->world, create_velocity_e, Velocity, {0, 0});
    
    ecs_query_t *f = ecs_query(it->world, {
        .terms = {{ ecs_id(Velocity) }}
    });

    ecs_iter_t fit = ecs_query_iter(it->world, f);
    test_bool(true, ecs_query_next(&fit));
    test_int(fit.count, 1);
    test_uint(fit.entities[0], create_velocity_e);
    test_bool(false, ecs_query_next(&fit));

    ecs_query_fini(f);

    ecs_defer_begin(it->world);
    no_staging_create_velocity_invoked ++;
}

static int32_t read_position_invoked = 0;
static void ReadPosition(ecs_iter_t *it) {
    ecs_os_ainc(&read_position_invoked);
}

static int32_t read_velocity_invoked = 0;
static void ReadVelocity(ecs_iter_t *it) {
    ecs_os_ainc(&read_velocity_invoked);
}

void Pipeline_inactive_system_after_no_staging_system_no_defer_w_filter(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position);

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate ))
        }),
        .callback = NoStagingSystemCreatePosition,
        .immediate = true
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate ))
        }),
        .query.terms = {{ ecs_id(Position) }},
        .callback = ReadPosition
    });

    ecs_progress(world, 0);
    test_int(no_staging_create_position_invoked, 1);
    test_int(read_position_invoked, 1);

    test_assert(create_position_e != 0);
    test_assert(ecs_has(world, create_position_e, Position));
    ecs_delete(world, create_position_e);
    
    ecs_progress(world, 0);
    test_int(no_staging_create_position_invoked, 2);
    test_int(read_position_invoked, 2);

    ecs_fini(world);
}

void Pipeline_inactive_system_after_no_staging_system_no_defer_w_filter_w_no_staging_at_end(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position);

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate ))
        }),
        .callback = NoStagingSystemCreatePosition,
        .immediate = true
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate ))
        }),
        .query.terms = {{ ecs_id(Position) }},
        .callback = ReadPosition
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate ))
        }),
        .callback = SysA,
        .immediate = true
    });

    ecs_progress(world, 0);
    test_int(no_staging_create_position_invoked, 1);
    test_int(read_position_invoked, 1);

    test_assert(create_position_e != 0);
    test_assert(ecs_has(world, create_position_e, Position));
    ecs_delete(world, create_position_e);
    
    ecs_progress(world, 0);
    test_int(no_staging_create_position_invoked, 2);
    test_int(read_position_invoked, 2);

    ecs_fini(world);
}

void Pipeline_inactive_system_after_2_no_staging_system_no_defer_w_filter(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate ))
        }),
        .callback = NoStagingSystemCreatePosition,
        .immediate = true
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate ))
        }),
        .callback = NoStagingSystemCreateVelocity,
        .immediate = true
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate ))
        }),
        .query.terms = {{ ecs_id(Position) }},
        .callback = ReadPosition
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate ))
        }),
        .query.terms = {{ ecs_id(Velocity) }},
        .callback = ReadVelocity
    });

    ecs_progress(world, 0);
    test_int(no_staging_create_position_invoked, 1);
    test_int(no_staging_create_velocity_invoked, 1);
    test_int(read_position_invoked, 1);
    test_int(read_velocity_invoked, 1);

    test_assert(create_position_e != 0);
    test_assert(ecs_has(world, create_position_e, Position));
    ecs_delete(world, create_position_e);
    
    test_assert(create_velocity_e != 0);
    test_assert(ecs_has(world, create_velocity_e, Velocity));
    ecs_delete(world, create_velocity_e);

    ecs_progress(world, 0);
    test_int(no_staging_create_position_invoked, 2);
    test_int(no_staging_create_velocity_invoked, 2);
    test_int(read_position_invoked, 2);
    test_int(read_velocity_invoked, 2);

    ecs_fini(world);
}

static
void Pipeline_inactive_multithread_system_after_no_staging_system_no_defer_internal(bool task_threads) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate ))
        }),
        .callback = NoStagingSystemCreatePosition,
        .immediate = true
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate ))
        }),
        .query.terms = {{ ecs_id(Position) }},
        .callback = ReadPosition,
        .multi_threaded = true
    });

    if (task_threads)
    {
        ecs_set_task_threads(world, 2);
    }
    else
    {
        ecs_set_threads(world, 2);
    }

    ecs_progress(world, 0);
    test_int(no_staging_create_position_invoked, 1);
    test_int(read_position_invoked, 1);

    test_assert(create_position_e != 0);
    test_assert(ecs_has(world, create_position_e, Position));
    ecs_delete(world, create_position_e);

    ecs_progress(world, 0);
    test_int(no_staging_create_position_invoked, 2);
    test_int(read_position_invoked, 2);

    ecs_fini(world);
}

void Pipeline_inactive_multithread_system_after_no_staging_system_no_defer(void) {
    Pipeline_inactive_multithread_system_after_no_staging_system_no_defer_internal(false);
}

void Pipeline_inactive_multithread_tasks_system_after_no_staging_system_no_defer(void) {
    Pipeline_inactive_multithread_system_after_no_staging_system_no_defer_internal(true);
}

static
void Pipeline_inactive_multithread_system_after_no_staging_system_no_defer_w_no_staging_at_end_internal(bool task_threads) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position);
    ECS_COMPONENT_DEFINE(world, Velocity);

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate ))
        }),
        .callback = NoStagingSystemCreatePosition,
        .immediate = true
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate ))
        }),
        .query.terms = {{ ecs_id(Position) }},
        .callback = ReadPosition,
        .multi_threaded = true
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate ))
        }),
        .callback = ReadPosition,
        .immediate = true
    });

    if (task_threads)
    {
        ecs_set_task_threads(world, 2);
    }
    else
    {
        ecs_set_threads(world, 2);
    }

    ecs_progress(world, 0);
    test_int(no_staging_create_position_invoked, 1);
    test_int(read_position_invoked, 2);

    test_assert(create_position_e != 0);
    test_assert(ecs_has(world, create_position_e, Position));
    ecs_delete(world, create_position_e);

    ecs_progress(world, 0);
    test_int(no_staging_create_position_invoked, 2);
    test_int(read_position_invoked, 4);

    ecs_fini(world);
}

void Pipeline_inactive_multithread_system_after_no_staging_system_no_defer_w_no_staging_at_end(void) {
    Pipeline_inactive_multithread_system_after_no_staging_system_no_defer_w_no_staging_at_end_internal(false);
}

void Pipeline_inactive_multithread_tasks_system_after_no_staging_system_no_defer_w_no_staging_at_end(void) {
    Pipeline_inactive_multithread_system_after_no_staging_system_no_defer_w_no_staging_at_end_internal(true);
}

static int add_id_invoked = 0;
static int foo_system_invoked = 0;

static void AddId(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    ecs_id_t id = ecs_field_id(it, 0);

    int i;
    for (i = 0; i < it->count; i++) {
        ecs_add_id(world, it->entities[i], id);
    }

    add_id_invoked ++;
}

static void FooSystem(ecs_iter_t *it) {
    foo_system_invoked ++;
}

static
void Pipeline_multi_threaded_pipeline_change_w_only_singlethreaded_internal(bool task_threads) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, FooSystem,   EcsOnUpdate, Position);
    ECS_SYSTEM(world, AddId,       EcsOnUpdate, !Position, Tag);

    if (task_threads)
    {
        ecs_set_task_threads(world, 2);
    }
    else
    {
        ecs_set_threads(world, 2);
    }

    ecs_entity_t e = ecs_new_w(world, Tag);

    ecs_progress(world, 0);

    test_assert(ecs_has(world, e, Position));
    test_int(foo_system_invoked, 0);
    test_int(add_id_invoked, 1);

    ecs_fini(world);
}

void Pipeline_multi_threaded_pipeline_change_w_only_singlethreaded(void) {
    Pipeline_multi_threaded_pipeline_change_w_only_singlethreaded_internal(false);
}

void Pipeline_multi_threaded_tasks_pipeline_change_w_only_singlethreaded(void) {
    Pipeline_multi_threaded_pipeline_change_w_only_singlethreaded_internal(true);
}

static void SetPosition(ecs_iter_t *it) {
    Position *p = ecs_field(it, Position, 0);

    for (int i = 0; i < it->count; i ++) {
        p[i].x = 10;
        p[i].y = 20;
    }
}

void Pipeline_sync_after_not_out_for_out(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, AddId,       EcsOnUpdate, [out] !Position, Tag);
    ECS_SYSTEM(world, SetPosition, EcsOnUpdate, [out] Position);

    ecs_entity_t e = ecs_new_w(world, Tag);

    const ecs_world_info_t *wi = ecs_get_world_info(world);

    ecs_progress(world, 0);

    test_int(wi->merge_count_total, 2);

    const Position *p = ecs_get(world, e, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Pipeline_pair_wildcard_read_after_staged_write(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Rel);
    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, AddId, EcsOnUpdate, [out] !(Rel, Position), Tag);
    ECS_SYSTEM(world, SysA,  EcsOnUpdate, [in]  (Rel, *));

    ecs_new_w(world, Tag);

    const ecs_world_info_t *wi = ecs_get_world_info(world);

    ecs_progress(world, 0);

    test_int(wi->merge_count_total, 2);

    test_int(add_id_invoked, 1);
    test_int(sys_a_invoked, 1);

    ecs_fini(world);
}

static int add_pair_invoked = 0;

static void AddPair(ecs_iter_t *it) {
    ecs_world_t *world = it->world;
    ecs_id_t id = ecs_field_id(it, 0);

    int i;
    for (i = 0; i < it->count; i++) {
        ecs_add_pair(world, it->entities[i], 
            ECS_PAIR_FIRST(id), ecs_id(Position));
    }

    add_pair_invoked ++;
}

void Pipeline_pair_read_after_staged_wildcard_write(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Rel);
    ECS_COMPONENT_DEFINE(world, Position);

    ECS_SYSTEM(world, AddPair, EcsOnUpdate, [out] !(Rel, *), Tag);
    ECS_SYSTEM(world, SysA,    EcsOnUpdate, [in]  (Rel, Position));

    ecs_new_w(world, Tag);

    const ecs_world_info_t *wi = ecs_get_world_info(world);

    ecs_progress(world, 0);

    test_int(wi->merge_count_total, 2);

    test_int(add_pair_invoked, 1);
    test_int(sys_a_invoked, 1);

    ecs_fini(world);
}

void Pipeline_no_sync_after_pair_wildcard_read_after_unmatching_staged_write(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Rel);
    ECS_TAG(world, Rel2);
    ECS_COMPONENT_DEFINE(world, Position);

    ECS_SYSTEM(world, AddPair, EcsOnUpdate, [out] !(Rel2, Position), Tag);
    ECS_SYSTEM(world, SysA,    EcsOnUpdate, [in] (Rel, *));

    ecs_new_w(world, Tag);

    const ecs_world_info_t *wi = ecs_get_world_info(world);

    ecs_progress(world, 0);

    test_int(wi->merge_count_total, 1);

    test_int(add_pair_invoked, 1);
    test_int(sys_a_invoked, 0);

    test_int(1, ecs_count_id(world, ecs_pair(Rel2, ecs_id(Position))));

    ecs_fini(world);
}

void Pipeline_no_merge_after_from_nothing_w_default_inout(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, SysA, EcsOnUpdate, Tag, Position());
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Tag, Position);

    ecs_new_w(world, Tag);
    
    const ecs_world_info_t *wi = ecs_get_world_info(world);

    ecs_progress(world, 0);

    test_int(wi->merge_count_total, 1);

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 0);

    ecs_fini(world);
}

static int sys_add_tag_invoked = 0;
static int sys_no_readonly_invoked = 0;

static void sys_add_tag(ecs_iter_t *it) {
  ecs_new_w(it->world, TagA);
  ecs_new_w(it->world, TagB);
  sys_add_tag_invoked ++;
  test_assert(sys_a_invoked == 0);
}

static void sys_no_readonly(ecs_iter_t *it) {
    test_assert(sys_a_invoked == 1);
    test_assert(it->world == it->real_world);
    test_assert(!ecs_stage_is_readonly(it->real_world));
    sys_no_readonly_invoked ++;
}

void Pipeline_on_merge_activate_system_before_merge(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG_DEFINE(world, TagA);
    ECS_TAG_DEFINE(world, TagB);

    // system is annotated with TagA but writes both TagA, TagB
    ecs_system(world, {
        .entity = ecs_entity(world, { .add = ecs_ids( ecs_dependson(EcsOnUpdate) )}),
        .query.terms = {{ TagA, .inout = EcsOut, .src.id = EcsIsEntity }},
        .callback = sys_add_tag
    });

    // no merge inserted between systems, but system activates after merge
    ecs_system(world, {
        .entity = ecs_entity(world, { .add = ecs_ids( ecs_dependson(EcsOnUpdate) )}),
        .query.terms = {{ TagB, .inout = EcsIn }},
        .callback = SysA
    });

    // read TagA, causes insertion of merge
    ecs_system(world, {
        .entity = ecs_entity(world, { .add = ecs_ids( ecs_dependson(EcsOnUpdate) )}),
        .query.terms = {{ TagA, .inout = EcsIn }},
        .immediate = true,
        .callback = sys_no_readonly
    });

    ecs_progress(world, 0);

    test_int(sys_a_invoked, 1);
    test_int(sys_add_tag_invoked, 1);
    test_int(sys_no_readonly_invoked, 1);

    test_int(ecs_count(world, TagA), 1);
    test_int(ecs_count(world, TagB), 1);

    ecs_fini(world);
}

void Pipeline_disable_phase(void) {
    ecs_world_t *world = ecs_init();

    ecs_system(world, {
        .entity = ecs_entity(world, { .add = ecs_ids( ecs_dependson(EcsOnUpdate) )}),
        .callback = SysA
    });

    ecs_system(world, {
        .entity = ecs_entity(world, { .add = ecs_ids( ecs_dependson(EcsPostUpdate) )}),
        .callback = SysB
    });

    ecs_progress(world, 0);

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);

    ecs_enable(world, EcsOnUpdate, false);

    ecs_progress(world, 0);

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 2);

    ecs_enable(world, EcsOnUpdate, true);
    ecs_enable(world, EcsPostUpdate, false);

    ecs_progress(world, 0);

    test_int(sys_a_invoked, 2);
    test_int(sys_b_invoked, 2);

    ecs_enable(world, EcsPostUpdate, true);

    ecs_progress(world, 0);

    test_int(sys_a_invoked, 3);
    test_int(sys_b_invoked, 3);

    ecs_fini(world);
}

void Pipeline_disable_parent(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t p1 = ecs_new(world);
    ecs_entity_t p2 = ecs_new(world);

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .parent = p1,
            .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
        }),
        .callback = SysA
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .parent = p2,
            .add = ecs_ids( ecs_dependson(EcsPostUpdate) )
        }),
        .callback = SysB
    });

    ecs_progress(world, 0);

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);

    ecs_enable(world, p1, false);

    ecs_progress(world, 0);

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 2);

    ecs_enable(world, p1, true);
    ecs_enable(world, p2, false);

    ecs_progress(world, 0);

    test_int(sys_a_invoked, 2);
    test_int(sys_b_invoked, 2);

    ecs_enable(world, p2, true);

    ecs_progress(world, 0);

    test_int(sys_a_invoked, 3);
    test_int(sys_b_invoked, 3);

    ecs_fini(world);
}

static int no_staging_add_position_invoked = 0;

static void NoReadonlyAddPosition(ecs_iter_t *it) {
    test_assert(it->world == it->real_world);
    no_staging_add_position_invoked ++;

    ecs_entity_t e = ecs_new(it->world);
    ecs_add(it->world, e, Position);
}

static
void Pipeline_multi_threaded_no_staging_w_add_after_read_internal(bool task_threads) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position);

    if (task_threads)
    {
        ecs_set_task_threads(world, 2);
    }
    else
    {
        ecs_set_threads(world, 2);
    }

    ecs_system(world, {
        .entity = ecs_entity(world, { .add = ecs_ids( ecs_dependson(EcsOnUpdate) )}),
        .query = { .terms = {{ ecs_id(Position) }}},
        .callback = SysA
    });

    ecs_system(world, {
        .entity = ecs_entity(world, { .add = ecs_ids( ecs_dependson(EcsOnUpdate) )}),
        .callback = NoReadonlyAddPosition,
        .immediate = true
    });

    ecs_progress(world, 0);

    test_int(sys_a_invoked, 0);
    test_int(no_staging_add_position_invoked, 1);
    test_int(ecs_count(world, Position), 1);

    ecs_progress(world, 0);

    test_int(sys_a_invoked, 1);
    test_int(no_staging_add_position_invoked, 2);
    test_int(ecs_count(world, Position), 2);

    ecs_fini(world);
}

void Pipeline_multi_threaded_no_staging_w_add_after_read(void) {
    Pipeline_multi_threaded_no_staging_w_add_after_read_internal(false);
}

void Pipeline_multi_threaded_tasks_no_staging_w_add_after_read(void) {
    Pipeline_multi_threaded_no_staging_w_add_after_read_internal(true);
}

void Pipeline_1_startup_system(void) {
    ecs_world_t *world = ecs_init();

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    ecs_system(world, {
        .entity = ecs_entity(world, { .add = ecs_ids( ecs_dependson(EcsOnStart) )}),
        .callback = SysA
    });

    ecs_system(world, {
        .entity = ecs_entity(world, { .add = ecs_ids( ecs_dependson(EcsOnUpdate) )}),
        .callback = SysB
    });

    test_int(stats->merge_count_total, 0);

    ecs_progress(world, 0);
    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);

    test_int(stats->merge_count_total, 2);

    ecs_progress(world, 0);
    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 2);

    test_int(stats->merge_count_total, 3);
    test_assert(sys_a_delta_time == 0);

    ecs_fini(world);
}

void Pipeline_2_startup_systems(void) {
    ecs_world_t *world = ecs_init();

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    ecs_system(world, {
        .entity = ecs_entity(world, { .add = ecs_ids( ecs_dependson(EcsOnStart) )}),
        .callback = SysA
    });

    ecs_system(world, {
        .entity = ecs_entity(world, { .add = ecs_ids( ecs_dependson(EcsOnStart) )}),
        .callback = SysB
    });

    ecs_system(world, {
        .entity = ecs_entity(world, { .add = ecs_ids( ecs_dependson(EcsOnUpdate) )}),
        .callback = SysC
    });

    test_int(stats->merge_count_total, 0);

    ecs_progress(world, 0);
    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);

    test_int(stats->merge_count_total, 2);

    ecs_progress(world, 0);
    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 2);

    test_int(stats->merge_count_total, 3);
    test_assert(sys_a_delta_time == 0);
    test_assert(sys_b_delta_time == 0);

    ecs_fini(world);
}

void Pipeline_2_startup_phases(void) {
    ecs_world_t *world = ecs_init();

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    ecs_entity_t AfterStart = ecs_new_w_id(world, EcsPhase);
    ecs_add_pair(world, AfterStart, EcsDependsOn, EcsOnStart);

    ecs_system(world, {
        .entity = ecs_entity(world, { .add = ecs_ids( ecs_dependson(AfterStart) )}),
        .callback = SysB
    });

    ecs_system(world, {
        .entity = ecs_entity(world, { .add = ecs_ids( ecs_dependson(EcsOnStart) )}),
        .callback = SysA
    });

    ecs_system(world, {
        .entity = ecs_entity(world, { .add = ecs_ids( ecs_dependson(EcsOnUpdate) )}),
        .callback = SysC
    });

    test_int(stats->merge_count_total, 0);

    ecs_progress(world, 0);
    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);

    test_int(stats->merge_count_total, 2);

    ecs_progress(world, 0);
    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 2);

    test_int(stats->merge_count_total, 3);
    test_assert(sys_a_delta_time == 0);
    test_assert(sys_b_delta_time == 0);

    ecs_fini(world);
}

void Pipeline_2_startup_systems_w_merge(void) {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_new_w(world, Position);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    ecs_system(world, {
        .entity = ecs_entity(world, { .add = ecs_ids( ecs_dependson(EcsOnStart) )}),
        .query = { .terms = {{ ecs_id(Position), .src.id = EcsIsEntity, .inout = EcsOut }}},
        .callback = SysA
    });

    ecs_system(world, {
        .entity = ecs_entity(world, { .add = ecs_ids( ecs_dependson(EcsOnStart) )}),
        .query = { .terms = {{ ecs_id(Position) }}},
        .callback = SysB
    });

    ecs_system(world, {
        .entity = ecs_entity(world, { .add = ecs_ids( ecs_dependson(EcsOnUpdate) )}),
        .callback = SysC
    });

    test_int(stats->merge_count_total, 0);

    ecs_progress(world, 0);
    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);

    test_int(stats->merge_count_total, 3);

    ecs_progress(world, 0);
    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 2);

    test_int(stats->merge_count_total, 4);

    ecs_fini(world);
}

void Pipeline_inactive_last_system_merge_count(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
        }),
        .query.terms = {
            { TagA },
            { TagB, .src.id = EcsIsEntity, .inout = EcsOut }
        },
        .callback = SysA
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
        }),
        .query.terms = {
            { TagB },
            { TagC, .src.id = EcsIsEntity, .inout = EcsOut }
        },
        .callback = SysB
    });

    test_int(stats->merge_count_total, 0);

    ecs_new_w(world, TagA);

    ecs_progress(world, 0);

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 0);

    test_int(stats->merge_count_total, 1);

    ecs_fini(world);
}

void Pipeline_inactive_middle_system_merge_count(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
        }),
        .query.terms = {
            { TagA },
            { TagB, .src.id = EcsIsEntity, .inout = EcsOut }
        },
        .callback = SysA
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
        }),
        .query.terms = {
            { TagB },
            { TagC, .src.id = EcsIsEntity, .inout = EcsOut }
        },
        .callback = SysC
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
        }),
        .query.terms = {
            { TagC },
            { TagD, .src.id = EcsIsEntity, .inout = EcsOut }
        },
        .callback = SysD
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
        }),
        .query.terms = {
            { TagD }
        },
        .callback = SysB
    });

    test_int(stats->merge_count_total, 0);

    ecs_new_w(world, TagA);
    ecs_new_w(world, TagD);

    ecs_progress(world, 0);

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 0);
    test_int(sys_d_invoked, 0);

    test_int(stats->merge_count_total, 2);

    ecs_fini(world);
}

static
void CreateEntity(ecs_iter_t *it) {
    ecs_id_t tag = ecs_field_id(it, 0);
    ecs_new_w_id(it->world, tag);
}

void Pipeline_last_no_readonly_system_merge_count(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
        }),
        .query.terms = {
            { TagA },
            { TagB, .src.id = EcsIsEntity, .inout = EcsOut }
        },
        .callback = SysA
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( ecs_dependson(EcsOnUpdate) )
        }),
        .query.terms = {
            { TagD }
        },
        .callback = CreateEntity,
        .immediate = true
    });

    test_int(stats->merge_count_total, 0);

    ecs_new_w(world, TagA);
    ecs_new_w(world, TagD);

    test_int(1, ecs_count(world, TagD));

    ecs_progress(world, 0);

    test_int(2, ecs_count(world, TagD));

    test_int(stats->merge_count_total, 1);

    ecs_fini(world);
}

void Pipeline_2_pipelines_1_system(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Phase1);
    ECS_TAG(world, Phase2);

    ecs_entity_t p1 = ecs_pipeline(world, {
        .query = {
            .terms = {
                { .id = EcsSystem },
                { .id = Phase1 }
            }
        }
    });

    ecs_entity_t p2 = ecs_pipeline(world, {
        .query = {
            .terms = {
                { .id = EcsSystem },
                { .id = Phase2 }
            }
        }
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {
            .add = ecs_ids( Phase1 )
        }),
        .callback = SysA
    });

    ecs_progress(world, 0);
    test_int(sys_a_invoked, 0);

    ecs_set_pipeline(world, p1);
    ecs_progress(world, 0);
    test_int(sys_a_invoked, 1);

    ecs_set_pipeline(world, p2);
    ecs_progress(world, 0);
    test_int(sys_a_invoked, 1);

    ecs_fini(world);
}

void Pipeline_builtin_pipeline_w_self_system_term(void) {
    ecs_world_t *world = ecs_init();

    ecs_entity_t pipeline = ecs_get_pipeline(world);
    test_assert(pipeline != 0);

    const EcsPoly *p = ecs_get_pair(world, pipeline, EcsPoly, EcsQuery);
    test_assert(p != NULL);
    test_assert(flecs_poly_is(p->poly, ecs_query_t));

    ecs_query_t *q = p->poly;
    test_assert(q != NULL);
    test_assert((q->terms[0].src.id & EcsTraverseFlags) == EcsSelf);

    ecs_fini(world);
}

void Pipeline_custom_pipeline_w_self_system_term(void) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t pipeline = ecs_pipeline(world, {
        .query.terms = {
            { EcsSystem },
            { Tag }
        }
    });
    test_assert(pipeline != 0);

    const EcsPoly *p = ecs_get_pair(world, pipeline, EcsPoly, EcsQuery);
    test_assert(p != NULL);
    test_assert(flecs_poly_is(p->poly, ecs_query_t));

    ecs_query_t *q = p->poly;
    test_assert(q != NULL);
    test_assert((q->terms[0].src.id & EcsTraverseFlags) == EcsSelf);

    ecs_fini(world);
}

void Pipeline_switch_from_threads_to_tasks(void) {
    ecs_world_t* world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position);

    ecs_system(world, {
        .entity = ecs_entity(world, {.add = ecs_ids( ecs_dependson(EcsOnUpdate) )}),
        .query = {.terms = {{ ecs_id(Position) }}},
        .callback = SysA
    });

    ecs_system(world, {
        .entity = ecs_entity(world, {.add = ecs_ids( ecs_dependson(EcsOnUpdate) )}),
        .callback = NoReadonlyAddPosition,
        .immediate = true
    });

    ecs_set_threads(world, 2);

    ecs_progress(world, 0);

    test_int(sys_a_invoked, 0);
    test_int(no_staging_add_position_invoked, 1);
    test_int(ecs_count(world, Position), 1);

    ecs_set_task_threads(world, 2);

    ecs_progress(world, 0);

    test_int(sys_a_invoked, 1);
    test_int(no_staging_add_position_invoked, 2);
    test_int(ecs_count(world, Position), 2);

    ecs_fini(world);
}

void Pipeline_switch_from_tasks_to_threads(void) {
    ecs_world_t* world = ecs_init();

    ECS_COMPONENT_DEFINE(world, Position);

    ecs_system(world, {
        .entity = ecs_entity(world, {.add = ecs_ids( ecs_dependson(EcsOnUpdate) )}),
        .query = {.terms = {{ ecs_id(Position) }}},
        .callback = SysA
        });

    ecs_system(world, {
        .entity = ecs_entity(world, {.add = ecs_ids( ecs_dependson(EcsOnUpdate) )}),
        .callback = NoReadonlyAddPosition,
        .immediate = true
        });

    ecs_set_threads(world, 2);

    ecs_progress(world, 0);

    test_int(sys_a_invoked, 0);
    test_int(no_staging_add_position_invoked, 1);
    test_int(ecs_count(world, Position), 1);

    ecs_set_task_threads(world, 2);

    ecs_progress(world, 0);

    test_int(sys_a_invoked, 1);
    test_int(no_staging_add_position_invoked, 2);
    test_int(ecs_count(world, Position), 2);

    ecs_fini(world);
}

void Pipeline_run_pipeline_multithreaded_internal(bool task_threads) {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ecs_entity_t s1 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysA", .add = ecs_ids(Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysA,
        .multi_threaded = true
    });
    ecs_entity_t s2 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysB", .add = ecs_ids(Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysB
    });
    ecs_entity_t s3 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysC", .add = ecs_ids(Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysC
    });
    ecs_entity_t s4 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysD", .add = ecs_ids(Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysD,
        .multi_threaded = true
    });
    ecs_entity_t s5 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysE", .add = ecs_ids(Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysE,
        .multi_threaded = true
    });
    ecs_entity_t s6 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = ecs_entity(world, { .name = "SysF", .add = ecs_ids(Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)) }),
        .query.expr = "Position",
        .callback = SysF
    });

    ECS_PIPELINE(world, P, flecs.system.System, flecs.pipeline.Phase(cascade DependsOn), Tag);

    test_assert(s1 != 0);
    test_assert(s2 != 0);
    test_assert(s3 != 0);
    test_assert(s4 != 0);
    test_assert(s5 != 0);
    test_assert(s6 != 0);

    ecs_new_w(world, Position);
    ecs_new_w(world, Position);

    if (task_threads)
    {
        ecs_set_task_threads(world, 2);
    }
    else
    {
    	ecs_set_threads(world, 2);
    }

    ecs_run_pipeline(world, P, 1);

    test_int(sys_a_invoked, 2);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);
    test_int(sys_d_invoked, 2);
    test_int(sys_e_invoked, 2);
    test_int(sys_f_invoked, 1);

    ecs_pipeline_stats_t stats = {0};
    test_bool(ecs_pipeline_stats_get(world, P, &stats), true);

    test_int(ecs_vec_count(&stats.systems), 10);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[0], s1);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[1], 0); /* merge */
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[2], s2);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[3], s3);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[4], 0); /* merge */
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[5], s4);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[6], s5);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[7], 0); /* merge */
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[8], s6);
    test_int(ecs_vec_get_t(&stats.systems, ecs_entity_t, 0)[9], 0); /* merge */

    ecs_pipeline_stats_fini(&stats);

    ecs_fini(world);
}

void Pipeline_run_pipeline_multithreaded(void) {
    Pipeline_run_pipeline_multithreaded_internal(false);
}

void Pipeline_run_pipeline_multithreaded_tasks(void) {
    Pipeline_run_pipeline_multithreaded_internal(true);
}

void Pipeline_pipeline_init_no_terms(void) {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    test_expect_abort();
    ecs_pipeline(world, { 0 });
}

void Pipeline_pipeline_init_no_system_term(void) {
    install_test_abort();

    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    test_expect_abort();
    ecs_pipeline(world, {
        .query.terms = {{ ecs_id(Position) }}
    });
}

static ECS_DECLARE(ToggleTag);
static ecs_entity_t toggle_entity = 0;
static int toggle_immediate_system_invoked = 0;

static void ToggleImmediateSystem(ecs_iter_t *it) {
    ecs_defer_suspend(it->world);
    ecs_enable_id(it->world, toggle_entity, ToggleTag, false);
    ecs_defer_resume(it->world);
    toggle_immediate_system_invoked ++;
}

void Pipeline_disable_component_from_immediate_system(void) {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY_DEFINE(world, ToggleTag, CanToggle);

    toggle_entity = ecs_new_w(world, ToggleTag);

    ecs_system(world, {
        .entity = ecs_entity(world, { 
            .add = ecs_ids(ecs_pair(EcsDependsOn, EcsOnUpdate)) 
        }),
        .callback = ToggleImmediateSystem,
        .immediate = true
    });

    ecs_progress(world, 0);
    test_int(toggle_immediate_system_invoked, 1);
    test_bool(ecs_is_deferred(world), false);

    ecs_fini(world);
}
