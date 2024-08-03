#include <core.h>

void Iter_page_iter_0_0(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new(world); ecs_set(world, e5, Self, {e5});

    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagB);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Self) }}
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_t pit = ecs_page_iter(&it, 0, 0);

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 2);
        test_int(pit.frame_offset, 0);
        test_int(pit.entities[0], e1);
        test_int(pit.entities[1], e2);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e1);
        test_int(ptr[1].value, e2);
    }

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 2);
        test_int(pit.frame_offset, 2);
        test_int(pit.entities[0], e3);
        test_int(pit.entities[1], e4);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e3);
        test_int(ptr[1].value, e4);
    }

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 1);
        test_int(pit.frame_offset, 4);
        test_int(pit.entities[0], e5);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e5);
    }

    test_bool(ecs_page_next(&pit), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Iter_page_iter_1_0(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new(world); ecs_set(world, e5, Self, {e5});

    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagB);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Self) }}
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_t pit = ecs_page_iter(&it, 1, 0);

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 1);
        test_int(pit.entities[0], e2);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e2);
    }

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 2);
        test_int(pit.entities[0], e3);
        test_int(pit.entities[1], e4);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e3);
        test_int(ptr[1].value, e4);
    }

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 1);
        test_int(pit.entities[0], e5);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e5);
    }

    test_bool(ecs_page_next(&pit), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Iter_page_iter_0_1(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new(world); ecs_set(world, e5, Self, {e5});

    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagB);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Self) }}
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_t pit = ecs_page_iter(&it, 0, 1);

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 1);
        test_int(pit.entities[0], e1);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e1);
    }

    test_bool(ecs_page_next(&pit), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Iter_page_iter_n_0(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new(world); ecs_set(world, e5, Self, {e5});

    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagB);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Self) }}
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_t pit = ecs_page_iter(&it, 2, 0);

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 1);
        test_int(pit.entities[0], e3);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e3);
    }

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 1);
        test_int(pit.entities[0], e4);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e4);
    }

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 1);
        test_int(pit.entities[0], e5);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e5);
    }

    test_bool(ecs_page_next(&pit), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Iter_page_iter_0_n(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new(world); ecs_set(world, e5, Self, {e5});

    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagB);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Self) }}
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_t pit = ecs_page_iter(&it, 0, 2);

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 2);
        test_int(pit.entities[0], e1);
        test_int(pit.entities[1], e2);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e1);
        test_int(ptr[1].value, e2);
    }

    test_bool(ecs_page_next(&pit), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Iter_page_iter_m_n(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new(world); ecs_set(world, e5, Self, {e5});
    ecs_entity_t e6 = ecs_new(world); ecs_set(world, e6, Self, {e6});

    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagA);
    ecs_add(world, e6, TagB);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Self) }}
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_t pit = ecs_page_iter(&it, 2, 3);

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 1);
        test_int(pit.entities[0], e3);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e3);
    }
    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 2);
        test_int(pit.entities[0], e4);
        test_int(pit.entities[1], e5);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e4);
        test_int(ptr[1].value, e5);
    }

    test_bool(ecs_page_next(&pit), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Iter_page_iter_skip_1_table(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new(world); ecs_set(world, e5, Self, {e5});

    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagB);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Self) }}
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_t pit = ecs_page_iter(&it, 2, 0);

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 2);
        test_int(pit.entities[0], e3);
        test_int(pit.entities[1], e4);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e3);
        test_int(ptr[1].value, e4);
    }

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 1);
        test_int(pit.entities[0], e5);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e5);
    }

    test_bool(ecs_page_next(&pit), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Iter_page_iter_skip_2_tables(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new(world); ecs_set(world, e5, Self, {e5});

    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagB);
    ecs_add(world, e5, TagB);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Self) }}
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_t pit = ecs_page_iter(&it, 3, 0);

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 2);
        test_int(pit.entities[0], e4);
        test_int(pit.entities[1], e5);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e4);
        test_int(ptr[1].value, e5);
    }

    test_bool(ecs_page_next(&pit), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Iter_worker_iter_1(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new(world); ecs_set(world, e5, Self, {e5});

    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagB);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Self) }}
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_t pit = ecs_worker_iter(&it, 0, 1);

    {
        test_bool(ecs_worker_next(&pit), true);
        test_int(pit.count, 2);
        test_int(pit.entities[0], e1);
        test_int(pit.entities[1], e2);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e1);
        test_int(ptr[1].value, e2);
    }

    {
        test_bool(ecs_worker_next(&pit), true);
        test_int(pit.count, 2);
        test_int(pit.entities[0], e3);
        test_int(pit.entities[1], e4);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e3);
        test_int(ptr[1].value, e4);
    }

    {
        test_bool(ecs_worker_next(&pit), true);
        test_int(pit.count, 1);
        test_int(pit.entities[0], e5);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e5);
    }

    test_bool(ecs_worker_next(&pit), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Iter_worker_iter_2(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new(world); ecs_set(world, e5, Self, {e5});

    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagB);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Self) }}
    });

    ecs_iter_t it_1 = ecs_query_iter(world, q);
    ecs_iter_t pit_1 = ecs_worker_iter(&it_1, 0, 2);
    ecs_iter_t it_2 = ecs_query_iter(world, q);
    ecs_iter_t pit_2 = ecs_worker_iter(&it_2, 1, 2);

    /* Iter 1 */
    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 1);
        test_int(pit_1.entities[0], e1);
        test_int(pit_1.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit_1, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e1);
    }

    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 1);
        test_int(pit_1.entities[0], e3);
        test_int(pit_1.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit_1, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e3);
    }

    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 1);
        test_int(pit_1.entities[0], e5);
        test_int(pit_1.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit_1, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e5);
    }

    test_bool(ecs_worker_next(&pit_1), false);

    /* Iter 2 */
    {
        test_bool(ecs_worker_next(&pit_2), true);
        test_int(pit_2.count, 1);
        test_int(pit_2.entities[0], e2);
        test_int(pit_2.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit_2, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e2);
    }

    {
        test_bool(ecs_worker_next(&pit_2), true);
        test_int(pit_2.count, 1);
        test_int(pit_2.entities[0], e4);
        test_int(pit_2.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit_2, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e4);
    }

    test_bool(ecs_worker_next(&pit_2), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Iter_worker_iter_3(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new(world); ecs_set(world, e5, Self, {e5});
    ecs_entity_t e6 = ecs_new(world); ecs_set(world, e6, Self, {e6});

    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagA);
    ecs_add(world, e6, TagB);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Self) }}
    });

    ecs_iter_t it_1 = ecs_query_iter(world, q);
    ecs_iter_t pit_1 = ecs_worker_iter(&it_1, 0, 3);
    ecs_iter_t it_2 = ecs_query_iter(world, q);
    ecs_iter_t pit_2 = ecs_worker_iter(&it_2, 1, 3);
    ecs_iter_t it_3 = ecs_query_iter(world, q);
    ecs_iter_t pit_3 = ecs_worker_iter(&it_3, 2, 3);

    /* Iter 1 */
    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 1);
        test_int(pit_1.entities[0], e1);
        test_int(pit_1.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit_1, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e1);
    }

    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 1);
        test_int(pit_1.entities[0], e4);
        test_int(pit_1.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit_1, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e4);
    }

    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 1);
        test_int(pit_1.entities[0], e6);
        test_int(pit_1.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit_1, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e6);
    }

    test_bool(ecs_worker_next(&pit_1), false);

    /* Iter 2 */
    {
        test_bool(ecs_worker_next(&pit_2), true);
        test_int(pit_2.count, 1);
        test_int(pit_2.entities[0], e2);
        test_int(pit_2.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit_2, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e2);
    }

    {
        test_bool(ecs_worker_next(&pit_2), true);
        test_int(pit_2.count, 1);
        test_int(pit_2.entities[0], e5);
        test_int(pit_2.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit_2, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e5);
    }

    test_bool(ecs_worker_next(&pit_2), false);

    /* Iter 3 */
    {
        test_bool(ecs_worker_next(&pit_3), true);
        test_int(pit_3.count, 1);
        test_int(pit_3.entities[0], e3);
        test_int(pit_3.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit_3, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e3);
    }

    test_bool(ecs_worker_next(&pit_3), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Iter_worker_iter_4(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new(world); ecs_set(world, e5, Self, {e5});
    ecs_entity_t e6 = ecs_new(world); ecs_set(world, e6, Self, {e6});
    ecs_entity_t e7 = ecs_new(world); ecs_set(world, e7, Self, {e7});
    ecs_entity_t e8 = ecs_new(world); ecs_set(world, e8, Self, {e8});
    ecs_entity_t e9 = ecs_new(world); ecs_set(world, e9, Self, {e9});

    ecs_add(world, e5, TagA);
    ecs_add(world, e6, TagA);
    ecs_add(world, e7, TagA);
    ecs_add(world, e8, TagB);
    ecs_add(world, e9, TagB);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Self) }}
    });

    ecs_iter_t it_1 = ecs_query_iter(world, q);
    ecs_iter_t pit_1 = ecs_worker_iter(&it_1, 0, 4);
    ecs_iter_t it_2 = ecs_query_iter(world, q);
    ecs_iter_t pit_2 = ecs_worker_iter(&it_2, 1, 4);
    ecs_iter_t it_3 = ecs_query_iter(world, q);
    ecs_iter_t pit_3 = ecs_worker_iter(&it_3, 2, 4);
    ecs_iter_t it_4 = ecs_query_iter(world, q);
    ecs_iter_t pit_4 = ecs_worker_iter(&it_4, 3, 4);

    /* Iter 1 */
    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 1);
        test_int(pit_1.entities[0], e1);
        test_int(pit_1.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit_1, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e1);
    }

    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 1);
        test_int(pit_1.entities[0], e5);
        test_int(pit_1.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit_1, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e5);
    }

    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 1);
        test_int(pit_1.entities[0], e8);
        test_int(pit_1.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit_1, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e8);
    }

    test_bool(ecs_worker_next(&pit_1), false);

    /* Iter 2 */
    {
        test_bool(ecs_worker_next(&pit_2), true);
        test_int(pit_2.count, 1);
        test_int(pit_2.entities[0], e2);
        test_int(pit_2.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit_2, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e2);
    }

    {
        test_bool(ecs_worker_next(&pit_2), true);
        test_int(pit_2.count, 1);
        test_int(pit_2.entities[0], e6);
        test_int(pit_2.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit_2, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e6);
    }

    {
        test_bool(ecs_worker_next(&pit_2), true);
        test_int(pit_2.count, 1);
        test_int(pit_2.entities[0], e9);
        test_int(pit_2.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit_2, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e9);
    }

    test_bool(ecs_worker_next(&pit_2), false);

    /* Iter 3 */
    {
        test_bool(ecs_worker_next(&pit_3), true);
        test_int(pit_3.count, 1);
        test_int(pit_3.entities[0], e3);
        test_int(pit_3.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit_3, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e3);
    }

    {
        test_bool(ecs_worker_next(&pit_3), true);
        test_int(pit_3.count, 1);
        test_int(pit_3.entities[0], e7);
        test_int(pit_3.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit_3, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e7);
    }

    test_bool(ecs_worker_next(&pit_3), false);

    /* Iter 4 */
    {
        test_bool(ecs_worker_next(&pit_4), true);
        test_int(pit_4.count, 1);
        test_int(pit_4.entities[0], e4);
        test_int(pit_4.ids[0], ecs_id(Self));

        Self *ptr = ecs_field(&pit_4, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e4);
    }

    test_bool(ecs_worker_next(&pit_4), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Iter_paged_iter_w_shared_comp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t foo = ecs_insert(world, ecs_value(Position, {10, 20}));

    ecs_entity_t e1 = ecs_new(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new(world); ecs_set(world, e5, Self, {e5});

    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagB);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Self) }, { ecs_id(Position), .src.id = foo} }
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_t pit = ecs_page_iter(&it, 0, 0);

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 2);
        test_int(pit.entities[0], e1);
        test_int(pit.entities[1], e2);
        test_int(pit.ids[0], ecs_id(Self));
        test_int(pit.ids[1], ecs_id(Position));
        test_int(pit.sources[0], 0);
        test_int(pit.sources[1], foo);

        Self *ptr = ecs_field(&pit, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e1);
        test_int(ptr[1].value, e2);

        Position *pptr = ecs_field(&pit, Position, 1);
        test_int(pptr->x, 10);
        test_int(pptr->y, 20);
    }

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 2);
        test_int(pit.entities[0], e3);
        test_int(pit.entities[1], e4);
        test_int(pit.ids[0], ecs_id(Self));
        test_int(pit.ids[1], ecs_id(Position));
        test_int(pit.sources[0], 0);
        test_int(pit.sources[1], foo);

        Self *ptr = ecs_field(&pit, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e3);
        test_int(ptr[1].value, e4);

        Position *pptr = ecs_field(&pit, Position, 1);
        test_int(pptr->x, 10);
        test_int(pptr->y, 20);
    }

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 1);
        test_int(pit.entities[0], e5);
        test_int(pit.ids[0], ecs_id(Self));
        test_int(pit.ids[1], ecs_id(Position));
        test_int(pit.sources[0], 0);
        test_int(pit.sources[1], foo);

        Self *ptr = ecs_field(&pit, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e5);

        Position *pptr = ecs_field(&pit, Position, 1);
        test_int(pptr->x, 10);
        test_int(pptr->y, 20);
    }

    test_bool(ecs_page_next(&pit), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Iter_worker_iter_w_shared_comp(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t foo = ecs_insert(world, ecs_value(Position, {10, 20}));

    ecs_entity_t e1 = ecs_new(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new(world); ecs_set(world, e5, Self, {e5});

    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagB);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Self) }, { ecs_id(Position), .src.id = foo} }
    });

    ecs_iter_t it_1 = ecs_query_iter(world, q);
    ecs_iter_t pit_1 = ecs_worker_iter(&it_1, 0, 2);
    ecs_iter_t it_2 = ecs_query_iter(world, q);
    ecs_iter_t pit_2 = ecs_worker_iter(&it_2, 1, 2);

    /* Iter 1 */
    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 1);
        test_int(pit_1.entities[0], e1);
        test_int(pit_1.ids[0], ecs_id(Self));
        test_int(pit_1.ids[1], ecs_id(Position));
        test_int(pit_1.sources[0], 0);
        test_int(pit_1.sources[1], foo);

        Self *ptr = ecs_field(&pit_1, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e1);

        Position *pptr = ecs_field(&pit_1, Position, 1);
        test_int(pptr->x, 10);
        test_int(pptr->y, 20);
    }

    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 1);
        test_int(pit_1.entities[0], e3);
        test_int(pit_1.ids[0], ecs_id(Self));
        test_int(pit_1.ids[1], ecs_id(Position));
        test_int(pit_1.sources[0], 0);
        test_int(pit_1.sources[1], foo);

        Self *ptr = ecs_field(&pit_1, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e3);

        Position *pptr = ecs_field(&pit_1, Position, 1);
        test_int(pptr->x, 10);
        test_int(pptr->y, 20);
    }

    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 1);
        test_int(pit_1.entities[0], e5);
        test_int(pit_1.ids[0], ecs_id(Self));
        test_int(pit_1.ids[1], ecs_id(Position));
        test_int(pit_1.sources[0], 0);
        test_int(pit_1.sources[1], foo);

        Self *ptr = ecs_field(&pit_1, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e5);

        Position *pptr = ecs_field(&pit_1, Position, 1);
        test_int(pptr->x, 10);
        test_int(pptr->y, 20);
    }

    test_bool(ecs_worker_next(&pit_1), false);

    /* Iter 2 */
    {
        test_bool(ecs_worker_next(&pit_2), true);
        test_int(pit_2.count, 1);
        test_int(pit_2.entities[0], e2);
        test_int(pit_2.ids[0], ecs_id(Self));
        test_int(pit_2.ids[1], ecs_id(Position));
        test_int(pit_2.sources[0], 0);
        test_int(pit_2.sources[1], foo);

        Self *ptr = ecs_field(&pit_2, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e2);

        Position *pptr = ecs_field(&pit_2, Position, 1);
        test_int(pptr->x, 10);
        test_int(pptr->y, 20);
    }

    {
        test_bool(ecs_worker_next(&pit_2), true);
        test_int(pit_2.count, 1);
        test_int(pit_2.entities[0], e4);
        test_int(pit_2.ids[0], ecs_id(Self));
        test_int(pit_2.ids[1], ecs_id(Position));
        test_int(pit_2.sources[0], 0);
        test_int(pit_2.sources[1], foo);

        Self *ptr = ecs_field(&pit_2, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e4);

        Position *pptr = ecs_field(&pit_2, Position, 1);
        test_int(pptr->x, 10);
        test_int(pptr->y, 20);
    }

    test_bool(ecs_worker_next(&pit_2), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Iter_paged_iter_w_task_query(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);

    ecs_entity_t foo = ecs_new(world); ecs_set(world, foo, Self, {foo});

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Self), .src.id = foo }}
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_t pit = ecs_page_iter(&it, 0, 0);

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 0);
        test_int(pit.ids[0], ecs_id(Self));
        test_int(pit.sources[0], foo);

        Self *ptr = ecs_field(&pit, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, foo);
    }

    test_bool(ecs_page_next(&pit), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Iter_worker_iter_w_task_query(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);

    ecs_entity_t foo = ecs_new(world); ecs_set(world, foo, Self, {foo});

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_id(Self), .src.id = foo }}
    });

    ecs_iter_t it_1 = ecs_query_iter(world, q);
    ecs_iter_t pit_1 = ecs_worker_iter(&it_1, 0, 2);
    ecs_iter_t it_2 = ecs_query_iter(world, q);
    ecs_iter_t pit_2 = ecs_worker_iter(&it_2, 1, 2);

    /* Iter 1 */
    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 0);
        test_int(pit_1.ids[0], ecs_id(Self));
        test_int(pit_1.sources[0], foo);

        Self *ptr = ecs_field(&pit_1, Self, 0);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, foo);
    }

    test_bool(ecs_worker_next(&pit_1), false);

    /* Iter 2 */

    test_bool(ecs_worker_next(&pit_2), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Iter_worker_iter_w_singleton(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Singleton);
    ECS_COMPONENT(world, Position);

    ecs_singleton_add(world, Singleton);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Position, {40, 50}));

    ecs_query_t *q = ecs_query(world, { .expr = "Position, Singleton($)" });

    Position *p;

    ecs_iter_t it_1 = ecs_query_iter(world, q);
    ecs_iter_t wit_1 = ecs_worker_iter(&it_1, 0, 2);
    test_bool(ecs_worker_next(&wit_1), true);
    test_int(wit_1.count, 2);
    test_int(wit_1.entities[0], e1);
    test_int(wit_1.entities[1], e2);
    p = ecs_field(&wit_1, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);

    ecs_iter_t it_2 = ecs_query_iter(world, q);
    ecs_iter_t wit_2 = ecs_worker_iter(&it_2, 1, 2);
    test_bool(ecs_worker_next(&wit_2), true);
    test_int(wit_2.count, 2);
    test_int(wit_2.entities[0], e3);
    test_int(wit_2.entities[1], e4);
    p = ecs_field(&wit_2, Position, 0);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);
    test_int(p[1].x, 40);
    test_int(p[1].y, 50);

    test_bool(ecs_worker_next(&wit_2), false);
    test_bool(ecs_worker_next(&wit_1), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Iter_worker_iter_w_singleton_instanced(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Singleton);
    ECS_COMPONENT(world, Position);

    ecs_singleton_add(world, Singleton);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Position, {40, 50}));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Singleton($)"
    });

    Position *p;

    ecs_iter_t it_1 = ecs_query_iter(world, q);
    ecs_iter_t wit_1 = ecs_worker_iter(&it_1, 0, 2);
    test_bool(ecs_worker_next(&wit_1), true);
    test_int(wit_1.count, 2);
    test_int(wit_1.entities[0], e1);
    test_int(wit_1.entities[1], e2);
    p = ecs_field(&wit_1, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);
    test_bool(ecs_worker_next(&wit_1), false);

    ecs_iter_t it_2 = ecs_query_iter(world, q);
    ecs_iter_t wit_2 = ecs_worker_iter(&it_2, 1, 2);
    test_bool(ecs_worker_next(&wit_2), true);
    test_int(wit_2.count, 2);
    test_int(wit_2.entities[0], e3);
    test_int(wit_2.entities[1], e4);
    p = ecs_field(&wit_2, Position, 0);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);
    test_int(p[1].x, 40);
    test_int(p[1].y, 50);
    test_bool(ecs_worker_next(&wit_2), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Iter_worker_iter_w_singleton_component_instanced(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_singleton_set(world, Velocity, {1, 2});
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Position, {40, 50}));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Velocity($)"
    });

    Position *p;
    Velocity *v;

    ecs_iter_t it_1 = ecs_query_iter(world, q);
    ecs_iter_t wit_1 = ecs_worker_iter(&it_1, 0, 2);
    test_bool(ecs_worker_next(&wit_1), true);
    test_int(wit_1.count, 2);
    test_int(wit_1.entities[0], e1);
    test_int(wit_1.entities[1], e2);
    p = ecs_field(&wit_1, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);
    v = ecs_field(&wit_1, Velocity, 1);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_bool(ecs_worker_next(&wit_1), false);

    ecs_iter_t it_2 = ecs_query_iter(world, q);
    ecs_iter_t wit_2 = ecs_worker_iter(&it_2, 1, 2);
    test_bool(ecs_worker_next(&wit_2), true);
    test_int(wit_2.count, 2);
    test_int(wit_2.entities[0], e3);
    test_int(wit_2.entities[1], e4);
    p = ecs_field(&wit_2, Position, 0);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);
    test_int(p[1].x, 40);
    test_int(p[1].y, 50);
    v = ecs_field(&wit_2, Velocity, 1);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_bool(ecs_worker_next(&wit_2), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Iter_paged_iter_w_singleton(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Singleton);
    ECS_COMPONENT(world, Position);

    ecs_singleton_add(world, Singleton);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Position, {40, 50}));

    ecs_query_t *q = ecs_query(world, { .expr = "Position, Singleton($)" });

    Position *p;

    ecs_iter_t it_1 = ecs_query_iter(world, q);
    ecs_iter_t pit_1 = ecs_page_iter(&it_1, 0, 2);
    test_bool(ecs_page_next(&pit_1), true);
    test_int(pit_1.count, 2);
    test_int(pit_1.entities[0], e1);
    test_int(pit_1.entities[1], e2);
    p = ecs_field(&pit_1, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);
    test_bool(ecs_page_next(&pit_1), false);

    ecs_iter_t it_2 = ecs_query_iter(world, q);
    ecs_iter_t pit_2 = ecs_page_iter(&it_2, 2, 2);
    test_bool(ecs_page_next(&pit_2), true);
    test_int(pit_2.count, 2);
    test_int(pit_2.entities[0], e3);
    test_int(pit_2.entities[1], e4);
    p = ecs_field(&pit_2, Position, 0);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);
    test_int(p[1].x, 40);
    test_int(p[1].y, 50);
    test_bool(ecs_page_next(&pit_2), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Iter_paged_iter_w_singleton_instanced(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Singleton);
    ECS_COMPONENT(world, Position);

    ecs_singleton_add(world, Singleton);
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Position, {40, 50}));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Singleton($)"
    });

    Position *p;

    ecs_iter_t it_1 = ecs_query_iter(world, q);
    ecs_iter_t pit_1 = ecs_page_iter(&it_1, 0, 2);
    test_bool(ecs_page_next(&pit_1), true);
    test_int(pit_1.count, 2);
    test_int(pit_1.entities[0], e1);
    test_int(pit_1.entities[1], e2);
    p = ecs_field(&pit_1, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);
    test_bool(ecs_page_next(&pit_1), false);

    ecs_iter_t it_2 = ecs_query_iter(world, q);
    ecs_iter_t pit_2 = ecs_page_iter(&it_2, 2, 2);
    test_bool(ecs_page_next(&pit_2), true);
    test_int(pit_2.count, 2);
    test_int(pit_2.entities[0], e3);
    test_int(pit_2.entities[1], e4);
    p = ecs_field(&pit_2, Position, 0);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);
    test_int(p[1].x, 40);
    test_int(p[1].y, 50);
    test_bool(ecs_page_next(&pit_2), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

void Iter_paged_iter_w_singleton_component_instanced(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_singleton_set(world, Velocity, {1, 2});
    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_entity_t e3 = ecs_insert(world, ecs_value(Position, {30, 40}));
    ecs_entity_t e4 = ecs_insert(world, ecs_value(Position, {40, 50}));

    ecs_query_t *q = ecs_query(world, {
        .expr = "Position, Velocity($)"
    });

    Position *p;
    Velocity *v;

    ecs_iter_t it_1 = ecs_query_iter(world, q);
    ecs_iter_t pit_1 = ecs_page_iter(&it_1, 0, 2);
    test_bool(ecs_page_next(&pit_1), true);
    test_int(pit_1.count, 2);
    test_int(pit_1.entities[0], e1);
    test_int(pit_1.entities[1], e2);
    p = ecs_field(&pit_1, Position, 0);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);
    v = ecs_field(&pit_1, Velocity, 1);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_bool(ecs_page_next(&pit_1), false);

    ecs_iter_t it_2 = ecs_query_iter(world, q);
    ecs_iter_t pit_2 = ecs_page_iter(&it_2, 2, 2);
    test_bool(ecs_page_next(&pit_2), true);
    test_int(pit_2.count, 2);
    test_int(pit_2.entities[0], e3);
    test_int(pit_2.entities[1], e4);
    p = ecs_field(&pit_2, Position, 0);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);
    test_int(p[1].x, 40);
    test_int(p[1].y, 50);
    v = ecs_field(&pit_2, Velocity, 1);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_bool(ecs_page_next(&pit_2), false);

    ecs_query_fini(q);

    ecs_fini(world);
}

