#include <api.h>
#include <stdlib.h>

void EnabledComponents_is_component_enabled() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);

    test_bool(ecs_is_enabled_component(world, e, Position), true);

    ecs_fini(world);
}

void EnabledComponents_is_empty_entity_disabled() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);

    test_bool(ecs_is_enabled_component(world, e, Position), false);

    ecs_fini(world);
}

void EnabledComponents_is_0_entity_disabled() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    test_expect_abort();

    test_bool(ecs_is_enabled_component(world, 0, Position), false);
}

void EnabledComponents_is_0_component_disabled() {
    install_test_abort();
    
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);

    test_expect_abort();

    test_bool(ecs_is_enabled_id(world, e, 0), false);
}

void EnabledComponents_is_nonexist_component_disabled() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, Velocity);

    test_bool(ecs_is_enabled_component(world, e, Position), false);

    ecs_fini(world);
}

void EnabledComponents_is_enabled_component_enabled() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_id(world);

    ecs_enable_component(world, e, Position, true);
    test_bool(ecs_is_enabled_component(world, e, Position), true);

    ecs_fini(world);
}

void EnabledComponents_is_disabled_component_enabled() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_id(world);

    ecs_enable_component(world, e, Position, false);
    test_bool(ecs_is_enabled_component(world, e, Position), false);

    ecs_fini(world);
}

void EnabledComponents_has_enabled_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_id(world);

    ecs_enable_component(world, e, Position, true);
    
    test_bool(ecs_has_id(world, e, ECS_TOGGLE | ecs_id(Position)), true);

    ecs_fini(world);
}

void EnabledComponents_is_enabled_after_add() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_id(world);

    ecs_enable_component(world, e, Position, true);
    test_bool(ecs_is_enabled_component(world, e, Position), true);

    ecs_add(world, e, Position);
    test_bool(ecs_is_enabled_component(world, e, Position), true);
    test_bool(ecs_has_id(world, e, ECS_TOGGLE | ecs_id(Position)), true);
    test_bool(ecs_has(world, e, Position), true);

    ecs_fini(world);
}

void EnabledComponents_is_enabled_after_remove() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);

    ecs_enable_component(world, e, Position, true);
    test_bool(ecs_is_enabled_component(world, e, Position), true);

    ecs_remove(world, e, Position);
    test_bool(ecs_is_enabled_component(world, e, Position), true);
    test_bool(ecs_has_id(world, e, ECS_TOGGLE | ecs_id(Position)), true);
    test_bool(ecs_has(world, e, Position), false);

    ecs_fini(world);
}

void EnabledComponents_is_enabled_after_disable() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_id(world);

    ecs_enable_component(world, e, Position, true);
    test_bool(ecs_is_enabled_component(world, e, Position), true);

    ecs_enable_component(world, e, Position, false);
    test_bool(ecs_is_enabled_component(world, e, Position), false);

    ecs_enable_component(world, e, Position, true);
    test_bool(ecs_is_enabled_component(world, e, Position), true);

    ecs_fini(world);
}

void EnabledComponents_is_disabled_after_enable() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_id(world);

    ecs_enable_component(world, e, Position, false);
    test_bool(ecs_is_enabled_component(world, e, Position), false);

    ecs_enable_component(world, e, Position, true);
    test_bool(ecs_is_enabled_component(world, e, Position), true);

    ecs_enable_component(world, e, Position, false);
    test_bool(ecs_is_enabled_component(world, e, Position), false);

    ecs_fini(world);
}

void EnabledComponents_is_enabled_randomized() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t entities[100000];
    bool enabled[100000];

    int32_t i;
    for (i = 0; i < 100000; i ++) {
        enabled[i] = rand() % 2;
        entities[i] = ecs_new_id(world);
        ecs_enable_component(world, entities[i], Position, enabled[i]);
    }

    for (i = 0; i < 100000; i ++) {
        test_bool(
            ecs_is_enabled_component(world, entities[i], Position), enabled[i]);
    }

    ecs_fini(world);
}

void EnabledComponents_is_enabled_after_add_randomized() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t entities[100000];
    bool enabled[100000];

    int32_t i;
    for (i = 0; i < 100000; i ++) {
        enabled[i] = rand() % 2;
        entities[i] = ecs_new_id(world);
        ecs_enable_component(world, entities[i], Position, enabled[i]);
    }

    for (i = 0; i < 100000; i ++) {
        ecs_add(world, entities[i], Position);
    }

    for (i = 0; i < 100000; i ++) {
        test_bool(
            ecs_is_enabled_component(world, entities[i], Position), enabled[i]);
    }

    ecs_fini(world);
}

