#include <addons.h>

void Pipeline_setup() {
}

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

void SysA(ecs_iter_t *it) { 
    ecs_os_ainc(&sys_a_invoked);
    sys_a_real_world = it->world == it->real_world;
    sys_a_world_readonly = ecs_stage_is_readonly(it->real_world);
}
void SysB(ecs_iter_t *it) { 
    test_assert(sys_a_invoked != 0);
    ecs_os_ainc(&sys_b_invoked);
    sys_b_real_world = it->world == it->real_world;
    sys_b_world_readonly = ecs_stage_is_readonly(it->real_world);
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

void Pipeline_system_order_same_phase_after_activate() {    
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

void Pipeline_system_order_different_phase_after_activate() {
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

void Pipeline_system_order_after_new_system_lower_id() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position);

    ecs_entity_t Sys = ecs_new(world, 0);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysC, EcsOnUpdate, Position);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    /* Create new system with Sys id */
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = {.entity = Sys, .name = "SysA", .add = {ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Position",
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

void Pipeline_system_order_after_new_system_inbetween_id() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysA, EcsOnUpdate, Position);
    ecs_entity_t Sys = ecs_new(world, 0);
    ECS_SYSTEM(world, SysC, EcsOnUpdate, Position);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    /* Create new system with Sys id */
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = {.entity = Sys, .name = "SysB", .add = {ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Position",
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

void Pipeline_system_order_after_new_system_higher_id() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysA, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Position);
    ecs_entity_t Sys = ecs_new(world, 0);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    /* Create new system with Sys id */
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = {.entity = Sys, .name = "SysC", .add = {ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Position",
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
    ecs_id_t ecs_id(Velocity) = ecs_term_id(it, 2);

    sys_out_invoked ++;

    int i;
    for (i = 0; i < it->count; i ++) {
        ecs_set(it->world, it->entities[i], Velocity, {10, 20});
    }
}

static void SysOutMain(ecs_iter_t *it) {
    Velocity *v = ecs_term(it, Velocity, 2);

    sys_out_invoked ++;

    int i;
    for (i = 0; i < it->count; i ++) {
        v[i].x = 10;
        v[i].y = 20;
    }
}

static void SysIn(ecs_iter_t *it) {
    ecs_id_t ecs_id(Velocity) = ecs_term_id(it, 1);

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
    Velocity *v = ecs_term(it, Velocity, 1);
    ecs_id_t ecs_id(Velocity) = ecs_term_id(it, 1);

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

void Pipeline_merge_after_staged_out() {
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
    test_int(stats->pipeline_build_count_total, 2);

    test_int(sys_out_invoked, 1);
    test_int(sys_in_invoked, 1);

    ecs_progress(world, 1);
    test_int(stats->pipeline_build_count_total, 2);

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

void Pipeline_merge_after_staged_in_out() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position, Velocity);

    /* Requires merge, because getting value in 2nd system cannot access data
     * written to stage from first system */
    ECS_SYSTEM(world, SysOut, EcsOnUpdate, Position, Velocity());
    ECS_SYSTEM(world, SysIn, EcsOnUpdate, Velocity());

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

void Pipeline_merge_after_staged_inout_main_implicit_inout() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position, Velocity);

    ECS_SYSTEM(world, SysOut, EcsOnUpdate, Position, Velocity());
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

void Pipeline_merge_after_staged_inout_main_inout() {
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

void Pipeline_merge_after_staged_out_before_owned() {
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

void Pipeline_switch_pipeline() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysA, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysC, EcsOnUpdate, Position);

    ecs_add(world, ecs_id(SysC), Tag);

    ECS_PIPELINE(world, P1, flecs.system.System, flecs.pipeline.Phase(cascade(DependsOn)), !Tag);

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

void Pipeline_run_pipeline() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysA, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysC, EcsPostUpdate, Position);

    ecs_add(world, ecs_id(SysC), Tag);

    ECS_PIPELINE(world, P1, flecs.system.System, flecs.pipeline.Phase(cascade(DependsOn)), !Tag);

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

void Pipeline_get_pipeline_from_stage() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t pipeline = ecs_get_pipeline(world);
    test_assert(pipeline != 0);

    /* Get default stage */
    ecs_world_t *stage = ecs_get_stage(world, 0);
    test_assert(stage != NULL);

    test_assert(pipeline == ecs_get_pipeline(stage));

    ecs_fini(world);
}

void Pipeline_3_systems_3_types() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_entity_t s1 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysA", .add = {ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Position",
        .callback = SysA
    });
    ecs_entity_t s2 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = NULL, .add = {ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Position",
        .callback = SysB
    });
    ecs_entity_t s3 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysC", .add = {ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Position()",
        .callback = SysC
    });

    test_assert(s1 != 0);
    test_assert(s2 != 0);
    test_assert(s3 != 0);

    ecs_add_id(world, s3, Tag);

    ecs_new(world, Position);

    ecs_progress(world, 1);  

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);

    ecs_fini(world);
}

