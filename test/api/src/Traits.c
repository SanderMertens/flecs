#include <api.h>

typedef struct Trait {
    float value;
} Trait;

typedef struct TraitA {
    float value;
} TraitA;

typedef struct TraitB {
    float value;
} TraitB;

void ProcessTraits(ecs_iter_t *it) {
    Trait *tr = ecs_column(it, Trait, 1);

    probe_system(it);

    if (tr) {
        int32_t i;
        for (i = 0; i < it->count; i ++) {
            tr[i].value ++;
        }
    }
}

void Traits_type_w_one_trait() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Trait);

    ECS_SYSTEM(world, ProcessTraits, EcsOnUpdate, TRAIT | Trait);

    /* Ensure that trait is matched against different components */
    ecs_entity_t e1 = ecs_set_trait(world, 0, Position, Trait, {
        .value = 10
    });  
    test_assert(e1 != 0);
    test_assert( ecs_has_trait(world, e1, Position, Trait));

    ecs_entity_t e2 = ecs_set_trait(world, 0, Velocity, Trait, {
        .value = 20
    });
    test_assert(e2 != 0);
    test_assert( ecs_has_trait(world, e2, Velocity, Trait));

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    const Trait* tr = ecs_get_trait(world, e1, Position, Trait);
    test_assert(tr != NULL);
    test_int(tr->value, 11);

    tr = ecs_get_trait(world, e2, Velocity, Trait);
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
    ecs_entity_t hi = ecs_entity_t_hi(c & ECS_ENTITY_MASK);
    ecs_entity_t lo = ecs_entity_t_lo(c);
    test_int(hi, ecs_entity(Trait));
    test_int(lo, ecs_entity(Position));

    c = ctx.c[1][0] & ECS_ENTITY_MASK;
    hi = ecs_entity_t_hi(c & ECS_ENTITY_MASK);
    lo = ecs_entity_t_lo(c);
    test_int(hi, ecs_entity(Trait));
    test_int(lo, ecs_entity(Velocity));

    test_int(ctx.s[0][0], 0);
    test_int(ctx.s[1][0], 0);

    ecs_fini(world);
}

void Traits_type_w_two_traits() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Trait);

    ECS_SYSTEM(world, ProcessTraits, EcsOnUpdate, TRAIT | Trait);

    /* Ensure that trait is matched against different components on same entity */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_set_trait(world, e1, Position, Trait, {
        .value = 10
    });  
    test_assert( ecs_has_trait(world, e1, Position, Trait));

    ecs_set_trait(world, e1, Velocity, Trait, {
        .value = 20
    });      
    test_assert( ecs_has_trait(world, e1, Velocity, Trait));

    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_set_trait(world, e2, Position, Trait, {
        .value = 30
    });
    test_assert( ecs_has_trait(world, e2, Position, Trait));

    ecs_set_trait(world, e2, Velocity, Trait, {
        .value = 40
    });
    test_assert( ecs_has_trait(world, e2, Position, Trait));

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    const Trait* tr = ecs_get_trait(world, e1, Position, Trait);
    test_assert(tr != NULL);
    test_int(tr->value, 11);

    tr = ecs_get_trait(world, e1, Velocity, Trait);
    test_assert(tr != NULL);
    test_int(tr->value, 21);    

    tr = ecs_get_trait(world, e2, Position, Trait);
    test_assert(tr != NULL);
    test_int(tr->value, 31);

    tr = ecs_get_trait(world, e2, Velocity, Trait);
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
    ecs_entity_t hi = ecs_entity_t_hi(c & ECS_ENTITY_MASK);
    ecs_entity_t lo = ecs_entity_t_lo(c);
    test_int(hi, ecs_entity(Trait));
    test_int(lo, ecs_entity(Velocity));

    c = ctx.c[1][0] & ECS_ENTITY_MASK;
    hi = ecs_entity_t_hi(c & ECS_ENTITY_MASK);
    lo = ecs_entity_t_lo(c);
    test_int(hi, ecs_entity(Trait));
    test_int(lo, ecs_entity(Position));

    test_int(ctx.s[0][0], 0);
    test_int(ctx.s[1][0], 0);
    test_int(ctx.s[2][0], 0);
    test_int(ctx.s[3][0], 0);

    ecs_fini(world);
}

void Traits_add_trait() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Trait);

    ECS_SYSTEM(world, ProcessTraits, EcsOnUpdate, TRAIT | Trait);

    ecs_entity_t e1 = ecs_new(world, 0);
    test_assert(e1 != 0);

    ecs_add_trait(world, e1, Position, Trait);
    test_assert( ecs_has_trait(world, e1, Position, Trait));

    ecs_fini(world);
}

