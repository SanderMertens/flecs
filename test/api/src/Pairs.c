#include <api.h>

typedef struct Rel {
    float value;
} Rel;

typedef struct RelA {
    float value;
} RelA;

typedef struct RelB {
    float value;
} RelB;

typedef struct Obj {
    float value;
} Obj;

void ProcessPairs(ecs_iter_t *it) {
    Rel *tr = ecs_field(it, Rel, 1);

    probe_iter(it);

    if (tr) {
        int32_t i;
        for (i = 0; i < it->count; i ++) {
            tr[i].value ++;
        }
    }
}

void Pairs_type_w_one_pair() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rel);

    ECS_SYSTEM(world, ProcessPairs, EcsOnUpdate, (Rel, *));

    /* Ensure that pair is matched against different components */
    ecs_entity_t e1 = ecs_set_pair(world, 0, Rel, ecs_id(Position), {
        .value = 10
    });

    test_assert(e1 != 0);
    test_assert( ecs_has_pair(world, e1, ecs_id(Rel), ecs_id(Position)));

    ecs_entity_t e2 = ecs_set_pair(world, 0, Rel, ecs_id(Velocity), {
        .value = 20
    });
    test_assert(e2 != 0);
    test_assert( ecs_has_pair(world, e2, ecs_id(Rel), ecs_id(Velocity)));

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    const Rel* tr = ecs_get_pair(world, e1, Rel, ecs_id(Position));
    test_assert(tr != NULL);
    test_int(tr->value, 11);

    tr = ecs_get_pair(world, e2, Rel, ecs_id(Velocity));
    test_assert(tr != NULL);
    test_int(tr->value, 21); 

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, ProcessPairs);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);

    ecs_entity_t c = ctx.c[0][0];
    ecs_entity_t hi = ECS_PAIR_FIRST(c);
    ecs_entity_t lo = ECS_PAIR_SECOND(c);
    test_int(hi, ecs_id(Rel));
    test_int(lo, ecs_id(Position));

    c = ctx.c[1][0];
    hi = ECS_PAIR_FIRST(c);
    lo = ECS_PAIR_SECOND(c);
    test_int(hi, ecs_id(Rel));
    test_int(lo, ecs_id(Velocity));

    test_int(ctx.s[0][0], 0);
    test_int(ctx.s[1][0], 0);

    ecs_fini(world);
}

void Pairs_type_w_two_pairs() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rel);

    ECS_SYSTEM(world, ProcessPairs, EcsOnUpdate, (Rel, *));

    /* Ensure that pair is matched against different components on same entity */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_set_pair(world, e1, Rel, ecs_id(Position), {
        .value = 10
    });  
    test_assert( ecs_has_pair(world, e1, ecs_id(Rel), ecs_id(Position)));

    ecs_set_pair(world, e1, Rel, ecs_id(Velocity), {
        .value = 20
    });      
    test_assert( ecs_has_pair(world, e1, ecs_id(Rel), ecs_id(Velocity)));

    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_set_pair(world, e2, Rel, ecs_id(Position), {
        .value = 30
    });
    test_assert( ecs_has_pair(world, e2, ecs_id(Rel), ecs_id(Position)));

    ecs_set_pair(world, e2, Rel, ecs_id(Velocity), {
        .value = 40
    });
    test_assert( ecs_has_pair(world, e2, ecs_id(Rel), ecs_id(Position)));

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    const Rel* tr = ecs_get_pair(world, e1, Rel, ecs_id(Position));
    test_assert(tr != NULL);
    test_int(tr->value, 11);

    tr = ecs_get_pair(world, e1, Rel, ecs_id(Velocity));
    test_assert(tr != NULL);
    test_int(tr->value, 21);

    tr = ecs_get_pair(world, e2, Rel, ecs_id(Position));
    test_assert(tr != NULL);
    test_int(tr->value, 31);

    tr = ecs_get_pair(world, e2, Rel, ecs_id(Velocity));
    test_assert(tr != NULL);
    test_int(tr->value, 41); 

    test_int(ctx.count, 4);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, ProcessPairs);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e1);
    test_int(ctx.e[3], e2);

    ecs_entity_t c = ctx.c[0][0];
    ecs_entity_t hi = ECS_PAIR_FIRST(c);
    ecs_entity_t lo = ECS_PAIR_SECOND(c);
    test_int(hi, ecs_id(Rel));
    test_int(lo, ecs_id(Position));

    c = ctx.c[1][0];
    hi = ECS_PAIR_FIRST(c);
    lo = ECS_PAIR_SECOND(c);
    test_int(hi, ecs_id(Rel));
    test_int(lo, ecs_id(Velocity));

    test_int(ctx.s[0][0], 0);
    test_int(ctx.s[1][0], 0);
    test_int(ctx.s[2][0], 0);
    test_int(ctx.s[3][0], 0);

    ecs_fini(world);
}

void Pairs_add_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rel);

    ecs_entity_t e1 = ecs_new(world, 0);
    test_assert(e1 != 0);

    ecs_add_pair(world, e1, ecs_id(Rel), ecs_id(Position));
    test_assert( ecs_has_pair(world, e1, ecs_id(Rel), ecs_id(Position)));

    ecs_fini(world);
}

void Pairs_remove_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rel);

    ecs_entity_t e1 = ecs_set_pair(world, 0, Rel, ecs_id(Position), {
        .value = 10
    });  
    test_assert(e1 != 0);
    test_assert( ecs_has_pair(world, e1, ecs_id(Rel), ecs_id(Position)));

    ecs_remove_pair(world, e1, ecs_id(Rel), ecs_id(Position));
    test_assert( !ecs_has_pair(world, e1, ecs_id(Rel), ecs_id(Position)));

    ecs_fini(world);
}

void ProcessPairTags(ecs_iter_t *it) {
    probe_iter(it);
}

void Pairs_add_tag_pair_for_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag1);
    ECS_TAG(world, Tag2);
    ECS_TAG(world, Rel);

    ECS_SYSTEM(world, ProcessPairTags, EcsOnUpdate, (Rel, *));

    /* Ensure that pair is matched against different components */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_add_pair(world, e1, Rel, Tag1);
    test_assert(e1 != 0);
    test_assert( ecs_has_pair(world, e1, Rel, Tag1));

    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_add_pair(world, e2, Rel, Tag2);
    test_assert(e2 != 0);
    test_assert( ecs_has_pair(world, e2, Rel, Tag2));

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, ProcessPairTags);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);

    ecs_entity_t c = ctx.c[0][0];

    ecs_entity_t hi = ECS_PAIR_FIRST(c);
    ecs_entity_t lo = ECS_PAIR_SECOND(c);
    test_int(hi, Rel);
    test_int(lo, Tag1);

    c = ctx.c[1][0];
    hi = ECS_PAIR_FIRST(c);
    lo = ECS_PAIR_SECOND(c);
    test_int(hi, Rel);
    test_int(lo, Tag2);

    test_int(ctx.s[0][0], 0);
    test_int(ctx.s[1][0], 0);

    ecs_fini(world);
}

void ProcessValuePairs(ecs_iter_t *it) {
    /* Strictly speaking this can be either Position or Velocity, but they have
     * the same layout. */
    Position *p = ecs_field(it, Position, 1);

    probe_iter(it);

    if (p) {
        int32_t i;
        for (i = 0; i < it->count; i ++) {
            p[i].x += 10;
            p[i].y += 20;
        }
    }
}

void Pairs_add_tag_pair_for_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Rel);

    ECS_SYSTEM(world, ProcessValuePairs, EcsOnUpdate, (Rel, *));

    ecs_entity_t e1 = ecs_set_pair_object(world, 0, Rel, Position, {
        .x = 1,
        .y = 2
    });
    test_assert( ecs_has_pair(world, e1, Rel, ecs_id(Position)));

    ecs_entity_t e2 = ecs_set_pair_object(world, 0, Rel, Velocity, {
        .x = 3,
        .y = 4
    });
    test_assert( ecs_has_pair(world, e2, Rel, ecs_id(Velocity)));

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    const Position* tr_p = ecs_get_pair_object(world, e1, Rel, Position);
    test_assert(tr_p != NULL);
    test_int(tr_p->x, 11);
    test_int(tr_p->y, 22);

    const Velocity* tr_v = ecs_get_pair_object(world, e2, Rel, Velocity);
    test_assert(tr_v != NULL);
    test_int(tr_v->x, 13);
    test_int(tr_v->y, 24);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, ProcessValuePairs);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);

    ecs_entity_t c = ctx.c[0][0];
    ecs_entity_t hi = ECS_PAIR_FIRST(c);
    ecs_entity_t lo = ECS_PAIR_SECOND(c);
    test_int(hi, Rel);
    test_int(lo, ecs_id(Position));

    c = ctx.c[1][0];
    hi = ECS_PAIR_FIRST(c);
    lo = ECS_PAIR_SECOND(c);
    test_int(hi, Rel);
    test_int(lo, ecs_id(Velocity));

    test_int(ctx.s[0][0], 0);
    test_int(ctx.s[1][0], 0);

    ecs_fini(world);
}

