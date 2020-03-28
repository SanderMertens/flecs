#include <api.h>

static bool system_a_invoked;
static bool system_b_invoked;

static
void SystemA(ecs_rows_t *rows) {
    test_int(rows->delta_time, 1.0);
    test_int(rows->delta_system_time, 3.0);
    system_a_invoked = true;
}

static 
void SystemB(ecs_rows_t *rows) {
    test_int(rows->delta_time, 1.0);
    test_int(rows->delta_system_time, 3.0);
    system_b_invoked = true;
}

void Timer_timeout() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, SystemA, EcsOnUpdate, Position);

    ecs_new(world, Position);

    ecs_entity_t timer = ecs_set_timeout(world, SystemA, 3.0);
    test_assert(timer != 0);
    test_assert(timer == SystemA);

    test_bool(system_a_invoked, false);
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);

    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, true);

    system_a_invoked = false;

    /* Make sure this was a one-shot timer */
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);    
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);

    ecs_fini(world);
}

void Timer_interval() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, SystemA, EcsOnUpdate, Position);

    ecs_new(world, Position);

    ecs_entity_t timer = ecs_set_interval(world, SystemA, 3.0);
    test_assert(timer != 0);
    test_assert(timer == SystemA);

    test_bool(system_a_invoked, false);
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, true);

    system_a_invoked = false;

    /* Make sure this was not a one-shot timer */
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);    
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, true);

    ecs_fini(world);
}

void Timer_shared_timeout() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, SystemA, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SystemB, EcsOnUpdate, Position);

    ecs_new(world, Position);

    ecs_entity_t timer = ecs_set_timeout(world, 0, 3.0);
    test_assert(timer != 0);

    ecs_set_tick_source(world, SystemA, timer);
    ecs_set_tick_source(world, SystemB, timer);

    test_bool(system_a_invoked, false);
    test_bool(system_b_invoked, false);
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);
    test_bool(system_b_invoked, false);
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);
    test_bool(system_b_invoked, false);
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, true);
    test_bool(system_b_invoked, true);

    system_a_invoked = false;
    system_b_invoked = false;

    /* Make sure this was a one-shot timer */
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);  
    test_bool(system_b_invoked, false);  
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);
    test_bool(system_b_invoked, false);
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);
    test_bool(system_b_invoked, false);

    ecs_fini(world);
}

void Timer_shared_interval() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, SystemA, EcsOnUpdate, Position);
    ECS_SYSTEM(world, SystemB, EcsOnUpdate, Position);

    ecs_new(world, Position);

    ecs_entity_t timer = ecs_set_interval(world, 0, 3.0);
    test_assert(timer != 0);

    ecs_set_tick_source(world, SystemA, timer);
    ecs_set_tick_source(world, SystemB, timer);

    test_bool(system_a_invoked, false);
    test_bool(system_b_invoked, false);
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);
    test_bool(system_b_invoked, false);
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);
    test_bool(system_b_invoked, false);
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, true);
    test_bool(system_b_invoked, true);

    system_a_invoked = false;
    system_b_invoked = false;

    /* Make sure this was a one-shot timer */
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);  
    test_bool(system_b_invoked, false);  
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);
    test_bool(system_b_invoked, false);
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, true);
    test_bool(system_b_invoked, true);

    ecs_fini(world);
}

void Timer_zero_timeout() {
    // Implement testcase
}

void Timer_zero_interval() {
    // Implement testcase
}
