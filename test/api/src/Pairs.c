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

void ProcessTraits(ecs_iter_t *it) {
    Rel *tr = ecs_column(it, Rel, 1);

    probe_system(it);

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

    ECS_SYSTEM(world, ProcessTraits, EcsOnUpdate, PAIR | Rel);

    /* Ensure that pair is matched against different components */
    ecs_entity_t e1 = ecs_set_pair(world, 0, Rel, ecs_typeid(Position), {
        .value = 10
    });

    test_assert(e1 != 0);
    test_assert( ecs_has_pair(world, e1, ecs_typeid(Rel), ecs_typeid(Position)));

    ecs_entity_t e2 = ecs_set_pair(world, 0, Rel, ecs_typeid(Velocity), {
        .value = 20
    });
    test_assert(e2 != 0);
    test_assert( ecs_has_pair(world, e2, ecs_typeid(Rel), ecs_typeid(Velocity)));

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    const Rel* tr = ecs_get_pair(world, e1, Rel, ecs_typeid(Position));
    test_assert(tr != NULL);
    test_int(tr->value, 11);

    tr = ecs_get_pair(world, e2, Rel, ecs_typeid(Velocity));
    test_assert(tr != NULL);
    test_int(tr->value, 21); 

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, ProcessTraits);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);

    ecs_entity_t c = ctx.c[0][0];
    ecs_entity_t hi = ECS_PAIR_RELATION(c);
    ecs_entity_t lo = ECS_PAIR_OBJECT(c);
    test_int(hi, ecs_typeid(Rel));
    test_int(lo, ecs_typeid(Position));

    c = ctx.c[1][0];
    hi = ECS_PAIR_RELATION(c);
    lo = ECS_PAIR_OBJECT(c);
    test_int(hi, ecs_typeid(Rel));
    test_int(lo, ecs_typeid(Velocity));

    test_int(ctx.s[0][0], 0);
    test_int(ctx.s[1][0], 0);

    ecs_fini(world);
}

void Pairs_type_w_two_pairs() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rel);

    ECS_SYSTEM(world, ProcessTraits, EcsOnUpdate, PAIR | Rel);

    /* Ensure that pair is matched against different components on same entity */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_set_pair(world, e1, Rel, ecs_typeid(Position), {
        .value = 10
    });  
    test_assert( ecs_has_pair(world, e1, ecs_typeid(Rel), ecs_typeid(Position)));

    ecs_set_pair(world, e1, Rel, ecs_typeid(Velocity), {
        .value = 20
    });      
    test_assert( ecs_has_pair(world, e1, ecs_typeid(Rel), ecs_typeid(Velocity)));

    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_set_pair(world, e2, Rel, ecs_typeid(Position), {
        .value = 30
    });
    test_assert( ecs_has_pair(world, e2, ecs_typeid(Rel), ecs_typeid(Position)));

    ecs_set_pair(world, e2, Rel, ecs_typeid(Velocity), {
        .value = 40
    });
    test_assert( ecs_has_pair(world, e2, ecs_typeid(Rel), ecs_typeid(Position)));

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    const Rel* tr = ecs_get_pair(world, e1, Rel, ecs_typeid(Position));
    test_assert(tr != NULL);
    test_int(tr->value, 11);

    tr = ecs_get_pair(world, e1, Rel, ecs_typeid(Velocity));
    test_assert(tr != NULL);
    test_int(tr->value, 21);

    tr = ecs_get_pair(world, e2, Rel, ecs_typeid(Position));
    test_assert(tr != NULL);
    test_int(tr->value, 31);

    tr = ecs_get_pair(world, e2, Rel, ecs_typeid(Velocity));
    test_assert(tr != NULL);
    test_int(tr->value, 41); 

    test_int(ctx.count, 4);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, ProcessTraits);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);
    test_int(ctx.e[2], e1);
    test_int(ctx.e[3], e2);

    ecs_entity_t c = ctx.c[0][0];
    ecs_entity_t hi = ECS_PAIR_RELATION(c);
    ecs_entity_t lo = ECS_PAIR_OBJECT(c);
    test_int(hi, ecs_typeid(Rel));
    test_int(lo, ecs_typeid(Velocity));

    c = ctx.c[1][0];
    hi = ECS_PAIR_RELATION(c);
    lo = ECS_PAIR_OBJECT(c);
    test_int(hi, ecs_typeid(Rel));
    test_int(lo, ecs_typeid(Position));

    test_int(ctx.s[0][0], 0);
    test_int(ctx.s[1][0], 0);
    test_int(ctx.s[2][0], 0);
    test_int(ctx.s[3][0], 0);

    ecs_fini(world);
}

