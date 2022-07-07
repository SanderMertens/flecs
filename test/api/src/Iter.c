#include <api.h>

void Iter_page_iter_0_0() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_id(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new_id(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new_id(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new_id(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new_id(world); ecs_set(world, e5, Self, {e5});

    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagB);

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ ecs_id(Self) }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);
    ecs_iter_t pit = ecs_page_iter(&it, 0, 0);

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 2);
        test_int(pit.frame_offset, 0);
        test_int(pit.entities[0], e1);
        test_int(pit.entities[1], e2);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit, Self, 1);
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

        Self *ptr = ecs_term(&pit, Self, 1);
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

        Self *ptr = ecs_term(&pit, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e5);
    }

    test_bool(ecs_page_next(&pit), false);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Iter_page_iter_1_0() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_id(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new_id(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new_id(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new_id(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new_id(world); ecs_set(world, e5, Self, {e5});

    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagB);

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ ecs_id(Self) }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);
    ecs_iter_t pit = ecs_page_iter(&it, 1, 0);

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 1);
        test_int(pit.entities[0], e2);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e2);
    }

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 2);
        test_int(pit.entities[0], e3);
        test_int(pit.entities[1], e4);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e3);
        test_int(ptr[1].value, e4);
    }

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 1);
        test_int(pit.entities[0], e5);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e5);
    }

    test_bool(ecs_page_next(&pit), false);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Iter_page_iter_0_1() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_id(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new_id(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new_id(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new_id(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new_id(world); ecs_set(world, e5, Self, {e5});

    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagB);

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ ecs_id(Self) }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);
    ecs_iter_t pit = ecs_page_iter(&it, 0, 1);

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 1);
        test_int(pit.entities[0], e1);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e1);
    }

    test_bool(ecs_page_next(&pit), false);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Iter_page_iter_n_0() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_id(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new_id(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new_id(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new_id(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new_id(world); ecs_set(world, e5, Self, {e5});

    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagB);

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ ecs_id(Self) }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);
    ecs_iter_t pit = ecs_page_iter(&it, 2, 0);

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 1);
        test_int(pit.entities[0], e3);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e3);
    }

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 1);
        test_int(pit.entities[0], e4);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e4);
    }

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 1);
        test_int(pit.entities[0], e5);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e5);
    }

    test_bool(ecs_page_next(&pit), false);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Iter_page_iter_0_n() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_id(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new_id(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new_id(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new_id(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new_id(world); ecs_set(world, e5, Self, {e5});

    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagB);

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ ecs_id(Self) }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);
    ecs_iter_t pit = ecs_page_iter(&it, 0, 2);

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 2);
        test_int(pit.entities[0], e1);
        test_int(pit.entities[1], e2);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e1);
        test_int(ptr[1].value, e2);
    }

    test_bool(ecs_page_next(&pit), false);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Iter_page_iter_m_n() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_id(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new_id(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new_id(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new_id(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new_id(world); ecs_set(world, e5, Self, {e5});
    ecs_entity_t e6 = ecs_new_id(world); ecs_set(world, e6, Self, {e6});

    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagA);
    ecs_add(world, e6, TagB);

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ ecs_id(Self) }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);
    ecs_iter_t pit = ecs_page_iter(&it, 2, 3);

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 1);
        test_int(pit.entities[0], e3);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e3);
    }
    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 2);
        test_int(pit.entities[0], e4);
        test_int(pit.entities[1], e5);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e4);
        test_int(ptr[1].value, e5);
    }

    test_bool(ecs_page_next(&pit), false);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Iter_page_iter_skip_1_table() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_id(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new_id(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new_id(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new_id(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new_id(world); ecs_set(world, e5, Self, {e5});

    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagB);

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ ecs_id(Self) }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);
    ecs_iter_t pit = ecs_page_iter(&it, 2, 0);

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 2);
        test_int(pit.entities[0], e3);
        test_int(pit.entities[1], e4);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e3);
        test_int(ptr[1].value, e4);
    }

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 1);
        test_int(pit.entities[0], e5);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e5);
    }

    test_bool(ecs_page_next(&pit), false);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Iter_page_iter_skip_2_tables() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_id(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new_id(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new_id(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new_id(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new_id(world); ecs_set(world, e5, Self, {e5});

    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagB);
    ecs_add(world, e5, TagB);

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ ecs_id(Self) }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);
    ecs_iter_t pit = ecs_page_iter(&it, 3, 0);

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 2);
        test_int(pit.entities[0], e4);
        test_int(pit.entities[1], e5);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e4);
        test_int(ptr[1].value, e5);
    }

    test_bool(ecs_page_next(&pit), false);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Iter_worker_iter_1() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_id(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new_id(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new_id(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new_id(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new_id(world); ecs_set(world, e5, Self, {e5});

    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagB);

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ ecs_id(Self) }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);
    ecs_iter_t pit = ecs_worker_iter(&it, 0, 1);

    {
        test_bool(ecs_worker_next(&pit), true);
        test_int(pit.count, 2);
        test_int(pit.entities[0], e1);
        test_int(pit.entities[1], e2);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit, Self, 1);
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

        Self *ptr = ecs_term(&pit, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e3);
        test_int(ptr[1].value, e4);
    }

    {
        test_bool(ecs_worker_next(&pit), true);
        test_int(pit.count, 1);
        test_int(pit.entities[0], e5);
        test_int(pit.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e5);
    }

    test_bool(ecs_worker_next(&pit), false);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Iter_worker_iter_2() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_id(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new_id(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new_id(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new_id(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new_id(world); ecs_set(world, e5, Self, {e5});

    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagB);

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ ecs_id(Self) }}
    });

    ecs_iter_t it_1 = ecs_filter_iter(world, &f);
    ecs_iter_t pit_1 = ecs_worker_iter(&it_1, 0, 2);
    ecs_iter_t it_2 = ecs_filter_iter(world, &f);
    ecs_iter_t pit_2 = ecs_worker_iter(&it_2, 1, 2);

    /* Iter 1 */
    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 1);
        test_int(pit_1.entities[0], e1);
        test_int(pit_1.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit_1, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e1);
    }

    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 1);
        test_int(pit_1.entities[0], e3);
        test_int(pit_1.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit_1, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e3);
    }

    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 1);
        test_int(pit_1.entities[0], e5);
        test_int(pit_1.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit_1, Self, 1);
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

        Self *ptr = ecs_term(&pit_2, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e2);
    }

    {
        test_bool(ecs_worker_next(&pit_2), true);
        test_int(pit_2.count, 1);
        test_int(pit_2.entities[0], e4);
        test_int(pit_2.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit_2, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e4);
    }

    test_bool(ecs_worker_next(&pit_2), false);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Iter_worker_iter_3() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_id(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new_id(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new_id(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new_id(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new_id(world); ecs_set(world, e5, Self, {e5});
    ecs_entity_t e6 = ecs_new_id(world); ecs_set(world, e6, Self, {e6});

    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagA);
    ecs_add(world, e6, TagB);

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ ecs_id(Self) }}
    });

    ecs_iter_t it_1 = ecs_filter_iter(world, &f);
    ecs_iter_t pit_1 = ecs_worker_iter(&it_1, 0, 3);
    ecs_iter_t it_2 = ecs_filter_iter(world, &f);
    ecs_iter_t pit_2 = ecs_worker_iter(&it_2, 1, 3);
    ecs_iter_t it_3 = ecs_filter_iter(world, &f);
    ecs_iter_t pit_3 = ecs_worker_iter(&it_3, 2, 3);

    /* Iter 1 */
    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 1);
        test_int(pit_1.entities[0], e1);
        test_int(pit_1.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit_1, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e1);
    }

    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 1);
        test_int(pit_1.entities[0], e4);
        test_int(pit_1.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit_1, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e4);
    }

    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 1);
        test_int(pit_1.entities[0], e6);
        test_int(pit_1.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit_1, Self, 1);
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

        Self *ptr = ecs_term(&pit_2, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e2);
    }

    {
        test_bool(ecs_worker_next(&pit_2), true);
        test_int(pit_2.count, 1);
        test_int(pit_2.entities[0], e5);
        test_int(pit_2.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit_2, Self, 1);
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

        Self *ptr = ecs_term(&pit_3, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e3);
    }

    test_bool(ecs_worker_next(&pit_3), false);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Iter_worker_iter_4() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t e1 = ecs_new_id(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new_id(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new_id(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new_id(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new_id(world); ecs_set(world, e5, Self, {e5});
    ecs_entity_t e6 = ecs_new_id(world); ecs_set(world, e6, Self, {e6});
    ecs_entity_t e7 = ecs_new_id(world); ecs_set(world, e7, Self, {e7});
    ecs_entity_t e8 = ecs_new_id(world); ecs_set(world, e8, Self, {e8});
    ecs_entity_t e9 = ecs_new_id(world); ecs_set(world, e9, Self, {e9});

    ecs_add(world, e5, TagA);
    ecs_add(world, e6, TagA);
    ecs_add(world, e7, TagA);
    ecs_add(world, e8, TagB);
    ecs_add(world, e9, TagB);

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ ecs_id(Self) }}
    });

    ecs_iter_t it_1 = ecs_filter_iter(world, &f);
    ecs_iter_t pit_1 = ecs_worker_iter(&it_1, 0, 4);
    ecs_iter_t it_2 = ecs_filter_iter(world, &f);
    ecs_iter_t pit_2 = ecs_worker_iter(&it_2, 1, 4);
    ecs_iter_t it_3 = ecs_filter_iter(world, &f);
    ecs_iter_t pit_3 = ecs_worker_iter(&it_3, 2, 4);
    ecs_iter_t it_4 = ecs_filter_iter(world, &f);
    ecs_iter_t pit_4 = ecs_worker_iter(&it_4, 3, 4);

    /* Iter 1 */
    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 1);
        test_int(pit_1.entities[0], e1);
        test_int(pit_1.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit_1, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e1);
    }

    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 1);
        test_int(pit_1.entities[0], e5);
        test_int(pit_1.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit_1, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e5);
    }

    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 1);
        test_int(pit_1.entities[0], e8);
        test_int(pit_1.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit_1, Self, 1);
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

        Self *ptr = ecs_term(&pit_2, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e2);
    }

    {
        test_bool(ecs_worker_next(&pit_2), true);
        test_int(pit_2.count, 1);
        test_int(pit_2.entities[0], e6);
        test_int(pit_2.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit_2, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e6);
    }

    {
        test_bool(ecs_worker_next(&pit_2), true);
        test_int(pit_2.count, 1);
        test_int(pit_2.entities[0], e9);
        test_int(pit_2.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit_2, Self, 1);
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

        Self *ptr = ecs_term(&pit_3, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e3);
    }

    {
        test_bool(ecs_worker_next(&pit_3), true);
        test_int(pit_3.count, 1);
        test_int(pit_3.entities[0], e7);
        test_int(pit_3.ids[0], ecs_id(Self));

        Self *ptr = ecs_term(&pit_3, Self, 1);
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

        Self *ptr = ecs_term(&pit_4, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e4);
    }

    test_bool(ecs_worker_next(&pit_4), false);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Iter_paged_iter_w_shared_comp() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Self);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t foo = ecs_set(world, 0, Position, {10, 20});

    ecs_entity_t e1 = ecs_new_id(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new_id(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new_id(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new_id(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new_id(world); ecs_set(world, e5, Self, {e5});

    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagB);

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ ecs_id(Self) }, { ecs_id(Position), .subj.entity = foo} },
        .instanced = true
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);
    ecs_iter_t pit = ecs_page_iter(&it, 0, 0);

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 2);
        test_int(pit.entities[0], e1);
        test_int(pit.entities[1], e2);
        test_int(pit.ids[0], ecs_id(Self));
        test_int(pit.ids[1], ecs_id(Position));
        test_int(pit.subjects[0], 0);
        test_int(pit.subjects[1], foo);

        Self *ptr = ecs_term(&pit, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e1);
        test_int(ptr[1].value, e2);

        Position *pptr = ecs_term(&pit, Position, 2);
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
        test_int(pit.subjects[0], 0);
        test_int(pit.subjects[1], foo);

        Self *ptr = ecs_term(&pit, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e3);
        test_int(ptr[1].value, e4);

        Position *pptr = ecs_term(&pit, Position, 2);
        test_int(pptr->x, 10);
        test_int(pptr->y, 20);
    }

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 1);
        test_int(pit.entities[0], e5);
        test_int(pit.ids[0], ecs_id(Self));
        test_int(pit.ids[1], ecs_id(Position));
        test_int(pit.subjects[0], 0);
        test_int(pit.subjects[1], foo);

        Self *ptr = ecs_term(&pit, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e5);

        Position *pptr = ecs_term(&pit, Position, 2);
        test_int(pptr->x, 10);
        test_int(pptr->y, 20);
    }

    test_bool(ecs_page_next(&pit), false);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Iter_worker_iter_w_shared_comp() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);
    ECS_COMPONENT(world, Position);
    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);

    ecs_entity_t foo = ecs_set(world, 0, Position, {10, 20});

    ecs_entity_t e1 = ecs_new_id(world); ecs_set(world, e1, Self, {e1});
    ecs_entity_t e2 = ecs_new_id(world); ecs_set(world, e2, Self, {e2});
    ecs_entity_t e3 = ecs_new_id(world); ecs_set(world, e3, Self, {e3});
    ecs_entity_t e4 = ecs_new_id(world); ecs_set(world, e4, Self, {e4});
    ecs_entity_t e5 = ecs_new_id(world); ecs_set(world, e5, Self, {e5});

    ecs_add(world, e3, TagA);
    ecs_add(world, e4, TagA);
    ecs_add(world, e5, TagB);

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ ecs_id(Self) }, { ecs_id(Position), .subj.entity = foo} },
        .instanced = true
    });

    ecs_iter_t it_1 = ecs_filter_iter(world, &f);
    ecs_iter_t pit_1 = ecs_worker_iter(&it_1, 0, 2);
    ecs_iter_t it_2 = ecs_filter_iter(world, &f);
    ecs_iter_t pit_2 = ecs_worker_iter(&it_2, 1, 2);

    /* Iter 1 */
    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 1);
        test_int(pit_1.entities[0], e1);
        test_int(pit_1.ids[0], ecs_id(Self));
        test_int(pit_1.ids[1], ecs_id(Position));
        test_int(pit_1.subjects[0], 0);
        test_int(pit_1.subjects[1], foo);

        Self *ptr = ecs_term(&pit_1, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e1);

        Position *pptr = ecs_term(&pit_1, Position, 2);
        test_int(pptr->x, 10);
        test_int(pptr->y, 20);
    }

    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 1);
        test_int(pit_1.entities[0], e3);
        test_int(pit_1.ids[0], ecs_id(Self));
        test_int(pit_1.ids[1], ecs_id(Position));
        test_int(pit_1.subjects[0], 0);
        test_int(pit_1.subjects[1], foo);

        Self *ptr = ecs_term(&pit_1, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e3);

        Position *pptr = ecs_term(&pit_1, Position, 2);
        test_int(pptr->x, 10);
        test_int(pptr->y, 20);
    }

    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 1);
        test_int(pit_1.entities[0], e5);
        test_int(pit_1.ids[0], ecs_id(Self));
        test_int(pit_1.ids[1], ecs_id(Position));
        test_int(pit_1.subjects[0], 0);
        test_int(pit_1.subjects[1], foo);

        Self *ptr = ecs_term(&pit_1, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e5);

        Position *pptr = ecs_term(&pit_1, Position, 2);
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
        test_int(pit_2.subjects[0], 0);
        test_int(pit_2.subjects[1], foo);

        Self *ptr = ecs_term(&pit_2, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e2);

        Position *pptr = ecs_term(&pit_2, Position, 2);
        test_int(pptr->x, 10);
        test_int(pptr->y, 20);
    }

    {
        test_bool(ecs_worker_next(&pit_2), true);
        test_int(pit_2.count, 1);
        test_int(pit_2.entities[0], e4);
        test_int(pit_2.ids[0], ecs_id(Self));
        test_int(pit_2.ids[1], ecs_id(Position));
        test_int(pit_2.subjects[0], 0);
        test_int(pit_2.subjects[1], foo);

        Self *ptr = ecs_term(&pit_2, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, e4);

        Position *pptr = ecs_term(&pit_2, Position, 2);
        test_int(pptr->x, 10);
        test_int(pptr->y, 20);
    }

    test_bool(ecs_worker_next(&pit_2), false);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Iter_paged_iter_w_task_query() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);

    ecs_entity_t foo = ecs_new_id(world); ecs_set(world, foo, Self, {foo});

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ ecs_id(Self), .subj.entity = foo }}
    });

    ecs_iter_t it = ecs_filter_iter(world, &f);
    ecs_iter_t pit = ecs_page_iter(&it, 0, 0);

    {
        test_bool(ecs_page_next(&pit), true);
        test_int(pit.count, 0);
        test_int(pit.ids[0], ecs_id(Self));
        test_int(pit.subjects[0], foo);

        Self *ptr = ecs_term(&pit, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, foo);
    }

    test_bool(ecs_page_next(&pit), false);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Iter_worker_iter_w_task_query() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Self);

    ecs_entity_t foo = ecs_new_id(world); ecs_set(world, foo, Self, {foo});

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ ecs_id(Self), .subj.entity = foo }}
    });

    ecs_iter_t it_1 = ecs_filter_iter(world, &f);
    ecs_iter_t pit_1 = ecs_worker_iter(&it_1, 0, 2);
    ecs_iter_t it_2 = ecs_filter_iter(world, &f);
    ecs_iter_t pit_2 = ecs_worker_iter(&it_2, 1, 2);

    /* Iter 1 */
    {
        test_bool(ecs_worker_next(&pit_1), true);
        test_int(pit_1.count, 0);
        test_int(pit_1.ids[0], ecs_id(Self));
        test_int(pit_1.subjects[0], foo);

        Self *ptr = ecs_term(&pit_1, Self, 1);
        test_assert(ptr != NULL);
        test_int(ptr[0].value, foo);
    }

    test_bool(ecs_worker_next(&pit_1), false);

    /* Iter 2 */

    test_bool(ecs_worker_next(&pit_2), false);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Iter_iter_1_term_no_alloc() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ TagA }}
    });

    ecs_new(world, TagA);

    ecs_iter_t it = ecs_filter_iter(world, &f);
    test_bool(ecs_filter_next(&it), true);
    test_assert(it.ids == it.priv.cache.ids);
    test_assert(it.columns == it.priv.cache.columns);
    test_assert(it.subjects == it.priv.cache.subjects);
    test_assert(it.sizes == it.priv.cache.sizes);
    test_assert(it.ptrs == it.priv.cache.ptrs);
    test_assert(it.match_indices == it.priv.cache.match_indices);

    ecs_iter_fini(&it);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Iter_iter_cache_size_terms_no_alloc() {
    ecs_world_t *world = ecs_mini();

    test_int(ECS_TERM_CACHE_SIZE, 4);

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ TagA }, { TagB }, { TagC }, { TagD }}
    });

    ecs_entity_t e = ecs_new(world, TagA);
    ecs_add(world, e, TagB);
    ecs_add(world, e, TagC);
    ecs_add(world, e, TagD);

    ecs_iter_t it = ecs_filter_iter(world, &f);
    test_bool(ecs_filter_next(&it), true);
    test_assert(it.ids == it.priv.cache.ids);
    test_assert(it.columns == it.priv.cache.columns);
    test_assert(it.subjects == it.priv.cache.subjects);
    test_assert(it.sizes == it.priv.cache.sizes);
    test_assert(it.ptrs == it.priv.cache.ptrs);
    test_assert(it.match_indices == it.priv.cache.match_indices);

    ecs_iter_fini(&it);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Iter_iter_lt_cache_size_terms_alloc() {
    ecs_world_t *world = ecs_mini();

    test_int(ECS_TERM_CACHE_SIZE, 4);

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);
    ECS_TAG(world, TagE);

    ecs_filter_t f = ECS_FILTER_INIT;
    ecs_filter_init(world, &(ecs_filter_desc_t){
        .storage = &f,
        .terms = {{ TagA }, { TagB }, { TagC }, { TagD }, { TagE }}
    });

    ecs_entity_t e = ecs_new(world, TagA);
    ecs_add(world, e, TagB);
    ecs_add(world, e, TagC);
    ecs_add(world, e, TagD);
    ecs_add(world, e, TagE);

    ecs_iter_t it = ecs_filter_iter(world, &f);
    test_bool(ecs_filter_next(&it), true);
    test_assert(it.ids != it.priv.cache.ids);
    test_assert(it.columns != it.priv.cache.columns);
    test_assert(it.subjects != it.priv.cache.subjects);
    test_assert(it.sizes != it.priv.cache.sizes);
    test_assert(it.ptrs != it.priv.cache.ptrs);
    test_assert(it.match_indices != it.priv.cache.match_indices);

    ecs_iter_fini(&it);

    ecs_filter_fini(&f);

    ecs_fini(world);
}