void ProcessTwoPairs(ecs_iter_t *it) {
    RelA *tr_a = ecs_field(it, RelA, 1);
    RelB *tr_b = ecs_field(it, RelB, 2);

    probe_iter(it);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        tr_a[i].value ++;
        tr_b[i].value ++;
    }
}

void Pairs_query_2_pairs() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, RelA);
    ECS_COMPONENT(world, RelB);

    ECS_SYSTEM(world, ProcessTwoPairs, EcsOnUpdate, (RelA, *), (RelB, *));

    /* Create entity with both RelA and RelB */
    ecs_entity_t e1 = ecs_set_pair(world, 0, RelA, ecs_id(Position), {
        .value = 1
    });

    ecs_set_pair(world, e1, RelB, ecs_id(Position), {
        .value = 2
    });

    test_assert( ecs_has_pair(world, e1, ecs_id(RelA), ecs_id(Position)));
    test_assert( ecs_has_pair(world, e1, ecs_id(RelB), ecs_id(Position)));

    /* Create entity with only RelA. Should not be matched with system */
    ecs_entity_t e2 = ecs_set_pair(world, 0, RelA, ecs_id(Position), {
        .value = 3
    });
    test_assert( ecs_has_pair(world, e2, ecs_id(RelA), ecs_id(Position)));

    /* Run system */
    Probe ctx = {0};
    ecs_set_context(world, &ctx);
    ecs_progress(world, 0);

    const RelA *tr_a = ecs_get_pair(world, e1, RelA, ecs_id(Position));
    test_int(tr_a->value, 2);

    const RelB *tr_b = ecs_get_pair(world, e1, RelB, ecs_id(Position));
    test_int(tr_b->value, 3);

    tr_a = ecs_get_pair(world, e2, RelA, ecs_id(Position));
    test_int(tr_a->value, 3);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, ProcessTwoPairs);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);

    ecs_entity_t c = ctx.c[0][0];
    ecs_entity_t hi = ECS_PAIR_FIRST(c);
    ecs_entity_t lo = ECS_PAIR_SECOND(c);
    test_int(hi, ecs_id(RelA));
    test_int(lo, ecs_id(Position));

    c = ctx.c[0][1];
    hi = ECS_PAIR_FIRST(c);
    lo = ECS_PAIR_SECOND(c);
    test_int(hi, ecs_id(RelB));
    test_int(lo, ecs_id(Position));    

    ecs_fini(world);
}

void Pairs_query_2_pairs_2_instances_per_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, RelA);
    ECS_COMPONENT(world, RelB);

    ECS_SYSTEM(world, ProcessTwoPairs, EcsOnUpdate, (RelA, *), (RelB, *));

    /* Create entity with both RelA and RelB, applied to two components*/
    ecs_entity_t e1 = ecs_set_pair(world, 0, RelA, ecs_id(Position), {
        .value = 1
    });

    ecs_set_pair(world, e1, RelB, ecs_id(Position), {
        .value = 2
    });

    ecs_set_pair(world, e1, RelA, ecs_id(Velocity), {
        .value = 3
    });

    ecs_set_pair(world, e1, RelB, ecs_id(Velocity), {
        .value = 4
    });        

    test_assert( ecs_has_pair(world, e1, ecs_id(RelA), ecs_id(Position)));
    test_assert( ecs_has_pair(world, e1, ecs_id(RelB), ecs_id(Position)));
    test_assert( ecs_has_pair(world, e1, ecs_id(RelA), ecs_id(Velocity)));
    test_assert( ecs_has_pair(world, e1, ecs_id(RelB), ecs_id(Velocity)));

    /* Run system */
    Probe ctx = {0};
    ecs_set_context(world, &ctx);
    ecs_progress(world, 0);

    const RelA *tr_a = ecs_get_pair(world, e1, RelA, ecs_id(Position));
    test_int(tr_a->value, 3);

    tr_a = ecs_get_pair(world, e1, RelA, ecs_id(Velocity));
    test_int(tr_a->value, 5);

    const RelB *tr_b = ecs_get_pair(world, e1, RelB, ecs_id(Position));
    test_int(tr_b->value, 4);

    tr_b = ecs_get_pair(world, e1, RelB, ecs_id(Velocity));
    test_int(tr_b->value, 6);

    test_int(ctx.count, 4);
    test_int(ctx.invoked, 4);
    test_int(ctx.system, ProcessTwoPairs);
    test_int(ctx.term_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e1);
    test_int(ctx.e[2], e1);
    test_int(ctx.e[3], e1);

    ecs_fini(world);
}

void Pairs_override_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Rel);

    ecs_entity_t base = ecs_new(world, 0);
    ecs_set_pair(world, base, Rel, ecs_id(Position), {.value = 10});

    ecs_entity_t instance = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(ecs_has_pair(world, instance, ecs_id(Rel), ecs_id(Position)));

    const Rel *t = ecs_get_pair(world, instance, Rel, ecs_id(Position));
    test_assert(t != NULL);
    test_int(t->value, 10);

    const Rel *t_2 = ecs_get_pair(world, base, Rel, ecs_id(Position));
    test_assert(t_2 != NULL);
    test_assert(t == t_2);

    ecs_add_pair(world, instance, ecs_id(Rel), ecs_id(Position));
    t = ecs_get_pair(world, instance, Rel, ecs_id(Position));
    test_assert(t != NULL);
    test_int(t->value, 10);
    test_assert(t != t_2);

    ecs_fini(world);
}

void Pairs_override_tag_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Rel);

    ecs_entity_t base = ecs_new(world, 0);
    ecs_set_pair_object(world, base, Rel, Position, {.x = 10, .y = 20});

    ecs_entity_t instance = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(ecs_has_pair(world, instance, Rel, ecs_id(Position)));

    const Position *t = ecs_get_pair_object(world, instance, Rel, Position);
    test_assert(t != NULL);
    test_int(t->x, 10);
    test_int(t->y, 20);

    const Position *t_2 = ecs_get_pair_object(world, base, Rel, Position);
    test_assert(t_2 != NULL);
    test_assert(t == t_2);

    ecs_add_pair(world, instance, Rel, ecs_id(Position));
    t = ecs_get_pair_object(world, instance, Rel, Position);
    test_assert(t != NULL);
    test_int(t->x, 10);
    test_int(t->y, 20);
    test_assert(t != t_2);

    ecs_fini(world);
}

static
void PairTrigger(ecs_iter_t *it) {
    probe_iter(it);
}

void Pairs_on_add_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rel);

    ECS_OBSERVER(world, PairTrigger, EcsOnAdd, (Rel, *));

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_add_pair(world, e, ecs_id(Rel), ecs_id(Position));

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, PairTrigger);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);

    ecs_entity_t c = ctx.c[0][0];
    ecs_entity_t hi = ECS_PAIR_FIRST(c);
    ecs_entity_t lo = ECS_PAIR_SECOND(c);
    test_int(hi, ecs_id(Rel));
    test_int(lo, ecs_id(Position));  

    ecs_os_zeromem(&ctx);
    ecs_add_pair(world, e, ecs_id(Rel), ecs_id(Velocity));

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, PairTrigger);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);

    c = ctx.c[0][0];
    hi = ECS_PAIR_FIRST(c);
    lo = ECS_PAIR_SECOND(c);
    test_int(hi, ecs_id(Rel));
    test_int(lo, ecs_id(Velocity));

    ecs_fini(world);
}

void Pairs_on_add_pair_tag() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Rel);

    ECS_OBSERVER(world, PairTrigger, EcsOnAdd, (Rel, Position));

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_add_pair(world, e, Rel, ecs_id(Position));

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, PairTrigger);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);

    ecs_entity_t c = ctx.c[0][0];
    ecs_entity_t hi = ECS_PAIR_FIRST(c);
    ecs_entity_t lo = ECS_PAIR_SECOND(c);
    test_int(hi, Rel);
    test_int(lo, ecs_id(Position));  

    ecs_fini(world);
}