void Traits_remove_trait() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Trait);

    ECS_SYSTEM(world, ProcessTraits, EcsOnUpdate, TRAIT | Trait);

    ecs_entity_t e1 = ecs_set_trait(world, 0, Position, Trait, {
        .value = 10
    });  
    test_assert(e1 != 0);
    test_assert( ecs_has_trait(world, e1, Position, Trait));

    ecs_remove_trait(world, e1, Position, Trait);

    test_assert( !ecs_has_trait(world, e1, Position, Trait));

    ecs_fini(world);
}

void Traits_add_tag_trait_for_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag1);
    ECS_TAG(world, Tag2);
    ECS_TAG(world, TraitTag);

    ECS_SYSTEM(world, ProcessTraits, EcsOnUpdate, TRAIT | TraitTag);

    /* Ensure that trait is matched against different components */
    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_add_entity(world, e1, ecs_trait(Tag1, TraitTag));
    test_assert(e1 != 0);
    test_assert( ecs_has_entity(world, e1, ecs_trait(Tag1, TraitTag)));

    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_add_entity(world, e2, ecs_trait(Tag2, TraitTag));
    test_assert(e2 != 0);
    test_assert( ecs_has_entity(world, e2, ecs_trait(Tag2, TraitTag)));

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

    ecs_entity_t hi = ecs_entity_t_hi(c & ECS_ENTITY_MASK);
    ecs_entity_t lo = ecs_entity_t_lo(c);
    test_int(hi, TraitTag);
    test_int(lo, Tag1);

    c = ctx.c[1][0] & ECS_ENTITY_MASK;
    hi = ecs_entity_t_hi(c & ECS_ENTITY_MASK);
    lo = ecs_entity_t_lo(c);
    test_int(hi, TraitTag);
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

void Traits_add_tag_trait_for_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TAG(world, TraitTag);

    ECS_SYSTEM(world, ProcessValueTraits, EcsOnUpdate, TRAIT | TraitTag);

    ecs_entity_t e1 = ecs_set_trait_tag(world, 0, TraitTag, Position, {
        .x = 1,
        .y = 2
    });
    test_assert( ecs_has_entity(world, e1, ecs_trait(ecs_entity(Position), TraitTag)));

    ecs_entity_t e2 = ecs_set_trait_tag(world, 0, TraitTag, Velocity, {
        .x = 3,
        .y = 4
    });
    test_assert( ecs_has_entity(world, e2, ecs_trait(ecs_entity(Velocity), TraitTag)));

    Probe ctx = {0};
    ecs_set_context(world, &ctx);

    ecs_progress(world, 0);

    const Position* tr_p = ecs_get_w_entity(world, e1, ecs_trait(ecs_entity(Position), TraitTag));
    test_assert(tr_p != NULL);
    test_int(tr_p->x, 11);
    test_int(tr_p->y, 22);

    const Velocity* tr_v = ecs_get_w_entity(world, e2, ecs_trait(ecs_entity(Velocity), TraitTag));
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
    ecs_entity_t hi = ecs_entity_t_hi(c & ECS_ENTITY_MASK);
    ecs_entity_t lo = ecs_entity_t_lo(c);
    test_int(hi, TraitTag);
    test_int(lo, ecs_entity(Position));

    c = ctx.c[1][0] & ECS_ENTITY_MASK;
    hi = ecs_entity_t_hi(c & ECS_ENTITY_MASK);
    lo = ecs_entity_t_lo(c);
    test_int(hi, TraitTag);
    test_int(lo, ecs_entity(Velocity));

    test_int(ctx.s[0][0], 0);
    test_int(ctx.s[1][0], 0);

    ecs_fini(world);
}

void ProcessTwoTraits(ecs_iter_t *it) {
    TraitA *tr_a = ecs_column(it, TraitA, 1);
    TraitB *tr_b = ecs_column(it, TraitB, 2);

    probe_system(it);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        tr_a[i].value ++;
        tr_b[i].value ++;
    }
}

