#include <api.h>

static bool system_a_invoked;
static bool system_b_invoked;
static bool system_c_invoked;

static
void SystemA(ecs_iter_t *it) {
    test_int(it->delta_time, 1.0);
    test_int(it->delta_system_time, 3.0);
    system_a_invoked = true;
}

static 
void SystemB(ecs_iter_t *it) {
    test_int(it->delta_time, 1.0);
    test_int(it->delta_system_time, 3.0);
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

void Timer_start_stop_one_shot() {
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

    /* Stop timer */
    ecs_stop_timer(world, timer);

    /* Timer is stopped, should not trigger system */
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);

    /* Start timer, this should reset timer */
    ecs_start_timer(world, timer);

    /* Make sure this was a one-shot timer */
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);    
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);
    ecs_progress(world, 1.0);

    /* System should have been triggered */
    test_bool(system_a_invoked, true);

    ecs_fini(world);
}

void Timer_start_stop_interval() {
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

    /* Stop timer */
    ecs_stop_timer(world, timer);

    /* Timer is stopped, should not trigger system */
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);

    /* Start timer, this should reset timer */
    ecs_start_timer(world, timer);

    /* Make sure this was a one-shot timer */
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);    
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);
    ecs_progress(world, 1.0);

    /* System should have been triggered */
    test_bool(system_a_invoked, true);

    system_a_invoked = false;

    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);
    ecs_progress(world, 1.0);
    test_bool(system_a_invoked, false);
    ecs_progress(world, 1.0);

    /* Ensure that timer still triggers repeatedly */
    test_bool(system_a_invoked, true);    

    ecs_fini(world);
}

void Timer_rate_filter() {
   ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, SystemA, EcsOnUpdate, Position);

    ecs_new(world, Position);

    ecs_entity_t filter = ecs_set_rate_filter(world, SystemA, 3, 0);
    test_assert(filter != 0);
    test_assert(filter == SystemA);

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
    test_bool(system_a_invoked, true);

    ecs_fini(world);
}

static
void SystemC(ecs_iter_t *it) {
    test_int(it->delta_time, 1.0);
    test_int(it->delta_system_time, 6.0);
    system_c_invoked = true;
}

void Timer_rate_filter_w_rate_filter_src() {
   ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, SystemC, EcsOnUpdate, Position);

    ecs_new(world, Position);

    ecs_entity_t filter_a = ecs_set_rate_filter(world, 0, 2, 0);
    test_assert(filter_a != 0);

    ecs_entity_t filter_b = ecs_set_rate_filter(world, SystemC, 3, filter_a);
    test_assert(filter_b != 0);
    test_assert(filter_b == SystemC);

    test_bool(system_c_invoked, false);

    int i;
    for (i = 0; i < 5; i ++) {
        ecs_progress(world, 1.0);
        test_bool(system_c_invoked, false);
    }

    ecs_progress(world, 1.0);
    test_bool(system_c_invoked, true);

    system_c_invoked = false;

    /* Make sure rate filter triggers repeatedly */
    for (i = 0; i < 5; i ++) {
        ecs_progress(world, 1.0);
        test_bool(system_c_invoked, false);
    }

    ecs_progress(world, 1.0);
    test_bool(system_c_invoked, true);

    ecs_fini(world);
}

void Timer_rate_filter_w_timer_src() {
   ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_SYSTEM(world, SystemC, EcsOnUpdate, Position);
    ECS_ENTITY(world, E1, Position);

    ecs_entity_t timer = ecs_set_interval(world, 0, 2.0);
    test_assert(timer != 0);

    ecs_entity_t filter = ecs_set_rate_filter(world, SystemC, 3, timer);
    test_assert(filter != 0);
    test_assert(filter == SystemC);

    test_bool(system_c_invoked, false);

    int i;
    for (i = 0; i < 5; i ++) {
        ecs_progress(world, 1.0);
        test_bool(system_c_invoked, false);
    }

    ecs_progress(world, 1.0);
    test_bool(system_c_invoked, true);

    system_c_invoked = false;

    /* Make sure rate filter triggers repeatedly */
    for (i = 0; i < 5; i ++) {
        ecs_progress(world, 1.0);
        test_bool(system_c_invoked, false);
    }

    ecs_progress(world, 1.0);
    test_bool(system_c_invoked, true);

    ecs_fini(world);
}