void Pairs_on_remove_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rel);

    ECS_OBSERVER(world, PairTrigger, EcsOnRemove, (Rel, *));

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_add_pair(world, e, ecs_id(Rel), ecs_id(Position));
    ecs_add_pair(world, e, ecs_id(Rel), ecs_id(Velocity));
    test_int(ctx.count, 0);

    ecs_remove_pair(world, e, ecs_id(Rel), ecs_id(Position));
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, PairTrigger);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);

    ecs_entity_t c = ctx.c[0][0];
    ecs_entity_t hi = ECS_PAIR_FIRST(c);
    ecs_entity_t lo = ECS_PAIR_SECOND(c);
    test_int(hi, ecs_id(Rel));
    test_int(lo, ecs_id(Position));  

    ecs_os_zeromem(&ctx);
    ecs_remove_pair(world, e, ecs_id(Rel), ecs_id(Velocity));
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, PairTrigger);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);

    c = ctx.c[0][0];
    hi = ECS_PAIR_FIRST(c);
    lo = ECS_PAIR_SECOND(c);
    test_int(hi, ecs_id(Rel));
    test_int(lo, ecs_id(Velocity));

    ecs_fini(world);
}

void Pairs_on_remove_pair_tag() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Rel);

    ECS_OBSERVER(world, PairTrigger, EcsOnRemove, (Rel, Position));

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_add_pair(world, e, Rel, ecs_id(Position));
    test_int(ctx.count, 0);

    ecs_remove_pair(world, e, Rel, ecs_id(Position));
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, PairTrigger);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);

    ecs_entity_t c = ctx.c[0][0];
    ecs_entity_t hi = ECS_PAIR_FIRST(c);
    ecs_entity_t lo = ECS_PAIR_SECOND(c);
    test_int(hi, Rel);
    test_int(lo, ecs_id(Position));  

    ecs_fini(world);
}

void Pairs_on_remove_pair_on_delete() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rel);

    ECS_OBSERVER(world, PairTrigger, EcsOnRemove, (Rel, *));

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_add_pair(world, e, ecs_id(Rel), ecs_id(Position));
    ecs_add_pair(world, e, ecs_id(Rel), ecs_id(Velocity));
    test_int(ctx.count, 0);

    ecs_delete(world, e);
    test_int(ctx.count, 2);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, PairTrigger);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);

    ecs_entity_t c = ctx.c[0][0];
    ecs_entity_t hi = ECS_PAIR_FIRST(c);
    ecs_entity_t lo = ECS_PAIR_SECOND(c);
    test_int(hi, ecs_id(Rel));
    test_int(lo, ecs_id(Position));  

    test_int(ctx.e[1], e);

    c = ctx.c[1][0];
    hi = ECS_PAIR_FIRST(c);
    lo = ECS_PAIR_SECOND(c);
    test_int(hi, ecs_id(Rel));
    test_int(lo, ecs_id(Velocity));

    ecs_fini(world);
}

static
void PairTriggerPosition(ecs_iter_t *it) {
    PairTrigger(it);
}

static
void PairTriggerVelocity(ecs_iter_t *it) {
    PairTrigger(it);
}

void Pairs_on_remove_pair_tag_on_delete() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Rel);

    ECS_OBSERVER(world, PairTriggerPosition, EcsOnRemove, (Rel, Position));
    ECS_OBSERVER(world, PairTriggerVelocity, EcsOnRemove, (Rel, Velocity));

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_add_pair(world, e, Rel, ecs_id(Position));
    ecs_add_pair(world, e, Rel, ecs_id(Velocity));
    test_int(ctx.count, 0);

    ecs_delete(world, e);
    test_int(ctx.count, 2);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, PairTriggerVelocity);
    test_int(ctx.term_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);

    ecs_entity_t c = ctx.c[0][0];
    ecs_entity_t hi = ECS_PAIR_FIRST(c);
    ecs_entity_t lo = ECS_PAIR_SECOND(c);
    test_int(hi, Rel);
    test_int(lo, ecs_id(Position));  

    test_int(ctx.e[1], e);

    c = ctx.c[1][0];
    hi = ECS_PAIR_FIRST(c);
    lo = ECS_PAIR_SECOND(c);
    test_int(hi, Rel);
    test_int(lo, ecs_id(Velocity));

    ecs_fini(world);
}

void Pairs_pair_w_component_query() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Rel);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_add_pair(world, e, ecs_id(Rel), ecs_id(Position));

    ecs_query_t *q = ecs_query_new(world, "(Rel, Position)");
    
    int32_t count = 0;
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        Rel *t = ecs_field(&it, Rel, 1);
        test_assert(t != NULL);

        int i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] == e);
        }
        count += it.count;
    }

    test_assert(count == 1);

    ecs_fini(world);
}

void Pairs_query_pair_or_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Rel);

    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_add_pair(world, e1, Rel, ecs_id(Position));
    ecs_entity_t e2 = ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "(Rel, Position) || Position");
    
    int32_t count = 0;
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        Position *t = ecs_field(&it, Position, 1);
        test_assert(t != NULL);

        int i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] == e1 || it.entities[i] == e2);
        }
        count += it.count;
    }

    test_assert(count == 2);

    ecs_fini(world);
}

void Pairs_query_pair_or_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_add_pair(world, e1, RelA, ecs_id(Position));
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_add_pair(world, e2, RelB, ecs_id(Position));

    ecs_query_t *q = ecs_query_new(world, "(RelA, Position) || (RelB, Position)");
    
    int32_t count = 0;
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        Position *t = ecs_field(&it, Position, 1);
        test_assert(t != NULL);

        int i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] == e1 || it.entities[i] == e2);
        }
        count += it.count;
    }

    test_int(count, 2);

    ecs_fini(world);
}

void Pairs_query_not_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Rel);

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_add_pair(world, e2, Rel, ecs_id(Position));

    ecs_query_t *q = ecs_query_new(world, "!(Rel, Position), Position");
    
    int32_t count = 0;
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        Position *t = ecs_field(&it, Position, 1);
        test_assert(t == NULL);
        Position *p = ecs_field(&it, Position, 2);
        test_assert(p != NULL);

        int i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] == e1);
        }
        count += it.count;
    }

    test_assert(count == 1);

    ecs_fini(world);
}

void Pairs_get_typeid_w_recycled_rel() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t o = ecs_new_id(world);
    test_assert(o != 0);

    ecs_entity_t dummy = ecs_new_id(world);
    ecs_delete(world, dummy); // force recycle

    // don't use ECS_COMPONENT, because it will try to get low ids first
    ecs_entity_t pos = ecs_set(world, 0, EcsComponent, {1, 1});
    test_assert(pos != 0);
    test_assert((uint32_t)pos != pos); // ensure recycled

    ecs_id_t pair = ecs_pair(pos, o);
    ecs_entity_t tid = ecs_get_typeid(world, pair);
    test_assert(tid != 0);
    test_assert(tid == pos);

    ecs_fini(world);
}

void Pairs_get_typeid_w_recycled_obj() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r = ecs_new_id(world);
    test_assert(r != 0);

    ecs_entity_t dummy = ecs_new_id(world);
    ecs_delete(world, dummy); // force recycle

    // don't use ECS_COMPONENT, because it will try to get low ids first
    ecs_entity_t pos = ecs_set(world, 0, EcsComponent, {1, 1});
    test_assert(pos != 0);
    test_assert((uint32_t)pos != pos); // ensure recycled

    ecs_id_t pair = ecs_pair(r, pos);
    ecs_entity_t tid = ecs_get_typeid(world, pair);
    test_assert(tid != 0);
    test_assert(tid == pos);

    ecs_fini(world);
}

void Pairs_id_str_w_recycled_rel() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t o = ecs_set_name(world, 0, "o");
    test_assert(o != 0);

    ecs_entity_t dummy = ecs_new_id(world);
    ecs_delete(world, dummy); // force recycle

    ecs_entity_t r = ecs_set_name(world, 0, "r");
    test_assert(r != 0);
    test_assert((uint32_t)r != r); // ensure recycled

    char *str = ecs_id_str(world, ecs_pair(r, o));
    test_str(str, "(r,o)");
    ecs_os_free(str);

    ecs_fini(world);
}

void Pairs_id_str_w_recycled_obj() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r = ecs_set_name(world, 0, "r");
    test_assert(r != 0);

    ecs_entity_t dummy = ecs_new_id(world);
    ecs_delete(world, dummy); // force recycle

    ecs_entity_t o = ecs_set_name(world, 0, "o");
    test_assert(o != 0);
    test_assert((uint32_t)o != o); // ensure recycled

    char *str = ecs_id_str(world, ecs_pair(r, o));
    test_str(str, "(r,o)");
    ecs_os_free(str);

    ecs_fini(world);
}