void Pairs_add_pair() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rel);

    ECS_SYSTEM(world, ProcessTraits, EcsOnUpdate, PAIR | Rel);

    ecs_entity_t e1 = ecs_new(world, 0);
    test_assert(e1 != 0);

    ecs_add_pair(world, e1, ecs_typeid(Rel), ecs_typeid(Position));
    test_assert( ecs_has_pair(world, e1, ecs_typeid(Rel), ecs_typeid(Position)));

    ecs_fini(world);
}

void Pairs_remove_pair() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rel);

    ECS_SYSTEM(world, ProcessTraits, EcsOnUpdate, PAIR | Rel);

    ecs_entity_t e1 = ecs_set_pair(world, 0, Rel, ecs_typeid(Position), {
        .value = 10
    });  
    test_assert(e1 != 0);
    test_assert( ecs_has_pair(world, e1, ecs_typeid(Rel), ecs_typeid(Position)));

    ecs_remove_pair(world, e1, ecs_typeid(Rel), ecs_typeid(Position));
    test_assert( !ecs_has_pair(world, e1, ecs_typeid(Rel), ecs_typeid(Position)));

    ecs_fini(world);
}

void Pairs_add_tag_pair_for_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag1);
    ECS_TAG(world, Tag2);
    ECS_TAG(world, Rel);

    ECS_SYSTEM(world, ProcessTraits, EcsOnUpdate, PAIR | Rel);

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
    test_int(ctx.system, ProcessTraits);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);

    ecs_entity_t c = ctx.c[0][0];

    ecs_entity_t hi = ECS_PAIR_RELATION(c);
    ecs_entity_t lo = ECS_PAIR_OBJECT(c);
    test_int(hi, Rel);
    test_int(lo, Tag1);

    c = ctx.c[1][0];
    hi = ECS_PAIR_RELATION(c);
    lo = ECS_PAIR_OBJECT(c);
    test_int(hi, Rel);
    test_int(lo, Tag2);

    test_int(ctx.s[0][0], 0);
    test_int(ctx.s[1][0], 0);

    ecs_fini(world);
}

void ProcessValueTraits(ecs_iter_t *it) {
    /* Strictly speaking this can be either Position or Velocity, but they have
     * the same layout. */
    Position *p = ecs_column(it, Position, 1);

    probe_system(it);

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

    ECS_SYSTEM(world, ProcessValueTraits, EcsOnUpdate, PAIR | Rel);

    ecs_entity_t e1 = ecs_set_pair_object(world, 0, Rel, Position, {
        .x = 1,
        .y = 2
    });
    test_assert( ecs_has_pair(world, e1, Rel, ecs_typeid(Position)));

    ecs_entity_t e2 = ecs_set_pair_object(world, 0, Rel, Velocity, {
        .x = 3,
        .y = 4
    });
    test_assert( ecs_has_pair(world, e2, Rel, ecs_typeid(Velocity)));

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
    test_int(ctx.system, ProcessValueTraits);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);

    ecs_entity_t c = ctx.c[0][0];
    ecs_entity_t hi = ECS_PAIR_RELATION(c);
    ecs_entity_t lo = ECS_PAIR_OBJECT(c);
    test_int(hi, Rel);
    test_int(lo, ecs_typeid(Position));

    c = ctx.c[1][0];
    hi = ECS_PAIR_RELATION(c);
    lo = ECS_PAIR_OBJECT(c);
    test_int(hi, Rel);
    test_int(lo, ecs_typeid(Velocity));

    test_int(ctx.s[0][0], 0);
    test_int(ctx.s[1][0], 0);

    ecs_fini(world);
}