void EnabledComponents_is_enabled_after_randomized_add_randomized() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t entities[100000];
    bool enabled[100000];

    int32_t i;
    for (i = 0; i < 100000; i ++) {
        enabled[i] = rand() % 2;
        entities[i] = ecs_new_id(world);
        ecs_enable_component(world, entities[i], Position, enabled[i]);
    }

    for (i = 0; i < 100000; i ++) {
        if (!(rand() % 5)) {
            ecs_add(world, entities[i], Position);
        }
    }

    for (i = 0; i < 100000; i ++) {
        test_bool(
            ecs_is_enabled_component(world, entities[i], Position), enabled[i]);
    }

    ecs_fini(world);
}

void EnabledComponents_is_enabled_2() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, 0);

    ecs_enable_component(world, e, Position, true);
    test_bool(ecs_is_enabled_component(world, e, Position), true);

    ecs_enable_component(world, e, Velocity, false);
    test_bool(ecs_is_enabled_component(world, e, Position), true);
    test_bool(ecs_is_enabled_component(world, e, Velocity), false);

    ecs_fini(world);
}

void EnabledComponents_is_enabled_3() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t e = ecs_new(world, 0);

    ecs_enable_component(world, e, Position, true);
    test_bool(ecs_is_enabled_component(world, e, Position), true);

    ecs_enable_component(world, e, Velocity, false);
    test_bool(ecs_is_enabled_component(world, e, Position), true);
    test_bool(ecs_is_enabled_component(world, e, Velocity), false);

    ecs_enable_component(world, e, Mass, true);
    test_bool(ecs_is_enabled_component(world, e, Position), true);
    test_bool(ecs_is_enabled_component(world, e, Velocity), false);
    test_bool(ecs_is_enabled_component(world, e, Mass), true);

    ecs_fini(world);
}

void EnabledComponents_is_enabled_2_after_add() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, 0);

    ecs_enable_component(world, e, Position, true);
    test_bool(ecs_is_enabled_component(world, e, Position), true);

    ecs_enable_component(world, e, Velocity, false);
    test_bool(ecs_is_enabled_component(world, e, Position), true);
    test_bool(ecs_is_enabled_component(world, e, Velocity), false);

    ecs_add(world, e, Position);
    test_bool(ecs_is_enabled_component(world, e, Position), true);
    test_bool(ecs_is_enabled_component(world, e, Velocity), false);    

    ecs_fini(world);
}

void EnabledComponents_is_enabled_3_after_add() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t e = ecs_new(world, 0);

    ecs_enable_component(world, e, Position, true);
    test_bool(ecs_is_enabled_component(world, e, Position), true);

    ecs_enable_component(world, e, Velocity, false);
    test_bool(ecs_is_enabled_component(world, e, Position), true);
    test_bool(ecs_is_enabled_component(world, e, Velocity), false);

    ecs_enable_component(world, e, Mass, true);
    test_bool(ecs_is_enabled_component(world, e, Position), true);
    test_bool(ecs_is_enabled_component(world, e, Velocity), false);
    test_bool(ecs_is_enabled_component(world, e, Mass), true);

    ecs_add(world, e, Position);
    test_bool(ecs_is_enabled_component(world, e, Position), true);
    test_bool(ecs_is_enabled_component(world, e, Velocity), false);
    test_bool(ecs_is_enabled_component(world, e, Mass), true);

    ecs_fini(world);
}

void EnabledComponents_is_pair_enabled() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t obj = ecs_new_id(world);
    ecs_entity_t e = ecs_new_w_pair(world, ecs_id(Position), obj);

    test_bool(ecs_is_enabled_pair(world, e, Position, obj), true);

    ecs_fini(world);
}

void EnabledComponents_is_enabled_pair_enabled() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_id(world);
    ecs_entity_t obj = ecs_new_id(world);

    ecs_enable_pair(world, e, Position, obj, true);
    test_bool(ecs_is_enabled_pair(world, e, Position, obj), true);

    ecs_fini(world);
}

void EnabledComponents_is_disabled_pair_enabled() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_id(world);
    ecs_entity_t obj = ecs_new_id(world);

    ecs_enable_pair(world, e, Position, obj, false);
    test_bool(ecs_is_enabled_pair(world, e, Position, obj), false);

    ecs_fini(world);
}