static
void Iter(ecs_world_t *world, ecs_query_t *q, int32_t offset, int32_t limit) {
    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_t pit = ecs_page_iter(&it, offset, limit);
    while (ecs_page_next(&pit)) {
        Position *p = ecs_field(&pit, Position, 0);
        Velocity *v = NULL;
        Mass *m = NULL;

        if (pit.field_count >= 2) {
            v = ecs_field(&pit, Velocity, 1);
        }

        if (pit.field_count >= 3) {
            m = ecs_field(&pit, Mass, 2);
        }

        int *param = pit.param;

        probe_iter(&pit);

        int i;
        for (i = 0; i < pit.count; i ++) {
            p[i].x = 10;
            p[i].y = 20;

            if (param) {
                p[i].x += *param;
                p[i].y += *param;
            }

            if (v) {
                v[i].x = 30;
                v[i].y = 40;
            }

            if (m) {
                m[i] = 50;
            }
        }
    }
}

void Iter_page_iter_w_offset_skip_1_archetype(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_QUERY(world, q, Position, Velocity);

    Probe ctx = {0};
    ecs_set_ctx(world, &ctx, NULL);

    Iter(world, q, 3, 0);

    test_int(ctx.count, 3);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e4);
    test_int(ctx.e[1], e5);
    test_int(ctx.e[2], e6);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_id(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_id(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        const Position *p = ecs_get(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_get(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Iter_page_iter_w_offset_skip_1_archetype_plus_one(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_QUERY(world, q, Position, Velocity);

    Probe ctx = {0};
    ecs_set_ctx(world, &ctx, NULL);

    Iter(world, q, 4, 0);

    test_int(ctx.count, 2);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e5);
    test_int(ctx.e[1], e6);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_id(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_id(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        const Position *p = ecs_get(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_get(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Iter_page_iter_w_offset_skip_2_archetypes(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_QUERY(world, q, Position, Velocity);

    Probe ctx = {0};
    ecs_set_ctx(world, &ctx, NULL);

    Iter(world, q, 5, 0);

    test_int(ctx.count, 1);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e6);

    test_int(ctx.c[0][0], ecs_id(Position));
    test_int(ctx.s[0][0], 0);
    test_int(ctx.c[0][1], ecs_id(Velocity));
    test_int(ctx.s[0][1], 0);

    const Position *p = ecs_get(world, e6, Position);
    test_int(p->x, 10);
    test_int(p->y, 20);
    const Velocity *v = ecs_get(world, e6, Velocity);
    test_int(v->x, 30);
    test_int(v->y, 40);        

    ecs_fini(world);
}

void Iter_page_iter_w_limit_skip_1_archetype(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_QUERY(world, q, Position, Velocity);

    Probe ctx = {0};
    ecs_set_ctx(world, &ctx, NULL);

    Iter(world, q, 0, 5);

    test_int(ctx.count, 5);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);
    test_int(ctx.e[3], e4);
    test_int(ctx.e[4], e5);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_id(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_id(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        const Position *p = ecs_get(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_get(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Iter_page_iter_w_limit_skip_1_archetype_minus_one(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_QUERY(world, q, Position, Velocity);

    Probe ctx = {0};
    ecs_set_ctx(world, &ctx, NULL);

    Iter(world, q, 0, 4);

    test_int(ctx.count, 4);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);
    test_int(ctx.e[3], e4);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_id(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_id(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        const Position *p = ecs_get(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_get(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Iter_page_iter_w_limit_skip_2_archetypes(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_QUERY(world, q, Position, Velocity);

    Probe ctx = {0};
    ecs_set_ctx(world, &ctx, NULL);

    Iter(world, q, 0, 3);

    test_int(ctx.count, 3);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e3);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_id(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_id(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        const Position *p = ecs_get(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_get(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Iter_page_iter_w_offset_1_limit_max(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_QUERY(world, q, Position, Velocity);

    Probe ctx = {0};
    ecs_set_ctx(world, &ctx, NULL);

    Iter(world, q, 1, 5);

    test_int(ctx.count, 5);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e2);
    test_int(ctx.e[1], e3);
    test_int(ctx.e[2], e4);
    test_int(ctx.e[3], e5);
    test_int(ctx.e[4], e6);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_id(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_id(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        const Position *p = ecs_get(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_get(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Iter_page_iter_w_offset_1_limit_minus_1(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_QUERY(world, q, Position, Velocity);

    Probe ctx = {0};
    ecs_set_ctx(world, &ctx, NULL);

    Iter(world, q, 1, 4);

    test_int(ctx.count, 4);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e2);
    test_int(ctx.e[1], e3);
    test_int(ctx.e[2], e4);
    test_int(ctx.e[3], e5);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_id(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_id(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        const Position *p = ecs_get(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_get(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Iter_page_iter_w_offset_2_type_limit_max(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_QUERY(world, q, Position, Velocity);

    Probe ctx = {0};
    ecs_set_ctx(world, &ctx, NULL);

    Iter(world, q, 3, 3);

    test_int(ctx.count, 3);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e4);
    test_int(ctx.e[1], e5);
    test_int(ctx.e[2], e6);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_id(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_id(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        const Position *p = ecs_get(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_get(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Iter_page_iter_w_offset_2_type_limit_minus_1(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_QUERY(world, q, Position, Velocity);

    Probe ctx = {0};
    ecs_set_ctx(world, &ctx, NULL);

    Iter(world, q, 3, 2);

    test_int(ctx.count, 2);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e4);
    test_int(ctx.e[1], e5);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_id(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_id(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        const Position *p = ecs_get(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_get(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Iter_page_iter_w_limit_1_all_offsets(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_QUERY(world, q, Position, Velocity);

    Probe ctx = {0};
    ecs_set_ctx(world, &ctx, NULL);

    Iter(world, q, 0, 1);
    Iter(world, q, 2, 1);
    Iter(world, q, 1, 1);
    Iter(world, q, 3, 1);
    Iter(world, q, 5, 1);
    Iter(world, q, 4, 1);

    test_int(ctx.count, 6);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e3);
    test_int(ctx.e[2], e2);
    test_int(ctx.e[3], e4);
    test_int(ctx.e[4], e6);
    test_int(ctx.e[5], e5);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_id(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_id(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        const Position *p = ecs_get(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_get(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Iter_page_iter_w_offset_out_of_bounds(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_QUERY(world, q, Position, Velocity);

    Probe ctx = {0};
    ecs_set_ctx(world, &ctx, NULL);

    Iter(world, q, 6, 1);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

void Iter_page_iter_w_limit_out_of_bounds(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);
    ECS_COMPONENT(world, Rotation);

    ECS_ENTITY(world, e1, Position, Velocity);
    ECS_ENTITY(world, e2, Position, Velocity);
    ECS_ENTITY(world, e3, Position, Velocity);
    ECS_ENTITY(world, e4, Position, Velocity, Mass);
    ECS_ENTITY(world, e5, Position, Velocity, Mass);
    ECS_ENTITY(world, e6, Position, Velocity, Rotation);
    ECS_ENTITY(world, e7, Position);

    ECS_QUERY(world, q, Position, Velocity);

    Probe ctx = {0};
    ecs_set_ctx(world, &ctx, NULL);

    Iter(world, q, 5, 2);

    test_int(ctx.count, 1);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e6);

    int i;
    for (i = 0; i < ctx.invoked; i ++) {
        test_int(ctx.c[i][0], ecs_id(Position));
        test_int(ctx.s[i][0], 0);
        test_int(ctx.c[i][1], ecs_id(Velocity));
        test_int(ctx.s[i][1], 0);
    }

    for (i = 0; i < ctx.count; i ++) {
        const Position *p = ecs_get(world, ctx.e[i], Position);
        test_int(p->x, 10);
        test_int(p->y, 20);
        const Velocity *v = ecs_get(world, ctx.e[i], Velocity);
        test_int(v->x, 30);
        test_int(v->y, 40);        
    }

    ecs_fini(world);
}

void Iter_page_iter_no_match(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_ENTITY(world, e1, Position);

    ECS_QUERY(world, q, Position, Velocity);

    Probe ctx = {0};
    ecs_set_ctx(world, &ctx, NULL);

    Iter(world, q, 0, 0);

    test_int(ctx.count, 0);

    ecs_fini(world);
}

typedef struct Param {
    ecs_entity_t entity;
    int count;
} Param;

static ECS_QUERY_DECLARE(qTestSubset);

static
void TestSubset(ecs_world_t *world, ecs_query_t *query, int32_t offset, 
    int32_t limit, Param *param) 
{
    ecs_iter_t it = ecs_query_iter(world, query);
    ecs_iter_t pit = ecs_page_iter(&it, offset, limit);
    while (ecs_page_next(&pit)) {
        int i;
        for (i = 0; i < pit.count; i ++) {
            test_assert(param->entity != pit.entities[i]);
            param->count ++;
        }
    }
}

static
void TestAll(ecs_world_t *world, ecs_query_t *query) {
    ecs_iter_t it = ecs_query_iter(world, query);
    while (ecs_query_next(&it)) {
        Position *p = ecs_field(&it, Position, 0);

        int i;
        for (i = 0; i < it.count; i ++) {
            Param param = {.entity = it.entities[i], 0};
            TestSubset(world, qTestSubset, it.frame_offset + i + 1, 0, &param);
            p[i].x += param.count;
        }
    }
}

void Iter_page_iter_comb_10_entities_1_type(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ECS_QUERY_DEFINE(world, qTestSubset, Position);
    ECS_QUERY(world, qTestAll, Position);

    int i, ENTITIES = 10;

    const ecs_entity_t *ids = ecs_bulk_new(world, Position, ENTITIES);

    for (i = 0; i < ENTITIES; i ++) {
        ecs_set(world, ids[i], Position, {1, 2});
    }

    TestAll(world, qTestAll);

    for (i = 0; i < ENTITIES; i ++) {
        const Position *p = ecs_get(world, ids[i], Position);
        test_int(p->x, ENTITIES - i);
    }

    ecs_fini(world);
}

void Iter_page_iter_comb_10_entities_2_types(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_QUERY_DEFINE(world, qTestSubset, Position);
    ECS_QUERY(world, qTestAll, Position);

    int i, ENTITIES = 10;
    ecs_entity_t ids_1[5], ids_2[5];

    for (int i = 0; i < ENTITIES / 2; i ++) {
        ids_1[i] = ecs_insert(world, ecs_value(Position, {1, 2}));
    }
    for (int i = 0; i < ENTITIES / 2; i ++) {
        ids_2[i] = ecs_insert(world, ecs_value(Position, {1, 2}));
        ecs_add(world, ids_2[i], Velocity);
    } 

    TestAll(world, qTestAll);

    for (i = 0; i < 5; i ++) {
        const Position *p = ecs_get(world, ids_1[i], Position);
        test_int(p->x, ENTITIES - i);

        p = ecs_get(world, ids_2[i], Position);
        test_int(p->x, ENTITIES - (i + 5));
    }

    ecs_fini(world);
}

void Iter_count(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    for (int i = 0; i < 500; i ++) {
        ecs_entity_t e = ecs_new_w(world, TagA);
        if (!(i % 2)) {
            ecs_add(world, e, TagB);
        }
        if (!(i % 3)) {
            ecs_add(world, e, TagC);
        }
    }

    ecs_query_t *q = ecs_query(world, { .expr = "TagA" });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_int(500, ecs_iter_count(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Iter_interleaved_iter(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_query_t *f = ecs_query(world, {
        .terms = {{ TagA }}
    });

    ecs_entity_t e1 = ecs_new_w(world, TagA);
    ecs_entity_t e2 = ecs_new_w(world, TagA);
    ecs_add(world, e2, TagB);

    /* Bit of whitebox testing, check whether the stack cursor is restored to
     * its original position after the 2nd iterator is done */
    ecs_iter_t it_1 = ecs_query_iter(world, f);
    ecs_stack_cursor_t cursor = *it_1.priv_.cache.stack_cursor;
    ecs_iter_t it_2 = ecs_query_iter(world, f);

    test_bool(true, ecs_query_next(&it_1));
    test_bool(true, ecs_query_next(&it_2));
    test_int(e1, it_1.entities[0]);
    test_int(e1, it_2.entities[0]);

    test_bool(true, ecs_query_next(&it_1));
    test_bool(true, ecs_query_next(&it_2));
    test_int(e2, it_1.entities[0]);
    test_int(e2, it_2.entities[0]);

    test_bool(false, ecs_query_next(&it_1));
    test_bool(false, ecs_query_next(&it_2));

    it_1 = ecs_query_iter(world, f);
    test_assert(it_1.priv_.cache.stack_cursor->page == cursor.page);
    test_assert(it_1.priv_.cache.stack_cursor->sp == cursor.sp);
    ecs_iter_fini(&it_1);

    ecs_query_fini(f);

    ecs_fini(world);
}

void Iter_iter_restore_stack_iter(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *f = ecs_query(world, {
        .terms = {{ ecs_id(Position) }}
    });

    ecs_iter_t it = ecs_query_iter(world, f);
    ecs_stack_cursor_t cursor = *it.priv_.cache.stack_cursor;
    ecs_iter_fini(&it);

    it = ecs_query_iter(world, f);
    test_assert(it.priv_.cache.stack_cursor->page == cursor.page);
    test_assert(it.priv_.cache.stack_cursor->sp == cursor.sp);
    ecs_iter_fini(&it);

    ecs_query_fini(f);

    ecs_fini(world);
}

void Iter_get_first(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new_w(world, Tag);
    ecs_new_w(world, Tag);
    ecs_new_w(world, Tag);

    ecs_query_t *f = ecs_query_init(world, &(ecs_query_desc_t){
        .terms = {{ Tag }}
    });

    ecs_iter_t it = ecs_query_iter(world, f);
    ecs_entity_t first = ecs_iter_first(&it);
    test_assert(first == e);

    ecs_query_fini(f);

    ecs_fini(world);
}

void Iter_page_iter_w_only_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_query_t *f = ecs_query(world, {
        .terms = {{ Tag }}
    });
    
    ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);

    ecs_iter_t it = ecs_query_iter(world, f);
    ecs_iter_t pit = ecs_page_iter(&it, 1, 0);

    test_assert(ecs_page_next(&pit));
    test_int(pit.count, 1);
    test_uint(pit.entities[0], e2);
    test_assert(!ecs_page_next(&pit));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Iter_worker_iter_w_only_tag(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_query_t *f = ecs_query(world, {
        .terms = {{ Tag }}
    });
    
    ecs_new_w(world, Tag);
    ecs_entity_t e2 = ecs_new_w(world, Tag);

    ecs_iter_t it = ecs_query_iter(world, f);
    ecs_iter_t pit = ecs_worker_iter(&it, 1, 2);

    test_assert(ecs_worker_next(&pit));
    test_int(pit.count, 1);
    test_uint(pit.entities[0], e2);
    test_assert(!ecs_worker_next(&pit));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Iter_page_iter_w_inout_none(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *f = ecs_query(world, {
        .terms = {{ ecs_id(Position), .inout = EcsInOutNone }}
    });
    
    ecs_new_w(world, Position);
    ecs_entity_t e2 = ecs_new_w(world, Position);

    ecs_iter_t it = ecs_query_iter(world, f);
    ecs_iter_t pit = ecs_page_iter(&it, 1, 0);

    test_assert(ecs_page_next(&pit));
    test_int(pit.count, 1);
    test_uint(pit.entities[0], e2);
    test_assert(!ecs_page_next(&pit));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Iter_worker_iter_w_inout_none(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *f = ecs_query(world, {
        .terms = {{ ecs_id(Position), .inout = EcsInOutNone }}
    });
    
    ecs_new_w(world, Position);
    ecs_entity_t e2 = ecs_new_w(world, Position);

    ecs_iter_t it = ecs_query_iter(world, f);
    ecs_iter_t pit = ecs_worker_iter(&it, 1, 2);

    test_assert(ecs_worker_next(&pit));
    test_int(pit.count, 1);
    test_uint(pit.entities[0], e2);
    test_assert(!ecs_worker_next(&pit));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Iter_page_iter_w_ctx(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_query_t *f = ecs_query(world, {
        .terms = {{ TagA  }}
    });
    
    ecs_new_w(world, TagA);

    int ctx;

    ecs_iter_t it = ecs_query_iter(world, f);
    it.ctx = &ctx;

    ecs_iter_t pit = ecs_page_iter(&it, 0, 1);
    test_assert(pit.ctx == &ctx);

    test_assert(ecs_page_next(&pit));
    test_assert(!ecs_page_next(&pit));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Iter_page_iter_w_binding_ctx(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_query_t *f = ecs_query(world, {
        .terms = {{ TagA  }}
    });
    
    ecs_new_w(world, TagA);

    int ctx;

    ecs_iter_t it = ecs_query_iter(world, f);
    it.callback_ctx = &ctx;

    ecs_iter_t pit = ecs_page_iter(&it, 0, 1);
    test_assert(pit.callback_ctx == &ctx);

    test_assert(ecs_page_next(&pit));
    test_assert(!ecs_page_next(&pit));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Iter_worker_iter_w_ctx(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_query_t *f = ecs_query(world, {
        .terms = {{ TagA  }}
    });
    
    ecs_new_w(world, TagA);

    int ctx;

    ecs_iter_t it = ecs_query_iter(world, f);
    it.ctx = &ctx;

    ecs_iter_t pit = ecs_worker_iter(&it, 0, 2);
    test_assert(pit.ctx == &ctx);

    test_assert(ecs_worker_next(&pit));
    test_assert(!ecs_worker_next(&pit));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Iter_worker_iter_w_binding_ctx(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_query_t *f = ecs_query(world, {
        .terms = {{ TagA  }}
    });
    
    ecs_new_w(world, TagA);

    int ctx;

    ecs_iter_t it = ecs_query_iter(world, f);
    it.callback_ctx = &ctx;

    ecs_iter_t pit = ecs_worker_iter(&it, 0, 2);
    test_assert(pit.callback_ctx == &ctx);

    test_assert(ecs_worker_next(&pit));
    test_assert(!ecs_worker_next(&pit));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Iter_column_index_owned(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_query_t *f = ecs_query(world, {
        .terms = {{ TagB }, { TagC }, { TagA }}
    });

    ecs_entity_t e1 = ecs_new(world);
    ecs_add(world, e1, TagA);
    ecs_add(world, e1, TagB);
    ecs_add(world, e1, TagC);

    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_add(world, e2, TagA);
    ecs_add(world, e2, TagB);
    ecs_add(world, e2, TagC);

    ecs_iter_t it = ecs_query_iter(world, f);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_int(1, ecs_field_column(&it, 0));
    test_int(2, ecs_field_column(&it, 1));
    test_int(0, ecs_field_column(&it, 2));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_int(2, ecs_field_column(&it, 0));
    test_int(3, ecs_field_column(&it, 1));
    test_int(1, ecs_field_column(&it, 2));
    
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Iter_column_index_shared(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_add_pair(world, TagA, EcsOnInstantiate, EcsInherit);

    ecs_query_t *f = ecs_query(world, {
        .terms = {{ TagB }, { TagC }, { TagA }}
    });

    ecs_entity_t base = ecs_new_w(world, TagA);

    ecs_entity_t e1 = ecs_new(world);
    ecs_add_pair(world, e1, EcsIsA, base);
    ecs_add(world, e1, TagB);
    ecs_add(world, e1, TagC);

    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_add_pair(world, e2, EcsIsA, base);
    ecs_add(world, e2, TagB);
    ecs_add(world, e2, TagC);

    ecs_iter_t it = ecs_query_iter(world, f);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_int(0, ecs_field_column(&it, 0));
    test_int(1, ecs_field_column(&it, 1));
    test_int(0, ecs_field_column(&it, 2));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_int(1, ecs_field_column(&it, 0));
    test_int(2, ecs_field_column(&it, 1));
    test_int(0, ecs_field_column(&it, 2));
    
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Iter_column_index_not(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    ecs_query_t *f = ecs_query(world, {
        .terms = {{ TagB }, { TagC }, { TagA, .oper = EcsNot }}
    });

    ecs_entity_t e1 = ecs_new(world);
    ecs_add(world, e1, TagB);
    ecs_add(world, e1, TagC);

    ecs_entity_t e2 = ecs_new_w(world, Position);
    ecs_add(world, e2, TagB);
    ecs_add(world, e2, TagC);

    ecs_iter_t it = ecs_query_iter(world, f);
    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e1, it.entities[0]);
    test_int(0, ecs_field_column(&it, 0));
    test_int(1, ecs_field_column(&it, 1));
    test_int(-1, ecs_field_column(&it, 2));

    test_bool(true, ecs_query_next(&it));
    test_int(1, it.count);
    test_uint(e2, it.entities[0]);
    test_int(1, ecs_field_column(&it, 0));
    test_int(2, ecs_field_column(&it, 1));
    test_int(-1, ecs_field_column(&it, 2));
    
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(f);

    ecs_fini(world);
}

void Iter_page_iter_w_fini(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_query_t *f = ecs_query(world, {
        .terms = {{ ecs_id(Position) }}
    });

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_add(world, e2, Foo);

    ecs_iter_t it = ecs_query_iter(world, f);
    ecs_iter_t pit = ecs_page_iter(&it, 0, 2);
    test_bool(true, ecs_page_next(&pit));
    test_int(pit.count, 1);
    test_int(pit.entities[0], e1);
    
    ecs_iter_fini(&pit);

    ecs_query_fini(f);

    ecs_fini(world);
}

void Iter_worker_iter_w_fini(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_query_t *f = ecs_query(world, {
        .terms = {{ ecs_id(Position) }}
    });

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_add(world, e2, Foo);

    ecs_iter_t it = ecs_query_iter(world, f);
    ecs_iter_t pit = ecs_worker_iter(&it, 0, 2);
    test_bool(true, ecs_worker_next(&pit));
    test_int(pit.count, 1);
    test_int(pit.entities[0], e1);
    ecs_iter_fini(&pit);

    ecs_query_fini(f);

    ecs_fini(world);
}

void Iter_rule_page_iter_w_fini(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_query_t *r = ecs_query(world, {
        .terms = {{ ecs_id(Position) }}
    });

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_add(world, e2, Foo);

    ecs_iter_t it = ecs_query_iter(world, r);
    ecs_iter_t pit = ecs_page_iter(&it, 0, 2);
    test_bool(true, ecs_page_next(&pit));
    test_int(pit.count, 1);
    test_int(pit.entities[0], e1);
    ecs_iter_fini(&pit);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Iter_rule_worker_iter_w_fini(void) {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Foo);

    ecs_query_t *r = ecs_query(world, {
        .terms = {{ ecs_id(Position) }}
    });

    ecs_entity_t e1 = ecs_insert(world, ecs_value(Position, {10, 20}));
    ecs_entity_t e2 = ecs_insert(world, ecs_value(Position, {20, 30}));
    ecs_add(world, e2, Foo);

    ecs_iter_t it = ecs_query_iter(world, r);
    ecs_iter_t pit = ecs_worker_iter(&it, 0, 2);
    test_bool(true, ecs_worker_next(&pit));
    test_int(pit.count, 1);
    test_int(pit.entities[0], e1);
    ecs_iter_fini(&pit);

    ecs_query_fini(r);

    ecs_fini(world);
}

void Iter_to_str_before_next(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query(world, { .expr = "Tag" });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    char *str = ecs_iter_str(&it);
    test_assert(str == NULL);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}

void Iter_to_str(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    
    ecs_entity_t e = ecs_entity(world, { .name = "foo" });
    ecs_add(world, e, Tag);

    ecs_query_t *q = ecs_query(world, { .expr = "Tag" });
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_bool(true, ecs_query_next(&it));
    char *str = ecs_iter_str(&it);
    test_assert(str != NULL);
    test_str(str, 
        "id:  Tag\n"
        "src: #0\n"
        "set: true\n"
        "this:\n"
        "    - foo\n"
    );
    ecs_os_free(str);
    test_bool(false, ecs_query_next(&it));

    ecs_query_fini(q);

    ecs_fini(world);
}