void ProcessTwoTraits(ecs_iter_t *it) {
    RelA *tr_a = ecs_column(it, RelA, 1);
    RelB *tr_b = ecs_column(it, RelB, 2);

    probe_system(it);

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

    ECS_SYSTEM(world, ProcessTwoTraits, EcsOnUpdate, PAIR | RelA, PAIR | RelB);

    /* Create entity with both RelA and RelB */
    ecs_entity_t e1 = ecs_set_pair(world, 0, RelA, ecs_typeid(Position), {
        .value = 1
    });

    ecs_set_pair(world, e1, RelB, ecs_typeid(Position), {
        .value = 2
    });

    test_assert( ecs_has_pair(world, e1, ecs_typeid(RelA), ecs_typeid(Position)));
    test_assert( ecs_has_pair(world, e1, ecs_typeid(RelB), ecs_typeid(Position)));

    /* Create entity with only RelA. Should not be matched with system */
    ecs_entity_t e2 = ecs_set_pair(world, 0, RelA, ecs_typeid(Position), {
        .value = 3
    });
    test_assert( ecs_has_pair(world, e2, ecs_typeid(RelA), ecs_typeid(Position)));

    /* Run system */
    Probe ctx = {0};
    ecs_set_context(world, &ctx);
    ecs_progress(world, 0);

    const RelA *tr_a = ecs_get_pair(world, e1, RelA, ecs_typeid(Position));
    test_int(tr_a->value, 2);

    const RelB *tr_b = ecs_get_pair(world, e1, RelB, ecs_typeid(Position));
    test_int(tr_b->value, 3);

    tr_a = ecs_get_pair(world, e2, RelA, ecs_typeid(Position));
    test_int(tr_a->value, 3);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, ProcessTwoTraits);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);

    ecs_entity_t c = ctx.c[0][0];
    ecs_entity_t hi = ECS_PAIR_RELATION(c);
    ecs_entity_t lo = ECS_PAIR_OBJECT(c);
    test_int(hi, ecs_typeid(RelA));
    test_int(lo, ecs_typeid(Position));

    c = ctx.c[0][1];
    hi = ECS_PAIR_RELATION(c);
    lo = ECS_PAIR_OBJECT(c);
    test_int(hi, ecs_typeid(RelB));
    test_int(lo, ecs_typeid(Position));    

    ecs_fini(world);
}

void Pairs_query_2_pairs_2_instances_per_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, RelA);
    ECS_COMPONENT(world, RelB);

    ECS_SYSTEM(world, ProcessTwoTraits, EcsOnUpdate, PAIR | RelA, PAIR | RelB);

    /* Create entity with both RelA and RelB, applied to two components*/
    ecs_entity_t e1 = ecs_set_pair(world, 0, RelA, ecs_typeid(Position), {
        .value = 1
    });

    ecs_set_pair(world, e1, RelB, ecs_typeid(Position), {
        .value = 2
    });

    ecs_set_pair(world, e1, RelA, ecs_typeid(Velocity), {
        .value = 3
    });

    ecs_set_pair(world, e1, RelB, ecs_typeid(Velocity), {
        .value = 4
    });        

    test_assert( ecs_has_pair(world, e1, ecs_typeid(RelA), ecs_typeid(Position)));
    test_assert( ecs_has_pair(world, e1, ecs_typeid(RelB), ecs_typeid(Position)));
    test_assert( ecs_has_pair(world, e1, ecs_typeid(RelA), ecs_typeid(Velocity)));
    test_assert( ecs_has_pair(world, e1, ecs_typeid(RelB), ecs_typeid(Velocity)));

    /* Run system */
    Probe ctx = {0};
    ecs_set_context(world, &ctx);
    ecs_progress(world, 0);

    const RelA *tr_a = ecs_get_pair(world, e1, RelA, ecs_typeid(Position));
    test_int(tr_a->value, 2);

    tr_a = ecs_get_pair(world, e1, RelA, ecs_typeid(Velocity));
    test_int(tr_a->value, 4);

    const RelB *tr_b = ecs_get_pair(world, e1, RelB, ecs_typeid(Position));
    test_int(tr_b->value, 3);

    tr_b = ecs_get_pair(world, e1, RelB, ecs_typeid(Velocity));
    test_int(tr_b->value, 5);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, ProcessTwoTraits);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e1);

    ecs_entity_t c = ctx.c[0][0];
    ecs_entity_t hi = ECS_PAIR_RELATION(c);
    ecs_entity_t lo = ECS_PAIR_OBJECT(c);
    test_int(hi, ecs_typeid(RelA));
    test_int(lo, ecs_typeid(Position));

    c = ctx.c[0][1];
    hi = ECS_PAIR_RELATION(c);
    lo = ECS_PAIR_OBJECT(c);
    test_int(hi, ecs_typeid(RelB));
    test_int(lo, ecs_typeid(Position));    

    c = ctx.c[1][0];
    hi = ECS_PAIR_RELATION(c);
    lo = ECS_PAIR_OBJECT(c);
    test_int(hi, ecs_typeid(RelA));
    test_int(lo, ecs_typeid(Velocity));

    c = ctx.c[1][1];
    hi = ECS_PAIR_RELATION(c);
    lo = ECS_PAIR_OBJECT(c);
    test_int(hi, ecs_typeid(RelB));
    test_int(lo, ecs_typeid(Velocity));  

    ecs_fini(world);
}