void EnabledComponents_has_enabled_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_id(world);
    ecs_entity_t obj = ecs_new_id(world);

    ecs_enable_pair(world, e, Position, obj, true);
    test_bool(ecs_has_id(world, e, ECS_TOGGLE | ecs_pair(ecs_id(Position), obj)), true);

    ecs_fini(world);
}

void EnabledComponents_is_pair_enabled_after_add() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_id(world);
    ecs_entity_t obj = ecs_new_id(world);

    ecs_enable_pair(world, e, Position, obj, true);
    test_bool(ecs_is_enabled_pair(world, e, Position, obj), true);

    ecs_add_pair(world, e, ecs_id(Position), obj);
    test_bool(ecs_is_enabled_pair(world, e, Position, obj), true);
    test_bool(ecs_has_id(world, e, ECS_TOGGLE |  ecs_pair(ecs_id(Position), obj)), true);
    test_bool(ecs_has_pair(world, e, ecs_id(Position), obj), true);

    ecs_fini(world);
}

void EnabledComponents_is_pair_enabled_after_remove() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    ecs_entity_t obj = ecs_new_id(world);

    ecs_enable_pair(world, e, Position, obj, true);
    test_bool(ecs_is_enabled_pair(world, e, Position, obj), true);

    ecs_remove_pair(world, e, ecs_id(Position), obj);
    test_bool(ecs_is_enabled_pair(world, e, Position, obj), true);
    test_bool(ecs_has_id(world, e, ECS_TOGGLE | ecs_pair(ecs_id(Position), obj)), true);
    test_bool(ecs_has_pair(world, e, ecs_id(Position), obj), false);

    ecs_fini(world);
}

void EnabledComponents_is_pair_enabled_after_disable() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_id(world);
    ecs_entity_t obj = ecs_new_id(world);

    ecs_enable_pair(world, e, Position, obj, true);
    test_bool(ecs_is_enabled_pair(world, e, Position, obj), true);

    ecs_enable_pair(world, e, Position, obj, false);
    test_bool(ecs_is_enabled_pair(world, e, Position, obj), false);

    ecs_enable_pair(world, e, Position, obj, true);
    test_bool(ecs_is_enabled_pair(world, e, Position, obj), true);

    ecs_fini(world);
}

void EnabledComponents_is_pair_disabled_after_enable() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_id(world);
    ecs_entity_t obj = ecs_new_id(world);

    ecs_enable_pair(world, e, Position, obj, false);
    test_bool(ecs_is_enabled_pair(world, e, Position, obj), false);

    ecs_enable_pair(world, e, Position, obj, true);
    test_bool(ecs_is_enabled_pair(world, e, Position, obj), true);

    ecs_enable_pair(world, e, Position, obj, false);
    test_bool(ecs_is_enabled_pair(world, e, Position, obj), false);

    ecs_fini(world);
}

void EnabledComponents_is_pair_enabled_2() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_entity_t obj = ecs_new_id(world);

    ecs_enable_pair(world, e, Position, obj, true);
    test_bool(ecs_is_enabled_pair(world, e, Position, obj), true);

    ecs_enable_pair(world, e, Velocity, obj, false);
    test_bool(ecs_is_enabled_pair(world, e, Position, obj), true);
    test_bool(ecs_is_enabled_pair(world, e, Velocity, obj), false);

    ecs_fini(world);
}

void EnabledComponents_is_pair_enabled_3() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_entity_t obj = ecs_new_id(world);

    ecs_enable_pair(world, e, Position, obj, true);
    test_bool(ecs_is_enabled_pair(world, e, Position, obj), true);

    ecs_enable_pair(world, e, Velocity, obj, false);
    test_bool(ecs_is_enabled_pair(world, e, Position, obj), true);
    test_bool(ecs_is_enabled_pair(world, e, Velocity, obj), false);

    ecs_enable_pair(world, e, Mass, obj, true);
    test_bool(ecs_is_enabled_pair(world, e, Position, obj), true);
    test_bool(ecs_is_enabled_pair(world, e, Velocity, obj), false);
    test_bool(ecs_is_enabled_pair(world, e, Mass, obj), true);

    ecs_fini(world);
}