void Pairs_set_object_w_zero_sized_rel_comp() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t r = ecs_set(world, 0, EcsComponent, {0});

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_set_pair_object(world, 0, r, Position, {10, 20});
    
    const Position *p = ecs_get_pair_object(world, e, r, Position);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);

    ecs_fini(world);
}

void Pairs_dsl_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Rel_2);
    ECS_TAG(world, Obj);
    ECS_TAG(world, Obj_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "(Rel, Obj)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj)} });
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj_2)} });
    test_assert(e2 != 0);

    ecs_entity_t e3 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel_2, Obj)} });
    test_assert(e3 != 0);

    ecs_entity_t e4 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel_2, Obj_2)} });
    test_assert(e4 != 0);

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, Obj));

    test_bool(ecs_query_next(&it), false); 

    ecs_fini(world);
}

void Pairs_dsl_pair_w_pred_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Rel_2);
    ECS_TAG(world, Obj);
    ECS_TAG(world, Obj_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "(*, Obj)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj)} });
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj_2)} });
    test_assert(e2 != 0);

    ecs_entity_t e3 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel_2, Obj)} });
    test_assert(e3 != 0);

    ecs_entity_t e4 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel_2, Obj_2)} });
    test_assert(e4 != 0);    

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, Obj));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel_2, Obj));

    test_bool(ecs_query_next(&it), false); 

    ecs_fini(world);
}

void Pairs_dsl_pair_w_obj_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Rel_2);
    ECS_TAG(world, Obj);
    ECS_TAG(world, Obj_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "(Rel_2, *)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj)} });
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj_2)} });
    test_assert(e2 != 0);

    ecs_entity_t e3 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel_2, Obj)} });
    test_assert(e3 != 0);

    ecs_entity_t e4 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel_2, Obj_2)} });
    test_assert(e4 != 0);    

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel_2, Obj));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e4);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel_2, Obj_2));

    test_bool(ecs_query_next(&it), false); 

    ecs_fini(world);
}

void Pairs_dsl_pair_w_both_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Rel_2);
    ECS_TAG(world, Obj);
    ECS_TAG(world, Obj_2);

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "(*, *), Tag" // add Tag or we'd match builtin entities
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj), Tag} });
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj_2), Tag} });
    test_assert(e2 != 0);

    ecs_entity_t e3 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel_2, Obj), Tag} });
    test_assert(e3 != 0);

    ecs_entity_t e4 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel_2, Obj_2), Tag} });
    test_assert(e4 != 0);    

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);

    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, Obj));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e2);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, Obj_2));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel_2, Obj));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e4);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel_2, Obj_2));

    test_bool(ecs_query_next(&it), false); 

    ecs_fini(world);
}

void Pairs_dsl_pair_w_explicit_subj_this() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Rel_2);
    ECS_TAG(world, Obj);
    ECS_TAG(world, Obj_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Rel($This, Obj)"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj)} });
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj_2)} });
    test_assert(e2 != 0);

    ecs_entity_t e3 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel_2, Obj)} });
    test_assert(e3 != 0);

    ecs_entity_t e4 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel_2, Obj_2)} });
    test_assert(e4 != 0);

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, Obj));

    test_bool(ecs_query_next(&it), false); 

    ecs_fini(world);
}

void Pairs_dsl_pair_w_explicit_subj() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Rel_2);
    ECS_TAG(world, Obj);
    ECS_TAG(world, Obj_2);

    ECS_TAG(world, Tag);

    ecs_entity_t Subj = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Subj", .add = {ecs_pair(Rel, Obj)} });
    test_assert(Subj != 0);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Rel(Subj, Obj), Tag"
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj)} });
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj_2)} });
    test_assert(e2 != 0);

    ecs_entity_t e3 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel_2, Obj)} });
    test_assert(e3 != 0);

    ecs_entity_t e4 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel_2, Obj_2), Tag} });
    test_assert(e4 != 0);

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e4);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, Obj));
    test_int(ecs_field_src(&it, 1), Subj);

    test_bool(ecs_query_next(&it), false); 

    ecs_fini(world);
}

void Pairs_api_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Rel_2);
    ECS_TAG(world, Obj);
    ECS_TAG(world, Obj_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.terms = {{ecs_pair(Rel, Obj)}}
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj)} });
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj_2)} });
    test_assert(e2 != 0);

    ecs_entity_t e3 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel_2, Obj)} });
    test_assert(e3 != 0);

    ecs_entity_t e4 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel_2, Obj_2)} });
    test_assert(e4 != 0);

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, Obj));

    test_bool(ecs_query_next(&it), false); 

    ecs_fini(world);
}

void Pairs_api_pair_w_pred_wildcard() {
   ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Rel_2);
    ECS_TAG(world, Obj);
    ECS_TAG(world, Obj_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.terms = {{ecs_pair(EcsWildcard, Obj)}}
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj)} });
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj_2)} });
    test_assert(e2 != 0);

    ecs_entity_t e3 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel_2, Obj)} });
    test_assert(e3 != 0);

    ecs_entity_t e4 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel_2, Obj_2)} });
    test_assert(e4 != 0);    

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, Obj));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel_2, Obj));

    test_bool(ecs_query_next(&it), false); 

    ecs_fini(world);
}

void Pairs_api_pair_w_obj_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Rel_2);
    ECS_TAG(world, Obj);
    ECS_TAG(world, Obj_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.terms = {{ecs_pair(Rel_2, EcsWildcard)}}
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj)} });
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj_2)} });
    test_assert(e2 != 0);

    ecs_entity_t e3 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel_2, Obj)} });
    test_assert(e3 != 0);

    ecs_entity_t e4 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel_2, Obj_2)} });
    test_assert(e4 != 0);    

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel_2, Obj));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e4);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel_2, Obj_2));

    test_bool(ecs_query_next(&it), false); 

    ecs_fini(world);
}

void Pairs_api_pair_w_both_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Rel_2);
    ECS_TAG(world, Obj);
    ECS_TAG(world, Obj_2);

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        // add Tag or we'd match builtin entities
        .filter.terms = {{ecs_pair(EcsWildcard, EcsWildcard)}, {Tag}} 
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj), Tag} });
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj_2), Tag} });
    test_assert(e2 != 0);

    ecs_entity_t e3 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel_2, Obj), Tag} });
    test_assert(e3 != 0);

    ecs_entity_t e4 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel_2, Obj_2), Tag} });
    test_assert(e4 != 0);    

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, Obj));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e2);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, Obj_2));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e3);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel_2, Obj));

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e4);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel_2, Obj_2));

    test_bool(ecs_query_next(&it), false); 

    ecs_fini(world);
}

void Pairs_api_pair_w_explicit_subj_this() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Rel_2);
    ECS_TAG(world, Obj);
    ECS_TAG(world, Obj_2);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.terms = {
            { ecs_pair(Rel, Obj), .src.id = EcsThis }
        }
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj)} });
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj_2)} });
    test_assert(e2 != 0);

    ecs_entity_t e3 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel_2, Obj)} });
    test_assert(e3 != 0);

    ecs_entity_t e4 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel_2, Obj_2)} });
    test_assert(e4 != 0);

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e1);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, Obj));

    test_bool(ecs_query_next(&it), false); 

    ecs_fini(world);
}

void Pairs_api_pair_w_explicit_subj() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Rel_2);
    ECS_TAG(world, Obj);
    ECS_TAG(world, Obj_2);

    ECS_TAG(world, Tag);

    ecs_entity_t Subj = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "Subj", .add = {ecs_pair(Rel, Obj)} });
    test_assert(Subj != 0);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.terms = {{ecs_pair(Rel, Obj), .src.id = Subj}, {Tag}}
    });

    test_assert(q != NULL);

    ecs_entity_t e1 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj)} });
    test_assert(e1 != 0);

    ecs_entity_t e2 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel, Obj_2)} });
    test_assert(e2 != 0);

    ecs_entity_t e3 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel_2, Obj)} });
    test_assert(e3 != 0);

    ecs_entity_t e4 = ecs_entity_init(world, &(ecs_entity_desc_t){
        .add = {ecs_pair(Rel_2, Obj_2), Tag} });
    test_assert(e4 != 0);

    ecs_iter_t it = ecs_query_iter(world, q);

    test_bool(ecs_query_next(&it), true);
    test_int(it.count, 1);
    test_int(it.entities[0], e4);
    test_int(ecs_field_id(&it, 1), ecs_pair(Rel, Obj));
    test_int(ecs_field_src(&it, 1), Subj);

    test_bool(ecs_query_next(&it), false); 

    ecs_fini(world);
}