void Pairs_override_pair() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Rel);

    ecs_entity_t base = ecs_new(world, 0);
    ecs_set_pair(world, base, Rel, ecs_typeid(Position), {.value = 10});

    ecs_entity_t instance = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(ecs_has_pair(world, instance, ecs_typeid(Rel), ecs_typeid(Position)));

    const Rel *t = ecs_get_pair(world, instance, Rel, ecs_typeid(Position));
    test_assert(t != NULL);
    test_int(t->value, 10);

    const Rel *t_2 = ecs_get_pair(world, base, Rel, ecs_typeid(Position));
    test_assert(t_2 != NULL);
    test_assert(t == t_2);

    ecs_add_pair(world, instance, ecs_typeid(Rel), ecs_typeid(Position));
    t = ecs_get_pair(world, instance, Rel, ecs_typeid(Position));
    test_assert(t != NULL);
    test_int(t->value, 10);
    test_assert(t != t_2);

    ecs_fini(world);
}

void Pairs_override_tag_pair() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Rel);

    ecs_entity_t base = ecs_new(world, 0);
    ecs_set_pair_object(world, base, Rel, Position, {.x = 10, .y = 20});

    ecs_entity_t instance = ecs_new_w_pair(world, EcsIsA, base);
    test_assert(ecs_has_pair(world, instance, Rel, ecs_typeid(Position)));

    const Position *t = ecs_get_pair_object(world, instance, Rel, Position);
    test_assert(t != NULL);
    test_int(t->x, 10);
    test_int(t->y, 20);

    const Position *t_2 = ecs_get_pair_object(world, base, Rel, Position);
    test_assert(t_2 != NULL);
    test_assert(t == t_2);

    ecs_add_pair(world, instance, Rel, ecs_typeid(Position));
    t = ecs_get_pair_object(world, instance, Rel, Position);
    test_assert(t != NULL);
    test_int(t->x, 10);
    test_int(t->y, 20);
    test_assert(t != t_2);

    ecs_fini(world);
}

