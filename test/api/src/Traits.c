#include <api.h>

typedef struct Trait {
    float value;
} Trait;

void ProcessTraits(ecs_iter_t *it) {
    Trait *tr = ecs_column(it, Trait, 1);

    probe_system(it);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        tr[i].value ++;
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

    ecs_entity_t e2 = ecs_set_trait(world, 0, Velocity, Trait, {
        .value = 20
    });
    test_assert(e2 != 0);

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

    ecs_set_trait(world, e1, Velocity, Trait, {
        .value = 20
    });      

    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_set_trait(world, e2, Position, Trait, {
        .value = 30
    });      
    ecs_set_trait(world, e2, Velocity, Trait, {
        .value = 40
    });

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