void Iter_worker_iter_w_singleton() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Singleton);
    ECS_COMPONENT(world, Position);

    ecs_singleton_add(world, Singleton);
    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {20, 30});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {30, 40});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {40, 50});

    ecs_query_t *q = ecs_query_new(world, "Position, Singleton($)");

    Position *p;

    ecs_iter_t it_1 = ecs_query_iter(world, q);
    ecs_iter_t wit_1 = ecs_worker_iter(&it_1, 0, 2);
    test_bool(ecs_worker_next(&wit_1), true);
    test_int(wit_1.count, 2);
    test_int(wit_1.entities[0], e1);
    test_int(wit_1.entities[1], e2);
    p = ecs_term(&wit_1, Position, 1);
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
    p = ecs_term(&wit_2, Position, 1);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);
    test_int(p[1].x, 40);
    test_int(p[1].y, 50);

    test_bool(ecs_worker_next(&wit_2), false);

    ecs_fini(world);
}

void Iter_worker_iter_w_singleton_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_singleton_set(world, Velocity, {1, 2});
    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {20, 30});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {30, 40});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {40, 50});

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity($)");

    Position *p;
    Velocity *v;

    ecs_iter_t it_1 = ecs_query_iter(world, q);
    ecs_iter_t wit_1 = ecs_worker_iter(&it_1, 0, 2);
    test_bool(ecs_worker_next(&wit_1), true);
    test_int(wit_1.count, 1);
    test_int(wit_1.entities[0], e1);
    p = ecs_term(&wit_1, Position, 1);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    v = ecs_term(&wit_1, Velocity, 2);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);

    test_bool(ecs_worker_next(&wit_1), true);
    test_int(wit_1.count, 1);
    test_int(wit_1.entities[0], e2);
    p = ecs_term(&wit_1, Position, 1);
    test_int(p[0].x, 20);
    test_int(p[0].y, 30);
    v = ecs_term(&wit_1, Velocity, 2);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);

    test_bool(ecs_worker_next(&wit_1), false);

    ecs_iter_t it_2 = ecs_query_iter(world, q);
    ecs_iter_t wit_2 = ecs_worker_iter(&it_2, 1, 2);
    test_bool(ecs_worker_next(&wit_2), true);
    test_int(wit_2.count, 1);
    test_int(wit_2.entities[0], e3);
    p = ecs_term(&wit_2, Position, 1);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);
    v = ecs_term(&wit_2, Velocity, 2);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);

    test_bool(ecs_worker_next(&wit_2), true);
    test_int(wit_2.count, 1);
    test_int(wit_2.entities[0], e4);
    p = ecs_term(&wit_2, Position, 1);
    test_int(p[0].x, 40);
    test_int(p[0].y, 50);
    v = ecs_term(&wit_2, Velocity, 2);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);

    test_bool(ecs_worker_next(&wit_2), false);

    ecs_fini(world);
}