void Pairs_typeid_from_tag() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t tag = ecs_new_id(world);
    test_assert(tag != 0);

    ecs_entity_t id = ecs_get_typeid(world, tag);
    test_assert(id == 0);

    ecs_fini(world);
}

void Pairs_typeid_from_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Rel);
    ecs_id_t rel_id = ecs_id(Rel);
    test_assert(rel_id != 0);

    ecs_entity_t id = ecs_get_typeid(world, rel_id);
    test_assert(id != 0);
    test_assert(id == rel_id);

    ecs_fini(world);
}

void Pairs_typeid_from_pair() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t rel_id = ecs_new_id(world);
    test_assert(rel_id != 0);

    ecs_entity_t obj_id = ecs_new_id(world);
    test_assert(obj_id != 0);

    ecs_id_t pair_id = ecs_pair(rel_id, obj_id);

    ecs_entity_t id = ecs_get_typeid(world, pair_id);
    test_assert(id == 0);

    ecs_fini(world);
}

void Pairs_typeid_from_pair_w_rel_type() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Rel);
    ecs_id_t rel_id = ecs_id(Rel);
    test_assert(rel_id != 0);

    ecs_entity_t obj_id = ecs_new_id(world);
    test_assert(obj_id != 0);

    ecs_id_t pair_id = ecs_pair(rel_id, obj_id);

    ecs_entity_t id = ecs_get_typeid(world, pair_id);
    test_assert(id != 0);
    test_assert(id == rel_id);

    ecs_fini(world);
}

void Pairs_typeid_from_pair_w_obj_type() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Obj);
    ecs_id_t obj_id = ecs_id(Obj);
    test_assert(obj_id != 0);

    ecs_entity_t rel_id = ecs_new_id(world);
    test_assert(rel_id != 0);

    ecs_id_t pair_id = ecs_pair(rel_id, obj_id);

    ecs_entity_t id = ecs_get_typeid(world, pair_id);
    test_assert(id != 0);
    test_assert(id == obj_id);

    ecs_fini(world);
}

void Pairs_typeid_from_pair_w_rel_obj_type() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Rel);
    ecs_id_t rel_id = ecs_id(Rel);
    test_assert(rel_id != 0);

    ECS_COMPONENT(world, Obj);
    ecs_id_t obj_id = ecs_id(Obj);
    test_assert(obj_id != 0);

    ecs_id_t pair_id = ecs_pair(rel_id, obj_id);

    ecs_entity_t id = ecs_get_typeid(world, pair_id);
    test_assert(id != 0);
    test_assert(id == rel_id);

    ecs_fini(world);
}

void Pairs_typeid_from_pair_w_rel_0_obj_type() {
    ecs_world_t *world = ecs_mini();

    ecs_id_t rel_id = ecs_set(world, 0, EcsComponent, {0});
    test_assert(rel_id != 0);

    ECS_COMPONENT(world, Obj);
    ecs_id_t obj_id = ecs_id(Obj);
    test_assert(obj_id != 0);

    ecs_id_t pair_id = ecs_pair(rel_id, obj_id);

    ecs_entity_t id = ecs_get_typeid(world, pair_id);
    test_assert(id != 0);
    test_assert(id == obj_id);

    ecs_fini(world);
}

void Pairs_typeid_from_pair_w_rel_obj_0_type() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Rel);
    ecs_id_t rel_id = ecs_id(Rel);
    test_assert(rel_id != 0);

    ecs_id_t obj_id = ecs_set(world, 0, EcsComponent, {0});
    test_assert(obj_id != 0);

    ecs_id_t pair_id = ecs_pair(rel_id, obj_id);

    ecs_entity_t id = ecs_get_typeid(world, pair_id);
    test_assert(id != 0);
    test_assert(id == rel_id);

    ecs_fini(world);
}

void Pairs_typeid_from_pair_w_rel_0_obj_0_type() {
    ecs_world_t *world = ecs_mini();

    ecs_id_t rel_id = ecs_set(world, 0, EcsComponent, {0});
    test_assert(rel_id != 0);

    ecs_id_t obj_id = ecs_set(world, 0, EcsComponent, {0});
    test_assert(obj_id != 0);

    ecs_id_t pair_id = ecs_pair(rel_id, obj_id);

    ecs_entity_t id = ecs_get_typeid(world, pair_id);
    test_assert(id == 0);

    ecs_fini(world);
}

void Pairs_typeid_from_pair_with_0_obj() {
    ecs_world_t *world = ecs_mini();

    ecs_id_t pair_id = ecs_pair(EcsChildOf, 0);

    ecs_entity_t id = ecs_get_typeid(world, pair_id);
    test_assert(id == 0);

    ecs_fini(world);
}

void Pairs_typeid_from_pair_w_override() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Rel);
    ecs_id_t rel_id = ecs_id(Rel);
    test_assert(rel_id != 0);

    ecs_entity_t obj_id = ecs_new_id(world);
    test_assert(obj_id != 0);

    ecs_id_t pair_id = ECS_OVERRIDE | ecs_pair(rel_id, obj_id);

    ecs_entity_t id = ecs_get_typeid(world, pair_id);
    test_assert(id == 0);

    ecs_fini(world);
}

void Pairs_typeid_from_pair_w_toggle() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Rel);
    ecs_id_t rel_id = ecs_id(Rel);
    test_assert(rel_id != 0);

    ecs_entity_t obj_id = ecs_new_id(world);
    test_assert(obj_id != 0);

    ecs_id_t pair_id = ECS_TOGGLE | ecs_pair(rel_id, obj_id);

    ecs_entity_t id = ecs_get_typeid(world, pair_id);
    test_assert(id == 0);

    ecs_fini(world);
}

void Pairs_tag_pair_w_rel_comp() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Obj);

    ecs_add_id(world, ecs_id(Position), EcsTag);

    ecs_entity_t e = ecs_new_id(world);
    ecs_id_t pair_id = ecs_pair(ecs_id(Position), Obj);

    ecs_add_id(world, e, pair_id);

    ecs_entity_t id = ecs_get_typeid(world, pair_id);
    test_assert(id == 0);

    ecs_fini(world);
}

void Pairs_tag_pair_w_obj_comp() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_COMPONENT(world, Position);

    ecs_add_id(world, Rel, EcsTag);

    ecs_entity_t e = ecs_new_id(world);
    ecs_id_t pair_id = ecs_pair(Rel, ecs_id(Position));

    ecs_add_id(world, e, pair_id);

    ecs_entity_t id = ecs_get_typeid(world, pair_id);
    test_assert(id == 0);

    ecs_fini(world);
}

void Pairs_tag_pair_w_rel_obj_comp() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsTag);

    ecs_entity_t e = ecs_new_id(world);
    ecs_id_t pair_id = ecs_pair(ecs_id(Position), ecs_id(Velocity));

    ecs_add_id(world, e, pair_id);

    ecs_entity_t id = ecs_get_typeid(world, pair_id);
    test_assert(id == 0);

    ecs_fini(world);
}

void Pairs_get_tag_pair_w_rel_comp() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Obj);

    ecs_add_id(world, ecs_id(Position), EcsTag);

    ecs_entity_t e = ecs_new_id(world);
    ecs_id_t pair_id = ecs_pair(ecs_id(Position), Obj);

    ecs_add_id(world, e, pair_id);

    test_expect_abort();
    ecs_get_id(world, e, pair_id);
}

void Pairs_get_tag_pair_w_obj_comp() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsTag);

    ecs_entity_t e = ecs_new_id(world);
    ecs_id_t pair_id = ecs_pair(ecs_id(Position), ecs_id(Velocity));

    ecs_add_id(world, e, pair_id);

    test_expect_abort();
    ecs_get_id(world, e, pair_id);
}

void Pairs_get_tag_pair_w_rel_obj_comp() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_add_id(world, ecs_id(Position), EcsTag);

    ecs_entity_t e = ecs_new_id(world);
    ecs_id_t pair_id = ecs_pair(ecs_id(Position), ecs_id(Velocity));

    ecs_add_id(world, e, pair_id);

    test_expect_abort();
    ecs_get_id(world, e, pair_id);
}