void EnabledComponents_is_pair_enabled_2_after_add() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_entity_t obj = ecs_new_id(world);

    ecs_enable_pair(world, e, Position, obj, true);
    test_bool(ecs_is_enabled_pair(world, e, Position, obj), true);

    ecs_enable_pair(world, e, Velocity, obj, false);
    test_bool(ecs_is_enabled_pair(world, e, Position, obj), true);
    test_bool(ecs_is_enabled_pair(world, e, Velocity, obj), false);

    ecs_add_pair(world, e, ecs_id(Position), obj);
    test_bool(ecs_is_enabled_pair(world, e, Position, obj), true);
    test_bool(ecs_is_enabled_pair(world, e, Velocity, obj), false);    

    ecs_fini(world);
}

void EnabledComponents_is_pair_enabled_3_after_add() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_entity_t obj = ecs_new_id(world);

    ecs_enable_pair(world, e, Position, obj, true);
    test_bool(ecs_is_enabled_pair(world, e, Position, obj), true);

    ecs_enable_pair(world, e, Velocity, obj, false);
    test_bool(ecs_is_enabled_pair(world, e, Position, obj), true);
    test_bool(ecs_is_enabled_pair(world, e, Velocity, obj), false);

    ecs_enable_pair(world, e, Mass, obj, true);
    test_bool(ecs_is_enabled_pair(world, e, Position, obj), true);
    test_bool(ecs_is_enabled_pair(world, e, Velocity, obj), false);
    test_bool(ecs_is_enabled_pair(world, e, Mass, obj), true);

    ecs_add_pair(world, e, ecs_id(Position), obj);
    test_bool(ecs_is_enabled_pair(world, e, Position, obj), true);
    test_bool(ecs_is_enabled_pair(world, e, Velocity, obj), false);
    test_bool(ecs_is_enabled_pair(world, e, Mass, obj), true);

    ecs_fini(world);
}

void EnabledComponents_query_disabled() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_entity_t e3 = ecs_new(world, Position);

    ecs_enable_component(world, e1, Position, true);
    ecs_enable_component(world, e2, Position, false);

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t table_count = 0, count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != e2);
            test_assert(it.entities[i] == e1 || it.entities[i] == e3);
            test_assert(ecs_is_enabled_component(world, it.entities[i], Position));
        }
        count += it.count;
        table_count ++;
    }

    test_int(count, 2);
    test_int(table_count, 2);

    ecs_fini(world);
}

void EnabledComponents_query_disabled_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tgt);

    ecs_entity_t e1 = ecs_new_w_pair(world, ecs_id(Position), Tgt);
    ecs_entity_t e2 = ecs_new_w_pair(world, ecs_id(Position), Tgt);
    ecs_entity_t e3 = ecs_new_w_pair(world, ecs_id(Position), Tgt);

    ecs_enable_pair(world, e1, Position, Tgt, true);
    ecs_enable_pair(world, e2, Position, Tgt, false);

    ecs_query_t *q = ecs_query_new(world, "(Position, Tgt)");
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t table_count = 0, count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != e2);
            test_assert(it.entities[i] == e1 || it.entities[i] == e3);
            test_assert(ecs_is_enabled_pair(world, it.entities[i], Position, Tgt));
        }
        count += it.count;
        table_count ++;
    }

    test_int(count, 2);
    test_int(table_count, 2);

    ecs_fini(world);
}

void EnabledComponents_query_disabled_skip_initial() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_entity_t e3 = ecs_new(world, Position);

    ecs_enable_component(world, e1, Position, false);
    ecs_enable_component(world, e2, Position, true);

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != e1);
            test_assert(it.entities[i] == e2 || it.entities[i] == e3);
            test_assert(ecs_is_enabled_component(world, it.entities[i], Position));
        }
        count += it.count;
    }

    test_int(count, 2);

    ecs_fini(world);
}

void EnabledComponents_query_disabled_pair_skip_initial() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tgt);

    ecs_entity_t e1 = ecs_new_w_pair(world, ecs_id(Position), Tgt);
    ecs_entity_t e2 = ecs_new_w_pair(world, ecs_id(Position), Tgt);
    ecs_entity_t e3 = ecs_new_w_pair(world, ecs_id(Position), Tgt);

    ecs_enable_pair(world, e1, Position, Tgt, false);
    ecs_enable_pair(world, e2, Position, Tgt, true);

    ecs_query_t *q = ecs_query_new(world, "(Position, Tgt)");
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != e1);
            test_assert(it.entities[i] == e2 || it.entities[i] == e3);
            test_assert(ecs_is_enabled_pair(world, it.entities[i], Position, Tgt));
        }
        count += it.count;
    }

    test_int(count, 2);

    ecs_fini(world);
}