void Traits_query_2_traits() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, TraitA);
    ECS_COMPONENT(world, TraitB);

    ECS_SYSTEM(world, ProcessTwoTraits, EcsOnUpdate, TRAIT | TraitA, TRAIT | TraitB);

    /* Create entity with both TraitA and TraitB */
    ecs_entity_t e1 = ecs_set_trait(world, 0, Position, TraitA, {
        .value = 1
    });

    ecs_set_trait(world, e1, Position, TraitB, {
        .value = 2
    });    

    test_assert( ecs_has_trait(world, e1, Position, TraitA));
    test_assert( ecs_has_trait(world, e1, Position, TraitB));

    /* Create entity with only TraitA. Should not be matched with system */
    ecs_entity_t e2 = ecs_set_trait(world, 0, Position, TraitA, {
        .value = 3
    });
    test_assert( ecs_has_trait(world, e2, Position, TraitA));

    /* Run system */
    Probe ctx = {0};
    ecs_set_context(world, &ctx);
    ecs_progress(world, 0);

    const TraitA *tr_a = ecs_get_trait(world, e1, Position, TraitA);
    test_int(tr_a->value, 2);

    const TraitB *tr_b = ecs_get_trait(world, e1, Position, TraitB);
    test_int(tr_b->value, 3);

    tr_a = ecs_get_trait(world, e2, Position, TraitA);
    test_int(tr_a->value, 3);

    test_int(ctx.count, 1);
    test_int(ctx.invoked, 1);
    test_int(ctx.system, ProcessTwoTraits);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);

    ecs_entity_t c = ctx.c[0][0];
    ecs_entity_t hi = ecs_entity_t_hi(c & ECS_ENTITY_MASK);
    ecs_entity_t lo = ecs_entity_t_lo(c);
    test_int(hi, ecs_entity(TraitA));
    test_int(lo, ecs_entity(Position));

    c = ctx.c[0][1];
    hi = ecs_entity_t_hi(c & ECS_ENTITY_MASK);
    lo = ecs_entity_t_lo(c);
    test_int(hi, ecs_entity(TraitB));
    test_int(lo, ecs_entity(Position));    

    ecs_fini(world);
}

void Traits_query_2_traits_2_instances_per_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, TraitA);
    ECS_COMPONENT(world, TraitB);

    ECS_SYSTEM(world, ProcessTwoTraits, EcsOnUpdate, TRAIT | TraitA, TRAIT | TraitB);

    /* Create entity with both TraitA and TraitB, applied to two components*/
    ecs_entity_t e1 = ecs_set_trait(world, 0, Position, TraitA, {
        .value = 1
    });

    ecs_set_trait(world, e1, Position, TraitB, {
        .value = 2
    });

    ecs_set_trait(world, e1, Velocity, TraitA, {
        .value = 3
    });

    ecs_set_trait(world, e1, Velocity, TraitB, {
        .value = 4
    });        

    test_assert( ecs_has_trait(world, e1, Position, TraitA));
    test_assert( ecs_has_trait(world, e1, Position, TraitB));
    test_assert( ecs_has_trait(world, e1, Velocity, TraitA));
    test_assert( ecs_has_trait(world, e1, Velocity, TraitB));

    /* Run system */
    Probe ctx = {0};
    ecs_set_context(world, &ctx);
    ecs_progress(world, 0);

    const TraitA *tr_a = ecs_get_trait(world, e1, Position, TraitA);
    test_int(tr_a->value, 2);

    tr_a = ecs_get_trait(world, e1, Velocity, TraitA);
    test_int(tr_a->value, 4);    

    const TraitB *tr_b = ecs_get_trait(world, e1, Position, TraitB);
    test_int(tr_b->value, 3);

    tr_b = ecs_get_trait(world, e1, Velocity, TraitB);
    test_int(tr_b->value, 5);

    test_int(ctx.count, 2);
    test_int(ctx.invoked, 2);
    test_int(ctx.system, ProcessTwoTraits);
    test_int(ctx.column_count, 2);
    test_null(ctx.param);

    test_int(ctx.e[0], e1);
    test_int(ctx.e[1], e1);

    ecs_entity_t c = ctx.c[0][0];
    ecs_entity_t hi = ecs_entity_t_hi(c & ECS_ENTITY_MASK);
    ecs_entity_t lo = ecs_entity_t_lo(c);
    test_int(hi, ecs_entity(TraitA));
    test_int(lo, ecs_entity(Position));

    c = ctx.c[0][1];
    hi = ecs_entity_t_hi(c & ECS_ENTITY_MASK);
    lo = ecs_entity_t_lo(c);
    test_int(hi, ecs_entity(TraitB));
    test_int(lo, ecs_entity(Position));    

    c = ctx.c[1][0];
    hi = ecs_entity_t_hi(c & ECS_ENTITY_MASK);
    lo = ecs_entity_t_lo(c);
    test_int(hi, ecs_entity(TraitA));
    test_int(lo, ecs_entity(Velocity));

    c = ctx.c[1][1];
    hi = ecs_entity_t_hi(c & ECS_ENTITY_MASK);
    lo = ecs_entity_t_lo(c);
    test_int(hi, ecs_entity(TraitB));
    test_int(lo, ecs_entity(Velocity));  

    ecs_fini(world);
}