void Pairs_tag_pair_w_childof_w_comp() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_id(world);
    ecs_id_t pair_id = ecs_pair(EcsChildOf, ecs_id(Position));

    ecs_add_id(world, e, pair_id);

    ecs_entity_t id = ecs_get_typeid(world, pair_id);
    test_assert(id == 0);

    ecs_fini(world);
}

void Pairs_tag_pair_w_isa_w_comp() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_new_id(world);
    ecs_id_t pair_id = ecs_pair(EcsIsA, ecs_id(Position));

    ecs_add_id(world, e, pair_id);

    ecs_entity_t id = ecs_get_typeid(world, pair_id);
    test_assert(id == 0);

    ecs_fini(world);
}

void Pairs_get_1_target() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_id(world);
    ecs_entity_t rel = ecs_new_id(world);
    ecs_entity_t obj = ecs_new_id(world);

    ecs_add_pair(world, e, rel, obj);
    test_assert(ecs_has_pair(world, e, rel, obj));
    test_assert(ecs_get_target(world, e, rel, 0) == obj);

    ecs_fini(world);
}

void Pairs_get_1_target_not_found() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_id(world);
    ecs_entity_t rel = ecs_new_id(world);

    test_assert(ecs_get_target(world, e, rel, 0) == 0);

    ecs_fini(world);
}

void Pairs_get_n_targets() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t e = ecs_new_id(world);
    ecs_entity_t rel = ecs_new_id(world);
    ecs_entity_t obj_1 = ecs_new_id(world);
    ecs_entity_t obj_2 = ecs_new_id(world);
    ecs_entity_t obj_3 = ecs_new_id(world);

    ecs_add_pair(world, e, rel, obj_1);
    ecs_add_pair(world, e, rel, obj_2);
    ecs_add_pair(world, e, rel, obj_3);
    test_assert(ecs_has_pair(world, e, rel, obj_1));
    test_assert(ecs_has_pair(world, e, rel, obj_2));
    test_assert(ecs_has_pair(world, e, rel, obj_3));
    test_assert(ecs_get_target(world, e, rel, 0) == obj_1);
    test_assert(ecs_get_target(world, e, rel, 1) == obj_2);
    test_assert(ecs_get_target(world, e, rel, 2) == obj_3);
    test_assert(ecs_get_target(world, e, rel, 3) == 0);

    ecs_fini(world);
}

void Pairs_get_target_from_base() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t rel = ecs_new_id(world);
    ecs_entity_t tgt = ecs_new_id(world);
    ecs_entity_t base = ecs_new_w_pair(world, rel, tgt);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);

    test_assert(ecs_get_target(world, base, rel, 0) == tgt);
    test_assert(ecs_get_target(world, inst, rel, 0) == tgt);

    ecs_fini(world);
}

void Pairs_get_target_from_2nd_base() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t rel = ecs_new_id(world);
    ecs_entity_t tgt = ecs_new_id(world);
    ecs_entity_t base_1 = ecs_new_id(world);
    ecs_entity_t base_2 = ecs_new_w_pair(world, rel, tgt);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base_1);
    ecs_add_pair(world, inst, EcsIsA, base_2);

    test_assert(ecs_get_target(world, base_1, rel, 0) == 0);
    test_assert(ecs_get_target(world, base_2, rel, 0) == tgt);
    test_assert(ecs_get_target(world, inst, rel, 0) == tgt);

    ecs_fini(world);
}

void Pairs_get_target_from_base_w_pair_on_instance() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t rel = ecs_new_id(world);
    ecs_entity_t tgt_a = ecs_new_id(world);
    ecs_entity_t tgt_b = ecs_new_id(world);
    ecs_entity_t base = ecs_new_w_pair(world, rel, tgt_b);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);
    ecs_add_pair(world, inst, rel, tgt_a);

    test_assert(ecs_get_target(world, base, rel, 0) == tgt_b);
    test_assert(ecs_get_target(world, inst, rel, 0) == tgt_a);
    test_assert(ecs_get_target(world, inst, rel, 1) == tgt_b);

    ecs_fini(world);
}

void Pairs_get_target_for_id_from_self() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t base = ecs_new(world, Tag);
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);
    ecs_add_id(world, e, Tag);

    ecs_entity_t result = ecs_get_target_for_id(world, e, EcsIsA, Tag);
    test_assert(result != 0);
    test_assert(result == e);

    ecs_fini(world);
}

void Pairs_get_target_for_id_from_base() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t base = ecs_new(world, Tag);
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);

    ecs_entity_t result = ecs_get_target_for_id(world, e, EcsIsA, Tag);
    test_assert(result != 0);
    test_assert(result == base);

    ecs_fini(world);
}

void Pairs_get_target_for_id_from_nested_base() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t base = ecs_new(world, Tag);
    ecs_entity_t base_2 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base_2);

    ecs_entity_t result = ecs_get_target_for_id(world, e, EcsIsA, Tag);
    test_assert(result != 0);
    test_assert(result == base);

    ecs_fini(world);
}

void Pairs_get_target_for_id_not_found() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t base = ecs_new_id(world);
    ecs_entity_t base_2 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base_2);

    ecs_entity_t result = ecs_get_target_for_id(world, e, EcsIsA, Tag);
    test_assert(result == 0);

    ecs_fini(world);
}

void Pairs_get_target_for_wildcard_from_self() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    ecs_id_t pair = ecs_pair(Rel, Obj);
    ecs_entity_t base = ecs_new_w_id(world, pair);
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);
    ecs_add_id(world, e, pair);

    ecs_entity_t result = ecs_get_target_for_id(world, e, EcsIsA, 
        ecs_pair(Rel, EcsWildcard));
    test_assert(result != 0);
    test_assert(result == e);

    ecs_fini(world);
}

void Pairs_get_target_for_wildcard_from_base() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    ecs_id_t pair = ecs_pair(Rel, Obj);
    ecs_entity_t base = ecs_new_w_id(world, pair);
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);

    ecs_entity_t result = ecs_get_target_for_id(world, e, EcsIsA, 
        ecs_pair(Rel, EcsWildcard));
    test_assert(result != 0);
    test_assert(result == base);

    ecs_fini(world);
}

void Pairs_get_target_for_wildcard_from_nested_base() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    ecs_id_t pair = ecs_pair(Rel, Obj);
    ecs_entity_t base = ecs_new_w_id(world, pair);
    ecs_entity_t base_2 = ecs_new_w_pair(world, EcsIsA, base);
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base_2);
    

    ecs_entity_t result = ecs_get_target_for_id(world, e, EcsIsA, 
        ecs_pair(Rel, EcsWildcard));
    test_assert(result != 0);
    test_assert(result == base);

    ecs_fini(world);
}

void Pairs_ignore_childof_from_base() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    ecs_entity_t parent = ecs_new_id(world);
    ecs_entity_t base = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_entity_t e = ecs_new_w_pair(world, EcsIsA, base);

    ecs_entity_t result = ecs_get_target_for_id(world, e, EcsIsA, 
        ecs_pair(EcsChildOf, parent));
    test_assert(result == 0);

    ecs_fini(world);
}

void Pairs_add_exclusive_relation_twice() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Exclusive);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_entity_t e = ecs_new_id(world);

    ecs_add_pair(world, e, Rel, ObjA);
    test_assert( ecs_has_pair(world, e, Rel, ObjA));

    ecs_add_pair(world, e, Rel, ObjB);
    test_assert( ecs_has_pair(world, e, Rel, ObjB));
    test_assert( !ecs_has_pair(world, e, Rel, ObjA));

    ecs_fini(world);
}

void Pairs_add_same_exclusive_relation_twice() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Exclusive);
    ECS_TAG(world, ObjA);

    ecs_entity_t e = ecs_new_id(world);

    ecs_add_pair(world, e, Rel, ObjA);
    test_assert( ecs_has_pair(world, e, Rel, ObjA));

    ecs_add_pair(world, e, Rel, ObjA);
    test_assert( ecs_has_pair(world, e, Rel, ObjA));

    ecs_fini(world);
}

void Pairs_set_exclusive_relation_twice() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Start);
    ECS_TAG(world, Stop);

    ecs_add_id(world, ecs_id(Position), EcsExclusive);

    ecs_entity_t e = ecs_new_id(world);

    ecs_set_pair(world, e, Position, Start, {10, 20});
    test_assert( ecs_has_pair(world, e, ecs_id(Position), Start));

    ecs_set_pair(world, e, Position, Stop, {20, 30});
    test_assert( ecs_has_pair(world, e, ecs_id(Position), Stop));
    test_assert( !ecs_has_pair(world, e, ecs_id(Position), Start));

    const Position *p = ecs_get_pair(world, e, Position, Stop);
    test_assert(p != NULL);
    test_int(p->x, 20);
    test_int(p->y, 30);

    p = ecs_get_pair(world, e, Position, Start);
    test_assert(p == NULL);

    ecs_fini(world);
}