void Pairs_pair_wildcard_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rel);

    ECS_SYSTEM(world, ProcessTraits, EcsOnUpdate, PAIR | Rel, PAIR | Rel > *);

    /* Ensure that pair is matched against different components */
    ecs_entity_t e1 = 
    ecs_set_pair(world, 0, Rel, ecs_typeid(Position), {
        .value = 10
    });
    ecs_add(world, e1, Position);

    ecs_entity_t e2 = 
    ecs_set_pair(world, 0, Rel, ecs_typeid(Velocity), {
        .value = 20
    });
    ecs_add(world, e2, Velocity);

    /* This entity should not be matched as it doesn't have Position */
    ecs_set_pair(world, 0, Rel, ecs_typeid(Position), {
        .value = 20
    });

    /* This entity should not be matched as it doesn't have the pair */
    ecs_set(world, 0, Position, { 10, 20 });

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    const Rel* tr = ecs_get_pair(world, e1, Rel, ecs_typeid(Position));
    test_assert(tr != NULL);
    test_int(tr->value, 11);

    tr = ecs_get_pair(world, e2, Rel, ecs_typeid(Velocity));
    test_assert(tr != NULL);
    test_int(tr->value, 21); 

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, ProcessTraits);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);

    ecs_entity_t c = ctx.c[0][0];
    ecs_entity_t hi = ECS_PAIR_RELATION(c);
    ecs_entity_t lo = ECS_PAIR_OBJECT(c);
    test_int(hi, ecs_typeid(Rel));
    test_int(lo, ecs_typeid(Position));
    test_int(ctx.c[0][1], ecs_typeid(Position));

    c = ctx.c[1][0];
    hi = ECS_PAIR_RELATION(c);
    lo = ECS_PAIR_OBJECT(c);
    test_int(hi, ecs_typeid(Rel));
    test_int(lo, ecs_typeid(Velocity));
    test_int(ctx.c[1][1], ecs_typeid(Velocity));

    test_int(ctx.s[0][0], 0);
    test_int(ctx.s[1][0], 0);

    ecs_fini(world);
}

static
void IterTraitWildcard(ecs_iter_t *it) {
    probe_system(it);
}

void Pairs_pair_only_wildcard_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rel);

    ECS_SYSTEM(world, IterTraitWildcard, EcsOnUpdate, PAIR | Rel > *);

    /* Ensure that pair is matched against different components */
    ecs_entity_t e1 = 
    ecs_set_pair(world, 0, Rel, ecs_typeid(Position), {
        .value = 10
    });
    ecs_add(world, e1, Position);

    ecs_entity_t e2 = 
    ecs_set_pair(world, 0, Rel, ecs_typeid(Velocity), {
        .value = 20
    });
    ecs_add(world, e2, Velocity);

    /* This entity should not be matched as it doesn't have Position */
    ecs_set_pair(world, 0, Rel, ecs_typeid(Position), {
        .value = 20
    });

    /* This entity should not be matched as it doesn't have the pair */
    ecs_set(world, 0, Position, { 10, 20 });

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, IterTraitWildcard);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e2);

    test_int(ctx.c[0][0], ecs_typeid(Position));
    test_int(ctx.c[1][0], ecs_typeid(Velocity));

    test_int(ctx.s[0][0], 0);
    test_int(ctx.s[1][0], 0);

    ecs_fini(world);
}

static int set_pair_invoked = 0;

static
void SetTrait(ecs_iter_t *it) {
    set_pair_invoked ++;
    probe_system(it);
}

void Pairs_pair_wildcard_on_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rel);

    ECS_SYSTEM(world, SetTrait, EcsOnSet, PAIR | Rel > *);

    /* Ensure that pair is matched against different components */
    ecs_entity_t e1 = 
    ecs_set_pair(world, 0, Rel, ecs_typeid(Position), { .value = 10 });
    ecs_add(world, e1, Position);
    test_int(set_pair_invoked, 0);

    ecs_entity_t e2 = 
    ecs_set_pair(world, 0, Rel, ecs_typeid(Velocity), { .value = 20 });
    ecs_add(world, e2, Velocity);
    test_int(set_pair_invoked, 0);

    /* This entity should not trigger as it doesn't have the pair */
    ecs_set(world, 0, Position, { 10, 20 });
    test_int(set_pair_invoked, 0);

    /* Set Position on e1, should trigger OnSet */
    ecs_set(world, e1, Position, {10, 20});
    test_int(set_pair_invoked, 1);

    ecs_set(world, e2, Position, {10, 20});
    test_int(set_pair_invoked, 1);

    ecs_set(world, e2, Velocity, {10, 20});
    test_int(set_pair_invoked, 2);

    ecs_fini(world);
}

