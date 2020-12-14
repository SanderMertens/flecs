#include <api.h>

void EnabledComponents_is_component_enabled() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);

    test_bool(ecs_is_component_enabled(world, e, Position), true);

    ecs_fini(world);
}

void EnabledComponents_is_empty_entity_disabled() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);

    test_bool(ecs_is_component_enabled(world, e, Position), false);

    ecs_fini(world);
}

void EnabledComponents_is_0_entity_disabled() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    test_bool(ecs_is_component_enabled(world, 0, Position), false);

    ecs_fini(world);
}

void EnabledComponents_is_0_component_disabled() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, 0);

    test_bool(ecs_is_component_enabled_w_entity(world, e, 0), false);

    ecs_fini(world);
}

void EnabledComponents_is_nonexist_component_disabled() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_new(world, Velocity);

    test_bool(ecs_is_component_enabled(world, e, Position), false);

    ecs_fini(world);
}

void EnabledComponents_is_enabled_component_enabled() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_id(world);

    ecs_enable_component(world, e, Position, true);
    test_bool(ecs_is_component_enabled(world, e, Position), true);

    ecs_fini(world);
}

void EnabledComponents_is_disabled_component_enabled() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_id(world);

    ecs_enable_component(world, e, Position, true);
    test_bool(ecs_is_component_enabled(world, e, Position), true);

    ecs_fini(world);
}

void EnabledComponents_has_enabled_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_id(world);

    ecs_enable_component(world, e, Position, true);
    
    test_bool(ecs_has_entity(world, e, ECS_DISABLED | ecs_typeid(Position)), true);

    ecs_fini(world);
}

void EnabledComponents_is_enabled_after_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_id(world);

    ecs_enable_component(world, e, Position, true);
    test_bool(ecs_is_component_enabled(world, e, Position), true);

    ecs_add(world, e, Position);
    test_bool(ecs_is_component_enabled(world, e, Position), true);
    test_bool(ecs_has_entity(world, e, ECS_DISABLED | ecs_typeid(Position)), true);
    test_bool(ecs_has(world, e, Position), true);

    ecs_fini(world);
}

void EnabledComponents_is_enabled_after_remove() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new(world, Position);

    ecs_enable_component(world, e, Position, true);
    test_bool(ecs_is_component_enabled(world, e, Position), true);

    ecs_remove(world, e, Position);
    test_bool(ecs_is_component_enabled(world, e, Position), true);
    test_bool(ecs_has_entity(world, e, ECS_DISABLED | ecs_typeid(Position)), true);
    test_bool(ecs_has(world, e, Position), false);

    ecs_fini(world);
}

void EnabledComponents_is_enabled_after_disable() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_id(world);

    ecs_enable_component(world, e, Position, true);
    test_bool(ecs_is_component_enabled(world, e, Position), true);

    ecs_enable_component(world, e, Position, false);
    test_bool(ecs_is_component_enabled(world, e, Position), false);

    ecs_enable_component(world, e, Position, true);
    test_bool(ecs_is_component_enabled(world, e, Position), true);

    ecs_fini(world);
}

void EnabledComponents_is_disabled_after_enable() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_id(world);

    ecs_enable_component(world, e, Position, false);
    test_bool(ecs_is_component_enabled(world, e, Position), false);

    ecs_enable_component(world, e, Position, true);
    test_bool(ecs_is_component_enabled(world, e, Position), true);

    ecs_enable_component(world, e, Position, false);
    test_bool(ecs_is_component_enabled(world, e, Position), false);

    ecs_fini(world);
}

void EnabledComponents_is_enabled_randomized() {
    ecs_world_t *world = ecs_init();

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
            ecs_is_component_enabled(world, entities[i], Position), enabled[i]);
    }

    ecs_fini(world);
}

void EnabledComponents_is_enabled_after_add_randomized() {
    ecs_world_t *world = ecs_init();

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
            ecs_is_component_enabled(world, entities[i], Position), enabled[i]);
    }

    ecs_fini(world);
}

void EnabledComponents_is_enabled_after_randomized_add_randomized() {
    ecs_world_t *world = ecs_init();

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
            ecs_is_component_enabled(world, entities[i], Position), enabled[i]);
    }

    ecs_fini(world);
}

void EnabledComponents_query_disabled() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_entity_t e3 = ecs_new(world, Position);

    ecs_enable_component(world, e1, Position, true);
    ecs_enable_component(world, e2, Position, false);

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(q);

    int32_t table_count = 0, count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != e2);
            test_assert(it.entities[i] == e1 || it.entities[i] == e3);
        }
        count += it.count;
        table_count ++;
    }

    test_int(count, 2);
    test_int(table_count, 2);

    ecs_fini(world);
}

void EnabledComponents_query_disabled_skip_initial() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_entity_t e3 = ecs_new(world, Position);

    ecs_enable_component(world, e1, Position, false);
    ecs_enable_component(world, e2, Position, true);

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] != e1);
            test_assert(it.entities[i] == e2 || it.entities[i] == e3);
        }
        count += it.count;
    }

    test_int(count, 2);

    ecs_fini(world);
}

void EnabledComponents_query_randomized_2() {
    ecs_world_t *world = ecs_init();

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

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] % 2);
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_fini(world);
}

void EnabledComponents_query_randomized_8() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    int32_t i, total_count = 0;
    for (i = 0; i < 64; i ++) {
        ecs_entity_t e = ecs_new(world, Position);
        if (!(e % 8)) {
            ecs_enable_component(world, e, Position, false);
        } else {
            ecs_enable_component(world, e, Position, true);
            total_count ++;
        }
    }

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] % 8);
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_fini(world);
}

void EnabledComponents_query_randomized_64() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    int32_t i, total_count = 0;
    for (i = 0; i < 1000; i ++) {
        ecs_entity_t e = ecs_new(world, Position);
        if (!(e % 64)) {
            ecs_enable_component(world, e, Position, false);
        } else {
            ecs_enable_component(world, e, Position, true);
            total_count ++;
        }
    }

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] % 64);
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_fini(world);
}

void EnabledComponents_query_randomized_256() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    int32_t i, total_count = 0;
    for (i = 0; i < 10000; i ++) {
        ecs_entity_t e = ecs_new(world, Position);
        if (!(e % 256)) {
            ecs_enable_component(world, e, Position, false);
        } else {
            ecs_enable_component(world, e, Position, true);
            total_count ++;
        }
    }

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] % 256);
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_fini(world);
}

void EnabledComponents_query_randomized_1024() {
    ecs_world_t *world = ecs_init();

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

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_iter_t it = ecs_query_iter(q);

    int32_t count = 0;

    while (ecs_query_next(&it)) {
        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] % 1024);
        }
        count += it.count;
    }

    test_int(count, total_count);

    ecs_fini(world);
}