static
void RandomWrite(ecs_iter_t *it) {
    ecs_entity_t ecs_id(Position) = ecs_term_id(it, 2);

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
    ecs_entity_t ecs_id(Position) = ecs_term_id(it, 2);

    int i;
    for (i = 0; i < it->count; i ++) {
        test_assert(!ecs_has(it->world, it->entities[i], Position));
        const Position *p = ecs_get(it->world, it->entities[i], Position);
        test_assert(p == NULL);
    }
}

void Pipeline_random_read_after_random_write_out_in() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ECS_SYSTEM(world, RandomWrite, EcsOnUpdate, Tag, [out] Position());
    ECS_SYSTEM(world, RandomRead, EcsOnUpdate, Tag, [in] Position());
    
    ecs_entity_t e = ecs_new(world, Tag);

    ecs_progress(world, 1);  

    test_assert(ecs_has(world, e, Position));
    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Pipeline_random_read_after_random_write_inout_in() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ECS_SYSTEM(world, RandomWrite, EcsOnUpdate, Tag, [inout] Position());
    ECS_SYSTEM(world, RandomRead, EcsOnUpdate, Tag, [in] Position());
    
    ecs_entity_t e = ecs_new(world, Tag);

    ecs_progress(world, 1);  

    test_assert(ecs_has(world, e, Position));
    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Pipeline_random_read_after_random_write_out_inout() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ECS_SYSTEM(world, RandomWrite, EcsOnUpdate, Tag, [out] Position());
    ECS_SYSTEM(world, RandomRead, EcsOnUpdate, Tag, [inout] Position());
    
    ecs_entity_t e = ecs_new(world, Tag);

    ecs_progress(world, 1);  

    test_assert(ecs_has(world, e, Position));
    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Pipeline_random_read_after_random_write_inout_inout() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ECS_SYSTEM(world, RandomWrite, EcsOnUpdate, Tag, [inout] Position());
    ECS_SYSTEM(world, RandomRead, EcsOnUpdate, Tag, [inout] Position());
    
    ecs_entity_t e = ecs_new(world, Tag);

    ecs_progress(world, 1);  

    test_assert(ecs_has(world, e, Position));
    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Pipeline_random_read_after_random_write_w_not_write() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ECS_SYSTEM(world, RandomWrite, EcsOnUpdate, Tag, [out] !Position);
    ECS_SYSTEM(world, RandomRead, EcsOnUpdate, Tag, [in] Position());
    
    ecs_entity_t e = ecs_new(world, Tag);

    ecs_progress(world, 1);  

    test_assert(ecs_has(world, e, Position));
    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Pipeline_random_read_after_random_write_w_not_read() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ECS_SYSTEM(world, RandomWrite, EcsOnUpdate, Tag, [out] Position());
    ECS_SYSTEM(world, RandomRead_Not, EcsOnUpdate, Tag, [in] !Position);
    
    ecs_entity_t e = ecs_new(world, Tag);

    ecs_progress(world, 1);

    test_assert(ecs_has(world, e, Position));
    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Pipeline_random_read_after_random_write_w_wildcard() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ECS_SYSTEM(world, RandomWrite, EcsOnUpdate, Tag, [out] Position());
    ECS_SYSTEM(world, RandomRead, EcsOnUpdate, Tag, [in] *());
    
    ecs_entity_t e = ecs_new(world, Tag);

    ecs_progress(world, 1);

    test_assert(ecs_has(world, e, Position));
    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 1);
    test_int(p->y, 2);

    ecs_fini(world);
}