static
void TraitTrigger(ecs_iter_t *it) {
    probe_system(it);
}

void Pairs_on_add_pair() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rel);

    ECS_TRIGGER(world, TraitTrigger, EcsOnAdd, Rel);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_add_pair(world, e, ecs_typeid(Rel), ecs_typeid(Position));

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, TraitTrigger);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);

    ecs_entity_t c = ctx.c[0][0];
    ecs_entity_t hi = ECS_PAIR_RELATION(c);
    ecs_entity_t lo = ECS_PAIR_OBJECT(c);
    test_int(hi, ecs_typeid(Rel));
    test_int(lo, ecs_typeid(Position));  

    ctx = (Probe){ 0 };
    ecs_add_pair(world, e, ecs_typeid(Rel), ecs_typeid(Velocity));

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, TraitTrigger);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);

    c = ctx.c[0][0];
    hi = ECS_PAIR_RELATION(c);
    lo = ECS_PAIR_OBJECT(c);
    test_int(hi, ecs_typeid(Rel));
    test_int(lo, ecs_typeid(Velocity));

    ecs_fini(world);
}

void Pairs_on_add_pair_tag() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Rel);

    ECS_TRIGGER(world, TraitTrigger, EcsOnAdd, Position);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_add_pair(world, e, Rel, ecs_typeid(Position));

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, TraitTrigger);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);

    ecs_entity_t c = ctx.c[0][0];
    ecs_entity_t hi = ECS_PAIR_RELATION(c);
    ecs_entity_t lo = ECS_PAIR_OBJECT(c);
    test_int(hi, Rel);
    test_int(lo, ecs_typeid(Position));  

    ecs_fini(world);
}

void Pairs_on_remove_pair() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rel);

    ECS_TRIGGER(world, TraitTrigger, EcsOnRemove, Rel);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_add_pair(world, e, ecs_typeid(Rel), ecs_typeid(Position));
    ecs_add_pair(world, e, ecs_typeid(Rel), ecs_typeid(Velocity));
    test_int(ctx.count, 0);

    ecs_remove_pair(world, e, ecs_typeid(Rel), ecs_typeid(Position));
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, TraitTrigger);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);

    ecs_entity_t c = ctx.c[0][0];
    ecs_entity_t hi = ECS_PAIR_RELATION(c);
    ecs_entity_t lo = ECS_PAIR_OBJECT(c);
    test_int(hi, ecs_typeid(Rel));
    test_int(lo, ecs_typeid(Position));  

    ctx = (Probe){ 0 };
    ecs_remove_pair(world, e, ecs_typeid(Rel), ecs_typeid(Velocity));
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, TraitTrigger);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);

    c = ctx.c[0][0];
    hi = ECS_PAIR_RELATION(c);
    lo = ECS_PAIR_OBJECT(c);
    test_int(hi, ecs_typeid(Rel));
    test_int(lo, ecs_typeid(Velocity));

    ecs_fini(world);
}

void Pairs_on_remove_pair_tag() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Rel);

    ECS_TRIGGER(world, TraitTrigger, EcsOnRemove, Position);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_add_pair(world, e, Rel, ecs_typeid(Position));
    test_int(ctx.count, 0);

    ecs_remove_pair(world, e, Rel, ecs_typeid(Position));
    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, TraitTrigger);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);

    ecs_entity_t c = ctx.c[0][0];
    ecs_entity_t hi = ECS_PAIR_RELATION(c);
    ecs_entity_t lo = ECS_PAIR_OBJECT(c);
    test_int(hi, Rel);
    test_int(lo, ecs_typeid(Position));  

    ecs_fini(world);
}