void EnabledComponents_query_mod_2() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    int32_t i, total_count = 0;
    for (i = 0; i < 64; i ++) {
        ecs_entity_t e = ecs_new(world, Position);
        if (!(e % 2)) {
            ecs_enable_component(world, e, Position, false);
        } else {
            ecs_enable_component(world, e, Position, true);
            total_count ++;
        }
    }

    test_assert(total_count != 0);

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] % 2);
            test_assert(ecs_is_enabled_component(world, it.entities[i], Position));
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_fini(world);
}

void EnabledComponents_query_mod_8() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    int32_t i, total_count = 0;
    for (i = 0; i < 65536; i ++) {
        ecs_entity_t e = ecs_new(world, Position);
        if (!(e % 8)) {
            ecs_enable_component(world, e, Position, false);
        } else {
            ecs_enable_component(world, e, Position, true);
            total_count ++;
        }
    }

    test_assert(total_count != 0);

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] % 8);
            test_assert(ecs_is_enabled_component(world, it.entities[i], Position));
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_fini(world);
}

void EnabledComponents_query_mod_64() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    int32_t i, total_count = 0;
    for (i = 0; i < 65536; i ++) {
        ecs_entity_t e = ecs_new(world, Position);
        if (!(e % 64)) {
            ecs_enable_component(world, e, Position, false);
        } else {
            ecs_enable_component(world, e, Position, true);
            total_count ++;
        }
    }

    test_assert(total_count != 0);

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] % 64);
            test_assert(ecs_is_enabled_component(world, it.entities[i], Position));
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_fini(world);
}

void EnabledComponents_query_mod_256() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    int32_t i, total_count = 0;
    for (i = 0; i < 65536; i ++) {
        ecs_entity_t e = ecs_new(world, Position);
        if (!(e % 256)) {
            ecs_enable_component(world, e, Position, false);
        } else {
            ecs_enable_component(world, e, Position, true);
            total_count ++;
        }
    }

    test_assert(total_count != 0);

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] % 256);
            test_assert(ecs_is_enabled_component(world, it.entities[i], Position));
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_fini(world);
}

void EnabledComponents_query_mod_1024() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    int32_t i, total_count = 0;
    for (i = 0; i < 100000; i ++) {
        ecs_entity_t e = ecs_new(world, Position);
        if (!(e % 1024)) {
            ecs_enable_component(world, e, Position, false);
        } else {
            ecs_enable_component(world, e, Position, true);
            total_count ++;
        }
    }

    test_assert(total_count != 0);

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] % 1024);
            test_assert(ecs_is_enabled_component(world, it.entities[i], Position));
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_fini(world);
}

void EnabledComponents_query_enable_mod_10() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    int32_t i, total_count = 0;
    for (i = 0; i < 65536; i ++) {
        ecs_entity_t e = ecs_new(world, Position);
        if (!(e % 10)) {
            ecs_enable_component(world, e, Position, true);
            total_count ++;
        } else {
            ecs_enable_component(world, e, Position, false);
        }
    }

    test_assert(total_count != 0);

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(!(it.entities[i] % 10));
            test_assert(ecs_is_enabled_component(world, it.entities[i], Position));
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_fini(world);
}

void EnabledComponents_query_mod_2_2_bitsets() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    int32_t i, total_count = 0;
    for (i = 0; i < 65536; i ++) {
        ecs_entity_t e = ecs_new(world, Position);
        ecs_add(world, e, Velocity);
        if (!(e % 2)) {
            ecs_enable_component(world, e, Position, true);
        } else {
            ecs_enable_component(world, e, Position, false);
        }
        
        if (!(e % 3)) {
            ecs_enable_component(world, e, Velocity, true);
        } else {
            ecs_enable_component(world, e, Velocity, false);
        }        

        if (!(e % 2) && !(e % 3)) {
            total_count ++;
        }
    }

    test_assert(total_count != 0);

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity");
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_is_enabled_component(world, it.entities[i], Position));
            test_assert(ecs_is_enabled_component(world, it.entities[i], Velocity));
            test_assert(!(it.entities[i] % 2) && !(it.entities[i] % 3));
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_fini(world);
}