void Pipeline_random_in_after_random_inout_after_random_out() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ECS_SYSTEM(world, RandomWrite, EcsOnUpdate, Tag, [out] Position());
    ECS_SYSTEM(world, RandomReadWrite, EcsOnUpdate, Tag, [inout] Position());
    ECS_SYSTEM(world, RandomReadAfterRW, EcsOnUpdate, Tag, [in] Position());
    
    ecs_entity_t e = ecs_new(world, Tag);

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

void Pipeline_system_reverse_order_by_phase_custom_pipeline() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);

    ecs_entity_t PreFrame = ecs_new_w_id(world, EcsPhase);
    ecs_entity_t OnFrame = ecs_new_w_id(world, EcsPhase);
    ecs_entity_t PostFrame = ecs_new_w_id(world, EcsPhase);
    ecs_add_pair(world, OnFrame, EcsDependsOn, PreFrame);
    ecs_add_pair(world, PostFrame, EcsDependsOn, OnFrame);

    ECS_PIPELINE(world, P, flecs.system.System, flecs.pipeline.Phase(cascade(DependsOn)), Tag);

    int count = 0;

    ecs_system_init(world, &(ecs_system_desc_t){
        .callback = cb_third,
        .ctx = &count,
        .entity.add = {Tag, ecs_pair(EcsDependsOn, PostFrame)}
    });

    ecs_system_init(world, &(ecs_system_desc_t){
        .callback = cb_second,
        .ctx = &count,
        .entity.add = {Tag, ecs_pair(EcsDependsOn, OnFrame)}
    });

    ecs_system_init(world, &(ecs_system_desc_t){
        .callback = cb_first,
        .ctx = &count,
        .entity.add = {Tag, ecs_pair(EcsDependsOn, PreFrame)}
    });

    test_int(count, 0);

    ecs_set_pipeline(world, P);

    ecs_progress(world, 0);

    test_int(count, 3);

    ecs_fini(world);
}

void Pipeline_stage_write_before_read() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ecs_entity_t s1 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysA", .add = {Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Position",
        .callback = SysA
    });
    ecs_entity_t s2 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysB", .add = {Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Position(), Position",
        .callback = SysB
    });
    ecs_entity_t s3 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysC", .add = {Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Position",
        .callback = SysC
    });

    ECS_PIPELINE(world, P, flecs.system.System, flecs.pipeline.Phase(cascade(DependsOn)), Tag);
    ecs_set_pipeline(world, P);

    test_assert(s1 != 0);
    test_assert(s2 != 0);
    test_assert(s3 != 0);

    ecs_new(world, Position);

    ecs_progress(world, 1);  

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);

    ecs_pipeline_stats_t stats = {0};
    test_bool(ecs_pipeline_stats_get(world, 
        ecs_get_pipeline(world), &stats), true);

    test_int(ecs_vector_count(stats.systems), 5);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[0], s1);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[1], s2);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[2], 0); /* merge */
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[3], s3);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[4], 0); /* merge */

    ecs_pipeline_stats_fini(&stats);

    ecs_fini(world);
}