void Pairs_on_remove_pair_on_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rel);

    ECS_TRIGGER(world, TraitTrigger, EcsOnRemove, Rel);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_add_pair(world, e, ecs_typeid(Rel), ecs_typeid(Position));
    ecs_add_pair(world, e, ecs_typeid(Rel), ecs_typeid(Velocity));
    test_int(ctx.count, 0);

    ecs_delete(world, e);
    test_int(ctx.count, 2);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, TraitTrigger);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);

    ecs_entity_t c = ctx.c[0][0];
    ecs_entity_t hi = ECS_PAIR_RELATION(c);
    ecs_entity_t lo = ECS_PAIR_OBJECT(c);
    test_int(hi, ecs_typeid(Rel));
    test_int(lo, ecs_typeid(Position));  

    test_int(ctx.e[1], e);

    c = ctx.c[1][0];
    hi = ECS_PAIR_RELATION(c);
    lo = ECS_PAIR_OBJECT(c);
    test_int(hi, ecs_typeid(Rel));
    test_int(lo, ecs_typeid(Velocity));

    ecs_fini(world);
}

static
void TraitTriggerPosition(ecs_iter_t *it) {
    TraitTrigger(it);
}

static
void TraitTriggerVelocity(ecs_iter_t *it) {
    TraitTrigger(it);
}

void Pairs_on_remove_pair_tag_on_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, Rel);

    ECS_TRIGGER(world, TraitTriggerPosition, EcsOnRemove, Position);
    ECS_TRIGGER(world, TraitTriggerVelocity, EcsOnRemove, Velocity);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_add_pair(world, e, Rel, ecs_typeid(Position));
    ecs_add_pair(world, e, Rel, ecs_typeid(Velocity));
    test_int(ctx.count, 0);

    ecs_delete(world, e);
    test_int(ctx.count, 2);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, TraitTriggerVelocity);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);
    test_int(ctx.e[0], e);

    ecs_entity_t c = ctx.c[0][0];
    ecs_entity_t hi = ECS_PAIR_RELATION(c);
    ecs_entity_t lo = ECS_PAIR_OBJECT(c);
    test_int(hi, Rel);
    test_int(lo, ecs_typeid(Position));  

    test_int(ctx.e[1], e);

    c = ctx.c[1][0];
    hi = ECS_PAIR_RELATION(c);
    lo = ECS_PAIR_OBJECT(c);
    test_int(hi, Rel);
    test_int(lo, ecs_typeid(Velocity));

    ecs_fini(world);
}

void Pairs_pair_from_shared() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Rel);

    ecs_entity_t base = ecs_new(world, 0);
    ecs_add_pair(world, base, ecs_typeid(Rel), ecs_typeid(Position));

    ecs_entity_t e = ecs_new(world, 0);
    ecs_add_pair(world, e, EcsIsA, base);

    ecs_query_t *q = ecs_query_new(world, "SHARED:PAIR|Rel>Position"); // ew
    
    int32_t count = 0;
    ecs_iter_t it = ecs_query_iter(q);
    while (ecs_query_next(&it)) {
        Rel *t = ecs_column(&it, Rel, 1);
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

void Pairs_simple_pair_syntax() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Rel);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_add_pair(world, e, ecs_typeid(Rel), ecs_typeid(Position));

    ecs_query_t *q = ecs_query_new(world, "Rel FOR Position"); // not ew
    
    int32_t count = 0;
    ecs_iter_t it = ecs_query_iter(q);
    while (ecs_query_next(&it)) {
        int i;
        for (i = 0; i < it.count; i ++) {
            test_assert(it.entities[i] == e);
        }
        count += it.count;
    }

    test_assert(count == 1);

    ecs_fini(world);
}