void Iter_worker_iter_w_singleton_instanced() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Singleton);
    ECS_COMPONENT(world, Position);

    ecs_singleton_add(world, Singleton);
    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {20, 30});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {30, 40});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {40, 50});

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter = {
            .expr = "Position, Singleton($)",
            .instanced = true
        }
    });

    Position *p;

    ecs_iter_t it_1 = ecs_query_iter(world, q);
    ecs_iter_t wit_1 = ecs_worker_iter(&it_1, 0, 2);
    test_bool(ecs_worker_next(&wit_1), true);
    test_int(wit_1.count, 2);
    test_int(wit_1.entities[0], e1);
    test_int(wit_1.entities[1], e2);
    p = ecs_term(&wit_1, Position, 1);
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
    p = ecs_term(&wit_2, Position, 1);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);
    test_int(p[1].x, 40);
    test_int(p[1].y, 50);
    test_bool(ecs_worker_next(&wit_2), false);

    ecs_fini(world);
}

void Iter_worker_iter_w_singleton_component_instanced() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_singleton_set(world, Velocity, {1, 2});
    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {20, 30});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {30, 40});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {40, 50});

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter = {
            .expr = "Position, Velocity($)",
            .instanced = true
        }
    });

    Position *p;
    Velocity *v;

    ecs_iter_t it_1 = ecs_query_iter(world, q);
    ecs_iter_t wit_1 = ecs_worker_iter(&it_1, 0, 2);
    test_bool(ecs_worker_next(&wit_1), true);
    test_int(wit_1.count, 2);
    test_int(wit_1.entities[0], e1);
    test_int(wit_1.entities[1], e2);
    p = ecs_term(&wit_1, Position, 1);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);
    v = ecs_term(&wit_1, Velocity, 2);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_bool(ecs_worker_next(&wit_1), false);

    ecs_iter_t it_2 = ecs_query_iter(world, q);
    ecs_iter_t wit_2 = ecs_worker_iter(&it_2, 1, 2);
    test_bool(ecs_worker_next(&wit_2), true);
    test_int(wit_2.count, 2);
    test_int(wit_2.entities[0], e3);
    test_int(wit_2.entities[1], e4);
    p = ecs_term(&wit_2, Position, 1);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);
    test_int(p[1].x, 40);
    test_int(p[1].y, 50);
    v = ecs_term(&wit_2, Velocity, 2);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_bool(ecs_worker_next(&wit_2), false);

    ecs_fini(world);
}