void EnabledComponents_query_mod_8_2_bitsets() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    int32_t i, total_count = 0;
    for (i = 0; i < 65536; i ++) {
        ecs_entity_t e = ecs_new(world, Position);
        ecs_add(world, e, Velocity);
        if (!(e % 8)) {
            ecs_enable_component(world, e, Position, true);
        } else {
            ecs_enable_component(world, e, Position, false);
        }
        
        if (!(e % 4)) {
            ecs_enable_component(world, e, Velocity, true);
        } else {
            ecs_enable_component(world, e, Velocity, false);
        }        

        if (!(e % 8) && !(e % 4)) {
            total_count ++;
        }
    }

    test_assert(total_count != 0);

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity");
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_is_enabled_component(world, it.entities[i], Position));
            test_assert(ecs_is_enabled_component(world, it.entities[i], Velocity));
            test_assert(!(it.entities[i] % 8) && !(it.entities[i] % 4));
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_fini(world);
}

void EnabledComponents_query_mod_64_2_bitsets() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    int32_t i, total_count = 0;
    for (i = 0; i < 65536; i ++) {
        ecs_entity_t e = ecs_new(world, Position);
        ecs_add(world, e, Velocity);
        if (!(e % 64)) {
            ecs_enable_component(world, e, Position, true);
        } else {
            ecs_enable_component(world, e, Position, false);
        }
        
        if (!(e % 16)) {
            ecs_enable_component(world, e, Velocity, true);
        } else {
            ecs_enable_component(world, e, Velocity, false);
        }        

        if (!(e % 64) && !(e % 16)) {
            total_count ++;
        }
    }

    test_assert(total_count != 0);

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity");
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_is_enabled_component(world, it.entities[i], Position));
            test_assert(ecs_is_enabled_component(world, it.entities[i], Velocity));
            test_assert(!(it.entities[i] % 64) && !(it.entities[i] % 16));
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_fini(world);
}

void EnabledComponents_query_mod_256_2_bitsets() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    int32_t i, total_count = 0;
    for (i = 0; i < 65536; i ++) {
        ecs_entity_t e = ecs_new(world, Position);
        ecs_add(world, e, Velocity);
        if (!(e % 256)) {
            ecs_enable_component(world, e, Position, true);
        } else {
            ecs_enable_component(world, e, Position, false);
        }
        
        if (!(e % 64)) {
            ecs_enable_component(world, e, Velocity, true);
        } else {
            ecs_enable_component(world, e, Velocity, false);
        }        

        if (!(e % 256) && !(e % 64)) {
            total_count ++;
        }
    }

    test_assert(total_count != 0);

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity");
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_is_enabled_component(world, it.entities[i], Position));
            test_assert(ecs_is_enabled_component(world, it.entities[i], Velocity));
            test_assert(!(it.entities[i] % 256) && !(it.entities[i] % 64));
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_fini(world);
}

void EnabledComponents_query_mod_1024_2_bitsets() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    int32_t i, total_count = 0;
    for (i = 0; i < 65536; i ++) {
        ecs_entity_t e = ecs_new(world, Position);
        ecs_add(world, e, Velocity);
        if (!(e % 1024)) {
            ecs_enable_component(world, e, Position, true);
        } else {
            ecs_enable_component(world, e, Position, false);
        }
        
        if (!(e % 128)) {
            ecs_enable_component(world, e, Velocity, true);
        } else {
            ecs_enable_component(world, e, Velocity, false);
        }        

        if (!(e % 1024) && !(e % 128)) {
            total_count ++;
        }
    }

    test_assert(total_count != 0);

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity");
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_is_enabled_component(world, it.entities[i], Position));
            test_assert(ecs_is_enabled_component(world, it.entities[i], Velocity));
            test_assert(!(it.entities[i] % 1024) && !(it.entities[i] % 128));
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_fini(world);
}

void EnabledComponents_query_randomized_2_bitsets() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    int32_t i, total_count = 0;
    for (i = 0; i < 65536; i ++) {
        ecs_entity_t e = ecs_new(world, Position);
        ecs_add(world, e, Velocity);

        bool enable_1 = rand() % 2;
        ecs_enable_component(world, e, Position, enable_1);
        bool enable_2 = rand() % 2;
        ecs_enable_component(world, e, Velocity, enable_2);
        
        if (enable_1 && enable_2) {
            total_count ++;
        }
    }

    test_assert(total_count != 0);

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity");
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_is_enabled_component(world, it.entities[i], Position));
            test_assert(ecs_is_enabled_component(world, it.entities[i], Velocity));
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_fini(world);
}