void Pairs_add_exclusive_non_trivial_edge() {

}

void Pairs_add_exclusive_non_empty_table() {
    ecs_world_t *world = ecs_mini();    

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_TAG(world, TagC);
    ECS_TAG(world, TagD);

    ECS_ENTITY(world, Rel, Exclusive);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add(world, e, TagA);
    ecs_add(world, e, TagB);
    ecs_add(world, e, TagC);
    ecs_add(world, e, TagD);

    ecs_add_pair(world, e, Rel, ObjA);
    test_assert( ecs_has_pair(world, e, Rel, ObjA));

    ecs_add_pair(world, e, Rel, ObjB);
    test_assert( ecs_has_pair(world, e, Rel, ObjB));
    test_assert( !ecs_has_pair(world, e, Rel, ObjA));

    ecs_fini(world);
}

void Pairs_add_exclusive_non_empty_table_w_pairs() {
    ecs_world_t *world = ecs_mini();    

    ECS_TAG(world, RelA);

    ECS_ENTITY(world, Rel, Exclusive);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ECS_TAG(world, RelB);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, RelA, ObjA);
    ecs_add_pair(world, e, RelB, ObjA);

    ecs_add_pair(world, e, Rel, ObjA);
    test_assert( ecs_has_pair(world, e, Rel, ObjA));
    
    ecs_add_pair(world, e, Rel, ObjB);
    test_assert( ecs_has_pair(world, e, Rel, ObjB));
    test_assert( !ecs_has_pair(world, e, Rel, ObjA));

    ecs_fini(world);
}

void Pairs_add_pair_to_entity_w_exclusive_pair() {
    ecs_world_t *world = ecs_mini();    

    ECS_TAG(world, ObjA);

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, EcsChildOf, ObjA);
    test_assert( ecs_has_pair(world, e, EcsChildOf, ObjA));

    ecs_add_pair(world, e, RelA, ObjA);
    test_assert( ecs_has_pair(world, e, RelA, ObjA));

    ecs_add_pair(world, e, RelB, ObjA);
    test_assert( ecs_has_pair(world, e, RelB, ObjA));

    ecs_fini(world);
}

void Pairs_add_pair_to_entity_w_scope() {
    ecs_world_t *world = ecs_mini();    

    ECS_TAG(world, ObjA);

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_set_scope(world, ObjA);

    ecs_entity_t e = ecs_new(world, 0);

    ecs_add_pair(world, e, RelA, ObjA);
    test_assert( ecs_has_pair(world, e, RelA, ObjA));

    ecs_add_pair(world, e, RelB, ObjA);
    test_assert( ecs_has_pair(world, e, RelB, ObjA));

    ecs_set_scope(world, 0);

    ecs_fini(world);
}

void Pairs_add_existing_exclusive_pair_after_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, ObjA);

    ecs_entity_t parent = ecs_new_id(world);

    ecs_entity_t e = ecs_new_w_pair(world, EcsChildOf, parent);
    ecs_add_pair(world, e, EcsChildOf, ObjA);
    test_assert( ecs_has_pair(world, e, EcsChildOf, ObjA));

    ecs_add_pair(world, e, RelA, ObjA);
    test_assert( ecs_has_pair(world, e, EcsChildOf, ObjA));
    test_assert( ecs_has_pair(world, e, RelA, ObjA));

    ecs_add_pair(world, e, EcsChildOf, ObjA);
    test_assert( ecs_has_pair(world, e, EcsChildOf, ObjA));
    test_assert( ecs_has_pair(world, e, RelA, ObjA));

    ecs_add_pair(world, e, RelB, ObjA);

    test_assert( ecs_has_pair(world, e, EcsChildOf, ObjA));
    test_assert( ecs_has_pair(world, e, RelA, ObjA));
    test_assert( ecs_has_pair(world, e, RelB, ObjA));

    ecs_fini(world);
}

void Pairs_add_symmetric_relation() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_add_id(world, Rel, EcsSymmetric);

    ecs_add_pair(world, ObjA, Rel, ObjB);
    test_assert(ecs_has_pair(world, ObjA, Rel, ObjB));
    test_assert(ecs_has_pair(world, ObjB, Rel, ObjA));

    ecs_fini(world);
}

void Pairs_remove_symmetric_relation() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_add_id(world, Rel, EcsSymmetric);

    ecs_add_pair(world, ObjA, Rel, ObjB);
    test_assert(ecs_has_pair(world, ObjA, Rel, ObjB));
    test_assert(ecs_has_pair(world, ObjB, Rel, ObjA));

    ecs_remove_pair(world, ObjA, Rel, ObjB);
    test_assert(!ecs_has_pair(world, ObjA, Rel, ObjB));
    test_assert(!ecs_has_pair(world, ObjB, Rel, ObjA));

    ecs_fini(world);
}

void Pairs_delete_entity_w_symmetric_relation() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_add_id(world, Rel, EcsSymmetric);

    ecs_add_pair(world, ObjA, Rel, ObjB);
    test_assert(ecs_has_pair(world, ObjA, Rel, ObjB));
    test_assert(ecs_has_pair(world, ObjB, Rel, ObjA));

    ecs_delete(world, ObjA);

    test_assert(!ecs_has_pair(world, ObjB, Rel, ObjA));

    ecs_fini(world);
}

void Pairs_add_symmetric_exclusive_relation() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);
    ECS_TAG(world, ObjC);

    ecs_add_id(world, Rel, EcsSymmetric);
    ecs_add_id(world, Rel, EcsExclusive);

    ecs_add_pair(world, ObjA, Rel, ObjB);
    test_assert(ecs_has_pair(world, ObjA, Rel, ObjB));
    test_assert(ecs_has_pair(world, ObjB, Rel, ObjA));

    ecs_add_pair(world, ObjA, Rel, ObjC);
    test_assert(ecs_has_pair(world, ObjA, Rel, ObjC));
    test_assert(!ecs_has_pair(world, ObjB, Rel, ObjA));
    test_assert(ecs_has_pair(world, ObjC, Rel, ObjA));

    ecs_fini(world);
}

void Pairs_with() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_ENTITY(world, TagB, (With, TagA));

    ecs_entity_t e = ecs_new_id(world);
    ecs_add(world, e, TagB);
    test_assert( ecs_has(world, e, TagA));
    test_assert( ecs_has(world, e, TagB));

    ecs_fini(world);
}

void Pairs_2_with() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_TAG(world, TagB);
    ECS_ENTITY(world, TagC, (With, TagA), (With, TagB));

    ecs_entity_t e = ecs_new_id(world);
    ecs_add(world, e, TagC);
    test_assert( ecs_has(world, e, TagA));
    test_assert( ecs_has(world, e, TagB));
    test_assert( ecs_has(world, e, TagC));

    ecs_fini(world);
}

void Pairs_nested_with() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ECS_ENTITY(world, TagB, (With, TagA));
    ECS_ENTITY(world, TagC, (With, TagB));

    ecs_entity_t e = ecs_new_id(world);
    ecs_add(world, e, TagC);
    test_assert( ecs_has(world, e, TagA));
    test_assert( ecs_has(world, e, TagB));
    test_assert( ecs_has(world, e, TagC));

    ecs_fini(world);
}

void Pairs_with_relation() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Obj);
    ECS_TAG(world, RelA);
    ECS_ENTITY(world, RelB, (With, RelA));

    ecs_entity_t e = ecs_new_w_pair(world, RelB, Obj);
    test_assert( ecs_has_pair(world, e, RelA, Obj));
    test_assert( ecs_has_pair(world, e, RelB, Obj));

    ecs_fini(world);
}

void Pairs_2_with_relation() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Obj);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_ENTITY(world, RelC, (With, RelA), (With, RelB));

    ecs_entity_t e = ecs_new_w_pair(world, RelC, Obj);
    test_assert( ecs_has_pair(world, e, RelA, Obj));
    test_assert( ecs_has_pair(world, e, RelB, Obj));

    ecs_fini(world);
}

