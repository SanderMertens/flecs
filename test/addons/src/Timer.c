#include <addons.h>

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

    ecs_entity_t filter = ecs_set_rate(world, SystemA, 3, 0);
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

    ecs_entity_t filter_a = ecs_set_rate(world, 0, 2, 0);
    test_assert(filter_a != 0);

    ecs_entity_t filter_b = ecs_set_rate(world, SystemC, 3, filter_a);
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

    ecs_entity_t filter = ecs_set_rate(world, SystemC, 3, timer);
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

void Timer_rate_filter_with_empty_src() {
   ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, SystemC, EcsOnUpdate, Position);

    ecs_new(world, Position);

    // Not actually a tick source
    ecs_entity_t filter_a = ecs_new_id(world);
    test_assert(filter_a != 0);

    ecs_entity_t filter_b = ecs_set_rate(world, SystemC, 6, filter_a);
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

void Timer_one_shot_timer_entity() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t timer = ecs_set_timeout(world, 0, 1.0);

    int i;
    for (i = 0; i < 3; i ++) {
        ecs_progress(world, 0.3);
        const EcsTickSource *src = ecs_get(world, timer, EcsTickSource);
        test_assert(src != NULL);
        test_bool(src->tick, false);
    }

    ecs_progress(world, 0.3);
    const EcsTickSource *src = ecs_get(world, timer, EcsTickSource);
    test_assert(src != NULL);
    test_bool(src->tick, true);   

    /* Ensure timer doesn't tick again */
    for (i = 0; i < 12; i ++) {
        ecs_progress(world, 0.3);
        test_bool(src->tick, false);
    }

    ecs_fini(world);
}

void Timer_interval_timer_entity() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t timer = ecs_set_interval(world, 0, 1.0);

    int i;
    for (i = 0; i < 3; i ++) {
        ecs_progress(world, 0.3);
        const EcsTickSource *src = ecs_get(world, timer, EcsTickSource);
        test_assert(src != NULL);
        test_bool(src->tick, false);
    }

    ecs_progress(world, 0.3);
    const EcsTickSource *src = ecs_get(world, timer, EcsTickSource);
    test_assert(src != NULL);
    test_bool(src->tick, true);   

    for (i = 0; i < 2; i ++) {
        ecs_progress(world, 0.3);
        test_bool(src->tick, false);
    }

    /* Timer should tick again */
    ecs_progress(world, 0.3);
    test_bool(src->tick, true);

    ecs_fini(world);
}

void Timer_rate_entity() {
    ecs_world_t *world = ecs_init();

    /* Specify 0 for source. This applies the rate to the frame ticks */
    ecs_entity_t rate = ecs_set_rate(world, 0, 4, 0);

    int i;
    for (i = 0; i < 3; i ++) {
        ecs_progress(world, 0);
        const EcsTickSource *src = ecs_get(world, rate, EcsTickSource);
        test_assert(src != NULL);
        test_bool(src->tick, false);
    }

    ecs_progress(world, 0);
    const EcsTickSource *src = ecs_get(world, rate, EcsTickSource);
    test_assert(src != NULL);
    test_bool(src->tick, true);   

    for (i = 0; i < 3; i ++) {
        ecs_progress(world, 0);
        const EcsTickSource *src = ecs_get(world, rate, EcsTickSource);
        test_assert(src != NULL);
        test_bool(src->tick, false);
    }

    /* Filter should tick again */
    ecs_progress(world, 0);
    test_bool(src->tick, true);

    ecs_fini(world);
}

void Timer_nested_rate_entity() {
    ecs_world_t *world = ecs_init();

    /* Nested rate filter */
    ecs_entity_t parent = ecs_set_rate(world, 0, 2, 0);
    ecs_entity_t rate = ecs_set_rate(world, 0, 2, parent);

    int i;
    for (i = 0; i < 3; i ++) {
        ecs_progress(world, 0);
        const EcsTickSource *src = ecs_get(world, rate, EcsTickSource);
        test_assert(src != NULL);
        test_bool(src->tick, false);
    }

    ecs_progress(world, 0);
    const EcsTickSource *src = ecs_get(world, rate, EcsTickSource);
    test_assert(src != NULL);
    test_bool(src->tick, true);   

    for (i = 0; i < 3; i ++) {
        ecs_progress(world, 0);
        const EcsTickSource *src = ecs_get(world, rate, EcsTickSource);
        test_assert(src != NULL);
        test_bool(src->tick, false);
    }

    /* Filter should tick again */
    ecs_progress(world, 0);
    test_bool(src->tick, true);

    ecs_fini(world);
}

void Timer_nested_rate_entity_empty_src() {
    ecs_world_t *world = ecs_init();

    /* Rate filter with source that is not a tick source */
    ecs_entity_t parent = ecs_new(world, 0);
    ecs_entity_t rate = ecs_set_rate(world, 0, 4, parent);

    int i;
    for (i = 0; i < 3; i ++) {
        ecs_progress(world, 0);
        const EcsTickSource *src = ecs_get(world, rate, EcsTickSource);
        test_assert(src != NULL);
        test_bool(src->tick, false);
    }

    ecs_progress(world, 0);
    const EcsTickSource *src = ecs_get(world, rate, EcsTickSource);
    test_assert(src != NULL);
    test_bool(src->tick, true);   

    for (i = 0; i < 3; i ++) {
        ecs_progress(world, 0);
        const EcsTickSource *src = ecs_get(world, rate, EcsTickSource);
        test_assert(src != NULL);
        test_bool(src->tick, false);
    }

    /* Filter should tick again */
    ecs_progress(world, 0);
    test_bool(src->tick, true);

    ecs_fini(world);
}

void Timer_naked_tick_entity() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t tick = ecs_set(world, 0, EcsTickSource, {0});

    int i;
    for (i = 0; i < 10; i ++) {
        ecs_progress(world, 0);
        const EcsTickSource *src = ecs_get(world, tick, EcsTickSource);
        test_assert(src != NULL);

        /* Tick should always be true, no filter is applied */
        test_bool(src->tick, true);
    }

    ecs_fini(world);
}