void Pipeline_mixed_multithreaded() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ecs_entity_t s1 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysA", .add = {Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Position",
        .callback = SysA,
        .multi_threaded = true
    });
    ecs_entity_t s2 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysB", .add = {Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Position",
        .callback = SysB
    });
    ecs_entity_t s3 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysC", .add = {Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Position",
        .callback = SysC
    });
    ecs_entity_t s4 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysD", .add = {Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Position",
        .callback = SysD,
        .multi_threaded = true
    });
    ecs_entity_t s5 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysE", .add = {Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Position",
        .callback = SysE,
        .multi_threaded = true
    });
    ecs_entity_t s6 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysF", .add = {Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Position",
        .callback = SysF
    });

    ECS_PIPELINE(world, P, flecs.system.System, flecs.pipeline.Phase(cascade(DependsOn)), Tag);
    ecs_set_pipeline(world, P);

    test_assert(s1 != 0);
    test_assert(s2 != 0);
    test_assert(s3 != 0);
    test_assert(s4 != 0);
    test_assert(s5 != 0);
    test_assert(s6 != 0);

    ecs_new(world, Position);
    ecs_new(world, Position);

    ecs_set_threads(world, 2);

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

    test_int(ecs_vector_count(stats.systems), 10);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[0], s1);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[1], 0); /* merge */
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[2], s2);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[3], s3);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[4], 0); /* merge */
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[5], s4);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[6], s5);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[7], 0); /* merge */
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[8], s6);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[9], 0); /* merge */

    ecs_pipeline_stats_fini(&stats);

    ecs_fini(world);
}

void Pipeline_mixed_staging() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ecs_entity_t s1 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysA", .add = {Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Position",
        .callback = SysA,
        .no_staging = true
    });
    ecs_entity_t s2 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysB", .add = {Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Position",
        .callback = SysB
    });
    ecs_entity_t s3 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysC", .add = {Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Position",
        .callback = SysC
    });
    ecs_entity_t s4 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysD", .add = {Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Position",
        .callback = SysD,
        .no_staging = true
    });
    ecs_entity_t s5 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysE", .add = {Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Position",
        .callback = SysE,
        .no_staging = true
    });
    ecs_entity_t s6 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysF", .add = {Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Position",
        .callback = SysF
    });

    ECS_PIPELINE(world, P, flecs.system.System, flecs.pipeline.Phase(cascade(DependsOn)), Tag);
    ecs_set_pipeline(world, P);

    test_assert(s1 != 0);
    test_assert(s2 != 0);
    test_assert(s3 != 0);
    test_assert(s4 != 0);
    test_assert(s5 != 0);
    test_assert(s6 != 0);

    ecs_new(world, Position);
    ecs_new(world, Position);

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

    test_int(ecs_vector_count(stats.systems), 10);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[0], s1);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[1], 0); /* merge */
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[2], s2);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[3], s3);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[4], 0); /* merge */
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[5], s4);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[6], s5);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[7], 0); /* merge */
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[8], s6);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[9], 0); /* merge */

    ecs_pipeline_stats_fini(&stats);

    ecs_fini(world);
}

static
void WritePosition(ecs_iter_t *it) {
    if (*(bool*)it->ctx) {
        ecs_entity_t ecs_id(Position) = ecs_term_id(it, 2);
        for (int i = 0; i < it->count; i ++) {
            ecs_add(it->world, it->entities[i], Position);
        }
    }
}

void Pipeline_single_threaded_pipeline_change() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    bool write_position = false;

    ecs_entity_t s1 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysA", .add = {ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Tag, !Position",
        .callback = SysA
    });

    ecs_entity_t s2 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysB", .add = {ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Tag, Position",
        .callback = SysB
    });

    ecs_entity_t s3 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "WritePosition", .add = {ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Tag, [out] Position()",
        .callback = WritePosition,
        .ctx = &write_position
    });

    ecs_entity_t s4 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysC", .add = {ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Tag, Position",
        .callback = SysC
    });

    ecs_entity_t s5 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysD", .add = {ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Tag, !Position",
        .callback = SysD
    });

    /* Initialize to 1 so asserts inside system won't trigger */
    sys_a_invoked = 1;
    sys_b_invoked = 1;
    sys_c_invoked = 1;
    sys_d_invoked = 1;

    ecs_new(world, Tag);
    ecs_new(world, Tag);

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

