#include <query.h>

void TrivialIter_uncached_trivial_search(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }}
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(it.flags & EcsIterTrivialSearch);
    test_assert(!(it.flags & EcsIterTrivialTest));
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void TrivialIter_uncached_trivial_test(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }}
    });

    test_assert(q != NULL);

    ecs_entity_t e = ecs_new(world);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_set_var(&it, 0, e);
    test_assert(!(it.flags & EcsIterTrivialSearch));
    test_assert(it.flags & EcsIterTrivialTest);
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void TrivialIter_cached_trivial_search(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }},
        .cache_kind = EcsQueryCacheAll
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(it.flags & EcsIterTrivialSearch);
    test_assert(it.flags & EcsIterTrivialCached);
    test_assert(!(it.flags & EcsIterTrivialTest));
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void TrivialIter_cached_trivial_test(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }},
        .cache_kind = EcsQueryCacheAll
    });

    test_assert(q != NULL);

    ecs_entity_t e = ecs_new(world);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_set_var(&it, 0, e);
    test_assert(it.flags & EcsIterTrivialTest);
    test_assert(it.flags & EcsIterTrivialCached);
    test_assert(!(it.flags & EcsIterTrivialSearch));
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void TrivialIter_cached_trivial_search_w_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo, .src.id = EcsUp }},
        .cache_kind = EcsQueryCacheAll
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(it.flags & EcsIterTrivialSearch);
    test_assert(it.flags & EcsIterCached);
    test_assert(!(it.flags & EcsIterTrivialTest));
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void TrivialIter_cached_trivial_test_w_up(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo, .src.id = EcsUp }},
        .cache_kind = EcsQueryCacheAll
    });

    test_assert(q != NULL);

    ecs_entity_t e = ecs_new(world);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_set_var(&it, 0, e);
    test_assert(it.flags & EcsIterTrivialTest);
    test_assert(it.flags & EcsIterCached);
    test_assert(!(it.flags & EcsIterTrivialSearch));
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void TrivialIter_cached_trivial_search_w_not_operator(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { Bar, .oper = EcsNot }},
        .cache_kind = EcsQueryCacheAll
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(it.flags & EcsIterTrivialSearch);
    test_assert(it.flags & EcsIterTrivialCached);
    test_assert(!(it.flags & EcsIterTrivialTest));
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void TrivialIter_cached_trivial_test_w_not_operator(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { Bar, .oper = EcsNot }},
        .cache_kind = EcsQueryCacheAll
    });

    test_assert(q != NULL);

    ecs_entity_t e = ecs_new(world);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_set_var(&it, 0, e);
    test_assert(it.flags & EcsIterTrivialTest);
    test_assert(it.flags & EcsIterTrivialCached);
    test_assert(!(it.flags & EcsIterTrivialSearch));
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void TrivialIter_cached_trivial_search_w_not_wildcard_operator(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_pair(Bar, EcsWildcard), .oper = EcsNot }},
        .cache_kind = EcsQueryCacheAll
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(it.flags & EcsIterTrivialSearch);
    test_assert(it.flags & EcsIterTrivialCached);
    test_assert(!(it.flags & EcsIterTrivialTest));
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void TrivialIter_cached_trivial_test_w_not_wildcard_operator(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_pair(Bar, EcsWildcard), .oper = EcsNot }},
        .cache_kind = EcsQueryCacheAll
    });

    test_assert(q != NULL);

    ecs_entity_t e = ecs_new(world);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_set_var(&it, 0, e);
    test_assert(it.flags & EcsIterTrivialTest);
    test_assert(it.flags & EcsIterTrivialCached);
    test_assert(!(it.flags & EcsIterTrivialSearch));
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void TrivialIter_cached_trivial_search_w_or_operator(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo, .oper = EcsOr }, { Bar }},
        .cache_kind = EcsQueryCacheAll
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(it.flags & EcsIterTrivialSearch);
    test_assert(it.flags & EcsIterCached);
    test_assert(!(it.flags & EcsIterTrivialTest));
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void TrivialIter_cached_trivial_test_w_or_operator(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo, .oper = EcsOr }, { Bar }},
        .cache_kind = EcsQueryCacheAll
    });

    test_assert(q != NULL);

    ecs_entity_t e = ecs_new(world);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_set_var(&it, 0, e);
    test_assert(it.flags & EcsIterTrivialTest);
    test_assert(it.flags & EcsIterCached);
    test_assert(!(it.flags & EcsIterTrivialSearch));
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void TrivialIter_cached_trivial_search_w_optional_operator(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { Bar, .oper = EcsOptional }},
        .cache_kind = EcsQueryCacheAll
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(it.flags & EcsIterTrivialSearch);
    test_assert(it.flags & EcsIterTrivialCached);
    test_assert(!(it.flags & EcsIterTrivialTest));
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void TrivialIter_cached_trivial_test_w_optional_operator(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { Bar, .oper = EcsOptional }},
        .cache_kind = EcsQueryCacheAll
    });

    test_assert(q != NULL);

    ecs_entity_t e = ecs_new(world);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_set_var(&it, 0, e);
    test_assert(it.flags & EcsIterTrivialTest);
    test_assert(it.flags & EcsIterTrivialCached);
    test_assert(!(it.flags & EcsIterTrivialSearch));
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void TrivialIter_cached_trivial_search_w_optional_wildcard_operator(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_pair(Bar, EcsWildcard), .oper = EcsOptional }},
        .cache_kind = EcsQueryCacheAll
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(it.flags & EcsIterTrivialSearch);
    test_assert(it.flags & EcsIterCached);
    test_assert(!(it.flags & EcsIterTrivialTest));
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void TrivialIter_cached_trivial_test_w_optional_wildcard_operator(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ Foo }, { ecs_pair(Bar, EcsWildcard), .oper = EcsOptional }},
        .cache_kind = EcsQueryCacheAll
    });

    test_assert(q != NULL);

    ecs_entity_t e = ecs_new(world);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_set_var(&it, 0, e);
    test_assert(it.flags & EcsIterTrivialTest);
    test_assert(it.flags & EcsIterCached);
    test_assert(!(it.flags & EcsIterTrivialSearch));
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void TrivialIter_cached_trivial_search_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_pair(Foo, EcsWildcard) }},
        .cache_kind = EcsQueryCacheAll
    });

    test_assert(q != NULL);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(it.flags & EcsIterTrivialSearch);
    test_assert(it.flags & EcsIterCached);
    test_assert(!(it.flags & EcsIterTrivialTest));
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}

void TrivialIter_cached_trivial_test_w_wildcard(void) {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Foo);

    ecs_query_t *q = ecs_query(world, {
        .terms = {{ ecs_pair(Foo, EcsWildcard) }},
        .cache_kind = EcsQueryCacheAll
    });

    test_assert(q != NULL);

    ecs_entity_t e = ecs_new(world);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_iter_set_var(&it, 0, e);
    test_assert(it.flags & EcsIterTrivialTest);
    test_assert(it.flags & EcsIterCached);
    test_assert(!(it.flags & EcsIterTrivialSearch));
    ecs_iter_fini(&it);

    ecs_query_fini(q);

    ecs_fini(world);
}