void Iter_paged_iter_w_singleton() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Singleton);
    ECS_COMPONENT(world, Position);

    ecs_singleton_add(world, Singleton);
    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {20, 30});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {30, 40});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {40, 50});

    ecs_query_t *q = ecs_query_new(world, "Position, Singleton($)");

    Position *p;

    ecs_iter_t it_1 = ecs_query_iter(world, q);
    ecs_iter_t pit_1 = ecs_page_iter(&it_1, 0, 2);
    test_bool(ecs_page_next(&pit_1), true);
    test_int(pit_1.count, 2);
    test_int(pit_1.entities[0], e1);
    test_int(pit_1.entities[1], e2);
    p = ecs_term(&pit_1, Position, 1);
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
    p = ecs_term(&pit_2, Position, 1);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);
    test_int(p[1].x, 40);
    test_int(p[1].y, 50);
    test_bool(ecs_page_next(&pit_2), false);

    ecs_fini(world);
}

void Iter_paged_iter_w_singleton_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_singleton_set(world, Velocity, {1, 2});
    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {20, 30});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {30, 40});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {40, 50});

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity($)");

    Position *p;
    Velocity *v;

    ecs_iter_t it_1 = ecs_query_iter(world, q);
    ecs_iter_t pit_1 = ecs_page_iter(&it_1, 0, 2);
    test_bool(ecs_page_next(&pit_1), true);
    test_int(pit_1.count, 1);
    test_int(pit_1.entities[0], e1);
    p = ecs_term(&pit_1, Position, 1);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    v = ecs_term(&pit_1, Velocity, 2);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_bool(ecs_page_next(&pit_1), true);
    test_int(pit_1.count, 1);
    test_int(pit_1.entities[0], e2);
    p = ecs_term(&pit_1, Position, 1);
    test_int(p[0].x, 20);
    test_int(p[0].y, 30);
    v = ecs_term(&pit_1, Velocity, 2);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_bool(ecs_page_next(&pit_1), false);

    ecs_iter_t it_2 = ecs_query_iter(world, q);
    ecs_iter_t pit_2 = ecs_page_iter(&it_2, 2, 2);
    test_bool(ecs_page_next(&pit_2), true);
    test_int(pit_2.count, 1);
    test_int(pit_2.entities[0], e3);
    p = ecs_term(&pit_2, Position, 1);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);
    v = ecs_term(&pit_2, Velocity, 2);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_bool(ecs_page_next(&pit_2), true);
    test_int(pit_2.count, 1);
    test_int(pit_2.entities[0], e4);
    p = ecs_term(&pit_2, Position, 1);
    test_int(p[0].x, 40);
    test_int(p[0].y, 50);
    v = ecs_term(&pit_2, Velocity, 2);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_bool(ecs_page_next(&pit_2), false);

    ecs_fini(world);
}