void EnabledComponents_query_randomized_3_bitsets() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    int32_t i, total_count = 0;
    for (i = 0; i < 65536; i ++) {
        ecs_entity_t e = ecs_new(world, Position);
        ecs_add(world, e, Velocity);
        ecs_add(world, e, Mass);

        bool enable_1 = rand() % 2;
        ecs_enable_component(world, e, Position, enable_1);
        bool enable_2 = rand() % 2;
        ecs_enable_component(world, e, Velocity, enable_2);
        bool enable_3 = rand() % 2;
        ecs_enable_component(world, e, Mass, enable_3);        

        if (enable_1 && enable_2 && enable_3) {
            total_count ++;
        }
    }

    test_assert(total_count != 0);

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity, Mass");
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_is_enabled_component(world, it.entities[i], Position));
            test_assert(ecs_is_enabled_component(world, it.entities[i], Velocity));
            test_assert(ecs_is_enabled_component(world, it.entities[i], Mass));
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_fini(world);
}

void EnabledComponents_query_randomized_4_bitsets() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    int32_t i, total_count = 0;
    for (i = 0; i < 65536; i ++) {
        ecs_entity_t e = ecs_new(world, Position);
        ecs_add(world, e, Velocity);
        ecs_add(world, e, Mass);
        ecs_add(world, e, Rotation);

        bool enable_1 = rand() % 2;
        ecs_enable_component(world, e, Position, enable_1);
        bool enable_2 = rand() % 2;
        ecs_enable_component(world, e, Velocity, enable_2);
        bool enable_3 = rand() % 2;
        ecs_enable_component(world, e, Mass, enable_3);        
        bool enable_4 = rand() % 2;
        ecs_enable_component(world, e, Rotation, enable_4); 

        if (enable_1 && enable_2 && enable_3 && enable_4) {
            total_count ++;
        }
    }

    test_assert(total_count != 0);

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity, Mass, Rotation");
    ecs_iter_t it = ecs_query_iter(world, q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_is_enabled_component(world, it.entities[i], Position));
            test_assert(ecs_is_enabled_component(world, it.entities[i], Velocity));
            test_assert(ecs_is_enabled_component(world, it.entities[i], Mass));
            test_assert(ecs_is_enabled_component(world, it.entities[i], Rotation));
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_fini(world);
}

void EnabledComponents_defer_enable() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);
    test_bool(ecs_is_enabled_component(world, e, Position), true);

    ecs_defer_begin(world);
    ecs_enable_component(world, e, Position, false);
    test_bool(ecs_is_enabled_component(world, e, Position), true);
    ecs_defer_end(world);

    test_bool(ecs_is_enabled_component(world, e, Position), false);

    ecs_fini(world);
}

static
int compare_position(ecs_entity_t e1, const void *ptr1, ecs_entity_t e2, const void *ptr2) {
    const Position *p1 = ptr1;
    const Position *p2 = ptr2;
    return (p1->x > p2->x) - (p1->x < p2->x);
}

void EnabledComponents_sort() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {3, 2});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {2, 2});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {1, 2});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {0, 2});

    ecs_enable_component(world, e1, Position, true);
    ecs_enable_component(world, e2, Position, true);
    ecs_enable_component(world, e3, Position, false);
    ecs_enable_component(world, e4, Position, false);

    test_bool(ecs_is_enabled_component(world, e1, Position), true);
    test_bool(ecs_is_enabled_component(world, e2, Position), true);
    test_bool(ecs_is_enabled_component(world, e3, Position), false);
    test_bool(ecs_is_enabled_component(world, e4, Position), false);
    
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position",
        .order_by_component = ecs_id(Position),
        .order_by = compare_position
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 2);
    test_assert(it.entities[0] == e2);
    test_assert(it.entities[1] == e1);
    test_assert(!ecs_query_next(&it));

    /* Entities will have shuffled around, ensure bits got shuffled too */
    test_bool(ecs_is_enabled_component(world, e1, Position), true);
    test_bool(ecs_is_enabled_component(world, e2, Position), true);
    test_bool(ecs_is_enabled_component(world, e3, Position), false);
    test_bool(ecs_is_enabled_component(world, e4, Position), false);

    ecs_fini(world);
}