void Pipeline_multi_threaded_pipeline_change() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    bool write_position = false;

    ecs_entity_t s1 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysA", .add = {ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Tag, !Position",
        .callback = SysA,
        .multi_threaded = true
    });

    ecs_entity_t s2 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysB", .add = {ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Tag, Position, [out] Position()",
        .callback = SysB,
        .multi_threaded = true
    });

    ecs_entity_t s3 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "WritePosition", .add = {ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Tag, [out] Position(), [in] ?Position",
        .callback = WritePosition,
        .ctx = &write_position,
        .multi_threaded = true
    });

    ecs_entity_t s4 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysC", .add = {ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Tag, Position",
        .callback = SysC,
        .multi_threaded = true
    });

    ecs_entity_t s5 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysD", .add = {ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Tag, !Position",
        .callback = SysD,
        .multi_threaded = true
    });

    ecs_set_threads(world, 2);

    /* Initialize to 1 so asserts inside system won't trigger */
    sys_a_invoked = 1;
    sys_b_invoked = 1;
    sys_c_invoked = 1;
    sys_d_invoked = 1;

    ecs_new(world, Tag);
    ecs_new(world, Tag);

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

void Pipeline_activate_after_add() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    bool write_position = false;

    ecs_entity_t s1 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "WritePosition", .add = {ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Tag, [out] Position()",
        .callback = WritePosition,
        .ctx = &write_position
    });

    ecs_entity_t s2 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysA", .add = {ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "Position",
        .callback = SysA
    });

    ecs_new(world, Tag);

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
    q_result = ecs_query_new(it->world, "Position");
}

void Pipeline_no_staging_system_create_query() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    ecs_entity_t s = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "CreateQuery", .add = {Tag, ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .callback = CreateQuery,
        .no_staging = true
    });

    ECS_PIPELINE(world, P, flecs.system.System, flecs.pipeline.Phase(cascade(DependsOn)), Tag);
    ecs_set_pipeline(world, P);

    test_assert(s != 0);

    ecs_progress(world, 1.0);

    test_assert(q_result != NULL);

    ecs_pipeline_stats_t stats = {0};
    test_bool(ecs_pipeline_stats_get(world, 
        ecs_get_pipeline(world), &stats), true);

    test_int(ecs_vector_count(stats.systems), 2);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[0], s);
    test_int(ecs_vector_get(stats.systems, ecs_entity_t, 0)[1], 0); /* merge */

    ecs_pipeline_stats_fini(&stats);

    ecs_fini(world);
}

ECS_DECLARE(TagA);
ECS_DECLARE(TagB);

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

void Pipeline_match_all_after_pipeline_rebuild() {
    ecs_world_t *world = ecs_init();

    ECS_TAG_DEFINE(world, TagA);
    ECS_TAG_DEFINE(world, TagB);

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .add = {ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "[out] TagB()",
        .callback = set_singleton
    });

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .add = {ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "TagB",
        .callback = match_singleton
    });

    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .add = {ecs_pair(EcsDependsOn, EcsOnUpdate)} },
        .query.filter.expr = "?TagA",
        .callback = match_all
    });

    ecs_progress(world, 0);

    test_int(set_singleton_invoked, 1);
    test_int(match_singleton_invoked, 1);
    test_assert(match_all_invoked >= 1);

    ecs_fini(world);
}

void Pipeline_empty_pipeline() {
    ecs_world_t *world = ecs_mini();

    ECS_IMPORT(world, FlecsPipeline);

    const ecs_world_info_t *info = ecs_get_world_info(world);
    test_int(info->frame_count_total, 0);

    ecs_progress(world, 0);

    test_int(info->frame_count_total, 1);

    ecs_fini(world);
}

void Pipeline_custom_pipeline_w_system_macro() {
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