void Iter_paged_iter_w_singleton_instanced() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Singleton);
    ECS_COMPONENT(world, Position);

    ecs_singleton_add(world, Singleton);
    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {20, 30});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {30, 40});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {40, 50});

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter = {
            .expr = "Position, Singleton($)",
            .instanced = true
        }
    });

    Position *p;

    ecs_iter_t it_1 = ecs_query_iter(world, q);
    ecs_iter_t pit_1 = ecs_page_iter(&it_1, 0, 2);
    test_bool(ecs_page_next(&pit_1), true);
    test_int(pit_1.count, 2);
    test_int(pit_1.entities[0], e1);
    test_int(pit_1.entities[1], e2);
    p = ecs_term(&pit_1, Position, 1);
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
    p = ecs_term(&pit_2, Position, 1);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);
    test_int(p[1].x, 40);
    test_int(p[1].y, 50);
    test_bool(ecs_page_next(&pit_2), false);

    ecs_fini(world);
}

void Iter_paged_iter_w_singleton_component_instanced() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_singleton_set(world, Velocity, {1, 2});
    ecs_entity_t e1 = ecs_set(world, 0, Position, {10, 20});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {20, 30});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {30, 40});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {40, 50});

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter = {
            .expr = "Position, Velocity($)",
            .instanced = true
        }
    });

    Position *p;
    Velocity *v;

    ecs_iter_t it_1 = ecs_query_iter(world, q);
    ecs_iter_t pit_1 = ecs_page_iter(&it_1, 0, 2);
    test_bool(ecs_page_next(&pit_1), true);
    test_int(pit_1.count, 2);
    test_int(pit_1.entities[0], e1);
    test_int(pit_1.entities[1], e2);
    p = ecs_term(&pit_1, Position, 1);
    test_int(p[0].x, 10);
    test_int(p[0].y, 20);
    test_int(p[1].x, 20);
    test_int(p[1].y, 30);
    v = ecs_term(&pit_1, Velocity, 2);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_bool(ecs_page_next(&pit_1), false);

    ecs_iter_t it_2 = ecs_query_iter(world, q);
    ecs_iter_t pit_2 = ecs_page_iter(&it_2, 2, 2);
    test_bool(ecs_page_next(&pit_2), true);
    test_int(pit_2.count, 2);
    test_int(pit_2.entities[0], e3);
    test_int(pit_2.entities[1], e4);
    p = ecs_term(&pit_2, Position, 1);
    test_int(p[0].x, 30);
    test_int(p[0].y, 40);
    test_int(p[1].x, 40);
    test_int(p[1].y, 50);
    v = ecs_term(&pit_2, Velocity, 2);
    test_int(v[0].x, 1);
    test_int(v[0].y, 2);
    test_bool(ecs_page_next(&pit_2), false);

    ecs_fini(world);
}

void Iter_count() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);

    for (int i = 0; i < 500; i ++) {
        ecs_entity_t e = ecs_new(world, TagA);
        if (!(i % 2)) {
            ecs_add(world, e, TagB);
        }
        if (!(i % 3)) {
            ecs_add(world, e, TagC);
        }
    }

    ecs_query_t *q = ecs_query_new(world, "TagA");
    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_int(500, ecs_iter_count(&it));

    ecs_fini(world);
}