void Pairs_nested_with_relation() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Obj);
    ECS_TAG(world, RelA);
    ECS_ENTITY(world, RelB, (With, RelA));
    ECS_ENTITY(world, RelC, (With, RelB));

    ecs_entity_t e = ecs_new_w_pair(world, RelC, Obj);
    test_assert( ecs_has_pair(world, e, RelA, Obj));
    test_assert( ecs_has_pair(world, e, RelB, Obj));
    test_assert( ecs_has_pair(world, e, RelC, Obj));

    ecs_fini(world);
}

void Pairs_65k_relations() {
    ecs_world_t *world = ecs_mini();

    ecs_set_entity_range(world, 65536, 0);

    ecs_entity_t rel = ecs_new_id(world);
    ecs_entity_t obj = ecs_new_id(world);
    ecs_entity_t e = ecs_new_w_pair(world, rel, obj);
    test_assert(ecs_has_pair(world, e, rel, obj));

    ecs_fini(world);
}

void Pairs_remove_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, RelA, ObjA);
    ecs_add_pair(world, e, RelA, ObjB);
    ecs_add_pair(world, e, RelB, ObjA);
    ecs_add_pair(world, e, RelB, ObjB);

    test_assert( ecs_has_pair(world, e, RelA, ObjA));
    test_assert( ecs_has_pair(world, e, RelA, ObjB));
    test_assert( ecs_has_pair(world, e, RelB, ObjA));
    test_assert( ecs_has_pair(world, e, RelB, ObjB));

    ecs_remove_pair(world, e, RelA, EcsWildcard);

    test_assert( !ecs_has_pair(world, e, RelA, ObjA));
    test_assert( !ecs_has_pair(world, e, RelA, ObjB));
    test_assert( ecs_has_pair(world, e, RelB, ObjA));
    test_assert( ecs_has_pair(world, e, RelB, ObjB));
    
    ecs_fini(world);
}

void Pairs_remove_relation_wildcard() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, RelA, ObjA);
    ecs_add_pair(world, e, RelA, ObjB);
    ecs_add_pair(world, e, RelB, ObjA);
    ecs_add_pair(world, e, RelB, ObjB);

    test_assert( ecs_has_pair(world, e, RelA, ObjA));
    test_assert( ecs_has_pair(world, e, RelA, ObjB));
    test_assert( ecs_has_pair(world, e, RelB, ObjA));
    test_assert( ecs_has_pair(world, e, RelB, ObjB));

    ecs_remove_pair(world, e, EcsWildcard, ObjA);
    
    test_assert( !ecs_has_pair(world, e, RelA, ObjA));
    test_assert( ecs_has_pair(world, e, RelA, ObjB));
    test_assert( !ecs_has_pair(world, e, RelB, ObjA));
    test_assert( ecs_has_pair(world, e, RelB, ObjB));

    ecs_fini(world);
}

void Pairs_add_not_alive_relation() {
    ecs_world_t *world = ecs_mini();

    ecs_entity_t obj = ecs_new_id(world);
    ecs_entity_t e = ecs_new_w_pair(world, 2000, obj);
    test_assert(ecs_has_pair(world, e, 2000, obj));

    ecs_fini(world);
}

void Pairs_remove_wildcard_all() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, RelA, ObjA);
    ecs_add_pair(world, e, RelA, ObjB);
    ecs_add_pair(world, e, RelB, ObjA);
    ecs_add_pair(world, e, RelB, ObjB);

    test_assert( ecs_has_pair(world, e, RelA, ObjA));
    test_assert( ecs_has_pair(world, e, RelA, ObjB));
    test_assert( ecs_has_pair(world, e, RelB, ObjA));
    test_assert( ecs_has_pair(world, e, RelB, ObjB));

    ecs_remove_pair(world, e, EcsWildcard, EcsWildcard);
    
    test_assert( !ecs_has_pair(world, e, RelA, ObjA));
    test_assert( !ecs_has_pair(world, e, RelA, ObjB));
    test_assert( !ecs_has_pair(world, e, RelB, ObjA));
    test_assert( !ecs_has_pair(world, e, RelB, ObjB));

    ecs_fini(world);
}

void Pairs_inherit_exclusive() {    
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, Exclusive);
    ECS_TAG(world, ObjA);
    ECS_TAG(world, ObjB);

    ecs_entity_t base = ecs_new_w_pair(world, Rel, ObjA);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);

    test_assert( ecs_has_pair(world, inst, Rel, ObjA));

    ecs_add_pair(world, inst, Rel, ObjB);

    test_assert( ecs_has_pair(world, inst, Rel, ObjB));
    test_assert( !ecs_has_pair(world, inst, Rel, ObjA));

    ecs_fini(world);
}

void Pairs_dont_inherit() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, TagA);
    ecs_add_id(world, TagA, EcsDontInherit);
    ECS_TAG(world, TagB);

    ecs_entity_t base = ecs_new_w_id(world, TagA);
    ecs_add(world, base, TagB);
    ecs_entity_t inst = ecs_new_w_pair(world, EcsIsA, base);

    test_assert( ecs_has(world, base, TagA));
    test_assert( !ecs_has(world, inst, TagA));

    test_assert( ecs_has(world, base, TagB));
    test_assert( ecs_has(world, inst, TagB));

    ecs_fini(world);
}

void Pairs_has_wildcard_w_pair() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Rel);
    ECS_TAG(world, Obj);

    ecs_entity_t e = ecs_new_w_pair(world, Rel, Obj);

    test_bool(ecs_has_pair(world, e, EcsWildcard, EcsWildcard), true);
    test_bool(ecs_has_pair(world, e, Rel, EcsWildcard), true);
    test_bool(ecs_has_pair(world, e, EcsWildcard, Obj), true);
    test_bool(ecs_has_pair(world, e, Rel, Obj), true);
    test_bool(ecs_has_id(world, e, EcsWildcard), false);

    ecs_fini(world);
}

void Pairs_has_pair_wildcard_w_tag() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_new(world, Tag);

    test_bool(ecs_has_pair(world, e, EcsWildcard, EcsWildcard), false);
    test_bool(ecs_has_id(world, e, Tag), true);
    test_bool(ecs_has_id(world, e, EcsWildcard), true);

    ecs_fini(world);
}

void Pairs_oneof_self() {
    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, OneOf);
    ECS_ENTITY(world, ObjA, (ChildOf, Rel));
    ECS_ENTITY(world, ObjB, (ChildOf, Rel));

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, Rel, ObjA);
    test_assert(ecs_has_pair(world, e, Rel, ObjA));

    ecs_add_pair(world, e, Rel, ObjB);
    test_assert(ecs_has_pair(world, e, Rel, ObjB));

    ecs_fini(world);
}

void Pairs_oneof_other() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Parent);
    ECS_ENTITY(world, Rel, (OneOf, Parent));
    ECS_ENTITY(world, ObjA, (ChildOf, Parent));
    ECS_ENTITY(world, ObjB, (ChildOf, Parent));

    ecs_entity_t e = ecs_new_id(world);
    ecs_add_pair(world, e, Rel, ObjA);
    test_assert(ecs_has_pair(world, e, Rel, ObjA));

    ecs_add_pair(world, e, Rel, ObjB);
    test_assert(ecs_has_pair(world, e, Rel, ObjB));

    ecs_fini(world);
}

void Pairs_oneof_self_constraint_violated() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_ENTITY(world, Rel, OneOf);
    ECS_ENTITY(world, ObjA, (ChildOf, Rel));
    ECS_ENTITY(world, ObjB, (ChildOf, Rel));
    ECS_TAG(world, ObjC);

    ecs_entity_t e = ecs_new_id(world);

    test_expect_abort();
    ecs_add_pair(world, e, Rel, ObjC);
}

void Pairs_oneof_other_constraint_violated() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Parent);
    ECS_ENTITY(world, Rel, (OneOf, Parent));
    ECS_ENTITY(world, ObjA, (ChildOf, Parent));
    ECS_ENTITY(world, ObjB, (ChildOf, Parent));
    ECS_TAG(world, ObjC);

    ecs_entity_t e = ecs_new_id(world);

    test_expect_abort();
    ecs_add_pair(world, e, Rel, ObjC);
}

void Pairs_oneof_other_rel_parent_constraint_violated() {
    install_test_abort();

    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Parent);
    ECS_ENTITY(world, Rel, (OneOf, Parent));
    ECS_ENTITY(world, ObjA, (ChildOf, Parent));
    ECS_ENTITY(world, ObjB, (ChildOf, Parent));
    ECS_ENTITY(world, ObjC, (ChildOf, Rel));

    ecs_entity_t e = ecs_new_id(world);

    test_expect_abort();
    ecs_add_pair(world, e, Rel, ObjC);
}