void Pairs_pair_w_component_query() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Rel);

    ecs_entity_t e = ecs_new(world, 0);
    ecs_add_pair(world, e, ecs_typeid(Rel), ecs_typeid(Position));

    ecs_query_t *q = ecs_query_new(world, "PAIR | Rel > Position");
    
    int32_t count = 0;
    ecs_iter_t it = ecs_query_iter(q);
    while (ecs_query_next(&it)) {
        Rel *t = ecs_column(&it, Rel, 1);
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
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Rel);

    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_add_pair(world, e1, Rel, ecs_typeid(Position));
    ecs_entity_t e2 = ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "PAIR | Rel > Position || Position");
    
    int32_t count = 0;
    ecs_iter_t it = ecs_query_iter(q);
    while (ecs_query_next(&it)) {
        Position *t = ecs_column(&it, Position, 1);
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
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, RelA);
    ECS_TAG(world, RelB);

    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_add_pair(world, e1, RelA, ecs_typeid(Position));
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_add_pair(world, e2, RelB, ecs_typeid(Position));

    ecs_query_t *q = ecs_query_new(world, "PAIR | RelA > Position || PAIR | RelB > Position");
    
    int32_t count = 0;
    ecs_iter_t it = ecs_query_iter(q);
    while (ecs_query_next(&it)) {
        Position *t = ecs_column(&it, Position, 1);
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

void Pairs_query_not_pair() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Rel);

    ecs_entity_t e1 = ecs_new(world, Position);
    ecs_entity_t e2 = ecs_new(world, Position);
    ecs_add_pair(world, e2, Rel, ecs_typeid(Position));

    ecs_query_t *q = ecs_query_new(world, "!PAIR | Rel > Position, Position");
    
    int32_t count = 0;
    ecs_iter_t it = ecs_query_iter(q);
    while (ecs_query_next(&it)) {
        Position *t = ecs_column(&it, Position, 1);
        test_assert(t == NULL);
        Position *p = ecs_column(&it, Position, 2);
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
    ecs_world_t *world = ecs_init();

    ecs_entity_t o = ecs_new_id(world);
    test_assert(o != 0);

    ecs_entity_t dummy = ecs_new_id(world);
    ecs_delete(world, dummy); // force recycle

    // don't use ECS_COMPONENT, because it will try to get low ids first
    ecs_entity_t pos = ecs_set(world, 0, EcsComponent, {4});
    test_assert(pos != 0);
    test_assert((uint32_t)pos != pos); // ensure recycled

    ecs_id_t pair = ecs_pair(pos, o);
    ecs_entity_t tid = ecs_get_typeid(world, pair);
    test_assert(tid != 0);
    test_assert(tid == pos);

    ecs_fini(world);
}

void Pairs_get_typeid_w_recycled_obj() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t r = ecs_new_id(world);
    test_assert(r != 0);

    ecs_entity_t dummy = ecs_new_id(world);
    ecs_delete(world, dummy); // force recycle

    // don't use ECS_COMPONENT, because it will try to get low ids first
    ecs_entity_t pos = ecs_set(world, 0, EcsComponent, {4});
    test_assert(pos != 0);
    test_assert((uint32_t)pos != pos); // ensure recycled

    ecs_id_t pair = ecs_pair(r, pos);
    ecs_entity_t tid = ecs_get_typeid(world, pair);
    test_assert(tid != 0);
    test_assert(tid == pos);

    ecs_fini(world);
}

void Pairs_id_str_w_recycled_rel() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t o = ecs_set(world, 0, EcsName, {"o"});
    test_assert(o != 0);

    ecs_entity_t dummy = ecs_new_id(world);
    ecs_delete(world, dummy); // force recycle

    ecs_entity_t r = ecs_set(world, 0, EcsName, {"r"});
    test_assert(r != 0);
    test_assert((uint32_t)r != r); // ensure recycled

    ecs_id_t pair = ecs_pair(r, o);
    char buff[64];
    ecs_id_str(world, pair, buff, sizeof(buff));

    test_str(buff, "(r,o)");

    ecs_fini(world);
}

void Pairs_id_str_w_recycled_obj() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t r = ecs_set(world, 0, EcsName, {"r"});
    test_assert(r != 0);

    ecs_entity_t dummy = ecs_new_id(world);
    ecs_delete(world, dummy); // force recycle

    ecs_entity_t o = ecs_set(world, 0, EcsName, {"o"});
    test_assert(o != 0);
    test_assert((uint32_t)o != o); // ensure recycled

    ecs_id_t pair = ecs_pair(r, o);
    char buff[64];
    ecs_id_str(world, pair, buff, sizeof(buff));

    test_str(buff, "(r,o)");

    ecs_fini(world);
}
