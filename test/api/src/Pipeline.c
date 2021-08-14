#include <api.h>

void Pipeline_setup() {
    ecs_tracing_enable(-3);
}

static int sys_a_invoked;
static int sys_b_invoked;
static int sys_c_invoked;

void SysA(ecs_iter_t *it) { 
    sys_a_invoked ++; 
}
void SysB(ecs_iter_t *it) { 
    test_assert(sys_a_invoked != 0);
    sys_b_invoked ++; 
}
void SysC(ecs_iter_t *it) { 
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
    
    test_assert( ecs_has_id(world, SysB, EcsInactive));
    ecs_add(world, E, Velocity);
    test_assert( !ecs_has_id(world, SysB, EcsInactive));

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
    
    test_assert( ecs_has_id(world, SysB, EcsInactive));
    ecs_add(world, E, Velocity);
    test_assert( !ecs_has_id(world, SysB, EcsInactive));

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

    ecs_entity_t Sys = ecs_new(world, 0);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysC, EcsOnUpdate, Position);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    /* Create new system with Sys id */
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = {.entity = Sys, .name = "SysA", .add = {EcsOnUpdate} },
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
        .entity = {.entity = Sys, .name = "SysB", .add = {EcsOnUpdate} },
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
        .entity = {.entity = Sys, .name = "SysC", .add = {EcsOnUpdate} },
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

    ECS_SYSTEM(world, SysOut, EcsOnUpdate, Position, [out] :Velocity);
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

void Pipeline_no_merge_after_staged_in_out() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position, Velocity);

    ECS_SYSTEM(world, SysOut, EcsOnUpdate, Position, :Velocity);
    ECS_SYSTEM(world, SysIn, EcsOnUpdate, :Velocity);

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

void Pipeline_merge_after_staged_out_before_owned() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysOut, EcsOnUpdate, Position, [out] :Velocity);
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

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysA, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysC, EcsPostUpdate, Position);

    ECS_PIPELINE(world, P1, flecs.pipeline.OnUpdate, flecs.pipeline.PostUpdate);
    ECS_PIPELINE(world, P2, flecs.pipeline.OnUpdate);

    ecs_set_pipeline(world, P1);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    ecs_progress(world, 1);

    test_int(stats->systems_ran_frame, 3);
    test_int(stats->merge_count_total, 1);
    test_int(stats->pipeline_build_count_total, 1);

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);

    ecs_set_pipeline(world, P2);

    ecs_progress(world, 1);
    test_int(stats->pipeline_build_count_total, 2);

    test_int(sys_a_invoked, 2);
    test_int(sys_b_invoked, 2);
    test_int(sys_c_invoked, 1);

    ecs_fini(world);
}

void Pipeline_run_pipeline() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_ENTITY(world, E, Position);

    ECS_SYSTEM(world, SysA, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysB, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SysC, EcsPostUpdate, Position);

    ECS_PIPELINE(world, P1, flecs.pipeline.OnUpdate, flecs.pipeline.PostUpdate);
    ECS_PIPELINE(world, P2, flecs.pipeline.OnUpdate);

    const ecs_world_info_t *stats = ecs_get_world_info(world);

    ecs_pipeline_run(world, P1, 1);

    test_int(stats->systems_ran_frame, 3);
    test_int(stats->merge_count_total, 1);
    test_int(stats->pipeline_build_count_total, 1);

    test_int(sys_a_invoked, 1);
    test_int(sys_b_invoked, 1);
    test_int(sys_c_invoked, 1);

    ecs_pipeline_run(world, P2, 1);

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
        .entity = { .name = "SysA", .add = {EcsOnUpdate} },
        .query.filter.expr = "Position",
        .callback = SysA
    });
    ecs_entity_t s2 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = NULL, .add = {EcsOnUpdate} },
        .query.filter.expr = "Position",
        .callback = SysB
    });
    ecs_entity_t s3 = ecs_system_init(world, &(ecs_system_desc_t){
        .entity = { .name = "SysC", .add = {EcsOnUpdate} },
        .query.filter.expr = ":Position",
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

    ECS_SYSTEM(world, RandomWrite, EcsOnUpdate, Tag, [out] :Position);
    ECS_SYSTEM(world, RandomRead, EcsOnUpdate, Tag, [in] :Position);
    
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

    ECS_SYSTEM(world, RandomWrite, EcsOnUpdate, Tag, [inout] :Position);
    ECS_SYSTEM(world, RandomRead, EcsOnUpdate, Tag, [in] :Position);
    
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

    ECS_SYSTEM(world, RandomWrite, EcsOnUpdate, Tag, [out] :Position);
    ECS_SYSTEM(world, RandomRead, EcsOnUpdate, Tag, [inout] :Position);
    
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

    ECS_SYSTEM(world, RandomWrite, EcsOnUpdate, Tag, [inout] :Position);
    ECS_SYSTEM(world, RandomRead, EcsOnUpdate, Tag, [inout] :Position);
    
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
    ECS_SYSTEM(world, RandomRead, EcsOnUpdate, Tag, [in] :Position);
    
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

    ECS_SYSTEM(world, RandomWrite, EcsOnUpdate, Tag, [out] :Position);
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

    ECS_SYSTEM(world, RandomWrite, EcsOnUpdate, Tag, [out] :Position);
    ECS_SYSTEM(world, RandomRead, EcsOnUpdate, Tag, [in] :*);
    
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

    ECS_SYSTEM(world, RandomWrite, EcsOnUpdate, Tag, [out] :Position);
    ECS_SYSTEM(world, RandomReadWrite, EcsOnUpdate, Tag, [inout] :Position);
    ECS_SYSTEM(world, RandomReadAfterRW, EcsOnUpdate, Tag, [in] :Position);
    
    ecs_entity_t e = ecs_new(world, Tag);

    ecs_progress(world, 1);

    test_assert(ecs_has(world, e, Position));
    const Position *p = ecs_get(world, e, Position);
    test_int(p->x, 2);
    test_int(p->y, 3);

    ecs_fini(world);
}
