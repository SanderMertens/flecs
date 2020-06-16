#include <api.h>

void Add_bulk_add_comp_from_comp_to_empty() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_bulk_new(world, Position, 10);
    test_assert(e != 0);

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    ecs_bulk_add(world, Velocity, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( ecs_has(world, e + i, Velocity));

        const Position *p = ecs_get(world, e + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2);

        test_assert(ecs_get(world, e + i, Velocity) != NULL);
    }

    test_int( ecs_count(world, Position), 10);
    test_int( ecs_count(world, Velocity), 10);

    ecs_fini(world);
}

void Add_bulk_add_comp_from_tag_to_empty() {
    ecs_world_t *world = ecs_init();
    
    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_bulk_new(world, Tag, 10);
    test_assert(e != 0);

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Tag));
    }

    ecs_bulk_add(world, Velocity, &(ecs_filter_t){
        .include = ecs_type(Tag)
    });

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Tag));
        test_assert( ecs_has(world, e + i, Velocity));
        test_assert(ecs_get(world, e + i, Velocity) != NULL);
    }

    test_int( ecs_count_entity(world, Tag), 10);
    test_int( ecs_count(world, Velocity), 10);

    ecs_fini(world);
}

void Add_bulk_add_comp_from_comp_to_existing() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t existing = ecs_bulk_new(world, Type, 10);
    test_assert(existing != 0);

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, existing + i, Position));
        ecs_set(world, existing + i, Position, {i * 3, i * 4});
        test_assert( ecs_has(world, existing + i, Velocity));
        ecs_set(world, existing + i, Velocity, {i * 5, i * 6});        
    }    

    ecs_entity_t e = ecs_bulk_new(world, Position, 10);
    test_assert(e != 0);
    test_assert(e != existing);

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    ecs_bulk_add(world, Velocity, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( ecs_has(world, e + i, Velocity));

        const Position *p = ecs_get(world, e + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2);

        test_assert(ecs_get(world, e + i, Velocity) != NULL);
    }

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, existing + i, Position));
        test_assert( ecs_has(world, existing + i, Velocity));

        const Position *p = ecs_get(world, existing + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i * 3);
        test_int(p->y, i * 4);

        const Velocity *v = ecs_get(world, existing + i, Velocity);
        test_assert(v != NULL);
        test_int(v->x, i * 5);
        test_int(v->y, i * 6);
    }    

    test_int( ecs_count(world, Position), 20);
    test_int( ecs_count(world, Velocity), 20);

    ecs_fini(world);
}

void Add_bulk_add_comp_from_tag_to_existing() {
    ecs_world_t *world = ecs_init();
    
    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Tag, Velocity);

    ecs_entity_t existing = ecs_bulk_new(world, Type, 10);
    test_assert(existing != 0);

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, existing + i, Velocity));
        ecs_set(world, existing + i, Velocity, {i * 3, i * 4});
    }    

    ecs_entity_t e = ecs_bulk_new_w_entity(world, Tag, 10);
    test_assert(e != 0);
    test_assert(e != existing);

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has_entity(world, e + i, Tag));
    }

    ecs_bulk_add(world, Velocity, &(ecs_filter_t){
        .include = ecs_type(Tag)
    });

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has_entity(world, e + i, Tag));
        test_assert( ecs_has(world, e + i, Velocity));
        test_assert(ecs_get(world, e + i, Velocity) != NULL);
    }

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has_entity(world, existing + i, Tag));
        test_assert( ecs_has(world, existing + i, Velocity));

        const Velocity *v = ecs_get(world, existing + i, Velocity);
        test_assert(v != NULL);
        test_int(v->x, i * 3);
        test_int(v->y, i * 4);
    }    

    test_int( ecs_count_entity(world, Tag), 20);
    test_int( ecs_count(world, Velocity), 20);

    ecs_fini(world);
}

void Add_bulk_add_tag_from_tag_to_empty() {
    ecs_world_t *world = ecs_init();
    
    ECS_TAG(world, Tag);
    ECS_TAG(world, Tag2);

    ecs_entity_t e = ecs_bulk_new(world, Tag, 10);
    test_assert(e != 0);

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Tag));
    }

    ecs_bulk_add(world, Tag2, &(ecs_filter_t){
        .include = ecs_type(Tag)
    });

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Tag));
        test_assert( ecs_has(world, e + i, Tag2));
    }

    test_int( ecs_count_entity(world, Tag), 10);
    test_int( ecs_count_entity(world, Tag2), 10);

    ecs_fini(world);
}

void Add_bulk_add_tag_from_tag_to_existing() {
    ecs_world_t *world = ecs_init();
    
    ECS_TAG(world, Tag);
    ECS_TAG(world, Tag2);
    ECS_TYPE(world, Type, Tag, Tag2);

    ecs_entity_t existing = ecs_bulk_new(world, Type, 10);
    test_assert(existing != 0);

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, existing + i, Tag2));
    }    

    ecs_entity_t e = ecs_bulk_new_w_entity(world, Tag, 10);
    test_assert(e != 0);
    test_assert(e != existing);

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has_entity(world, e + i, Tag));
    }

    ecs_bulk_add(world, Tag2, &(ecs_filter_t){
        .include = ecs_type(Tag)
    });

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has_entity(world, e + i, Tag));
        test_assert( ecs_has(world, e + i, Tag2));
    }

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has_entity(world, existing + i, Tag));
        test_assert( ecs_has(world, existing + i, Tag2));
    }    

    test_int( ecs_count_entity(world, Tag), 20);
    test_int( ecs_count_entity(world, Tag2), 20);

    ecs_fini(world);
}

void Add_bulk_add_comp_to_more_existing() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t existing = ecs_bulk_new(world, Type, 20);
    test_assert(existing != 0);

    int i;
    for (i = 0; i < 20; i ++) {
        test_assert( ecs_has(world, existing + i, Position));
        ecs_set(world, existing + i, Position, {i * 3, i * 4});
        test_assert( ecs_has(world, existing + i, Velocity));
        ecs_set(world, existing + i, Velocity, {i * 5, i * 6});        
    }    

    ecs_entity_t e = ecs_bulk_new(world, Position, 10);
    test_assert(e != 0);
    test_assert(e != existing);

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    ecs_bulk_add(world, Velocity, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( ecs_has(world, e + i, Velocity));

        const Position *p = ecs_get(world, e + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2);
    }

    for (i = 0; i < 20; i ++) {
        test_assert( ecs_has(world, existing + i, Position));
        test_assert( ecs_has(world, existing + i, Velocity));

        const Position *p = ecs_get(world, existing + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i * 3);
        test_int(p->y, i * 4);

        const Velocity *v = ecs_get(world, existing + i, Velocity);
        test_assert(v != NULL);
        test_int(v->x, i * 5);
        test_int(v->y, i * 6);
    }    

    test_int( ecs_count(world, Position), 30);
    test_int( ecs_count(world, Velocity), 30);

    ecs_fini(world);
}

void Add_bulk_add_comp_to_fewer_existing() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_entity_t existing = ecs_bulk_new(world, Type, 5);
    test_assert(existing != 0);

    int i;
    for (i = 0; i < 5; i ++) {
        test_assert( ecs_has(world, existing + i, Position));
        ecs_set(world, existing + i, Position, {i * 3, i * 4});
        test_assert( ecs_has(world, existing + i, Velocity));
        ecs_set(world, existing + i, Velocity, {i * 5, i * 6});        
    }    

    ecs_entity_t e = ecs_bulk_new(world, Position, 10);
    test_assert(e != 0);
    test_assert(e != existing);

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    ecs_bulk_add(world, Velocity, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( ecs_has(world, e + i, Velocity));

        const Position *p = ecs_get(world, e + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2);

        test_assert(ecs_get(world, e + i, Velocity) != NULL);
    }

    for (i = 0; i < 5; i ++) {
        test_assert( ecs_has(world, existing + i, Position));
        test_assert( ecs_has(world, existing + i, Velocity));

        const Position *p = ecs_get(world, existing + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i * 3);
        test_int(p->y, i * 4);

        const Velocity *v = ecs_get(world, existing + i, Velocity);
        test_assert(v != NULL);
        test_int(v->x, i * 5);
        test_int(v->y, i * 6);
    }    

    test_int( ecs_count(world, Position), 15);
    test_int( ecs_count(world, Velocity), 15);

    ecs_fini(world);
}

void AddVelocity(ecs_iter_t *it) {
    probe_system(it);
}

void Add_bulk_on_add() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TRIGGER(world, AddVelocity, EcsOnAdd, Velocity);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_bulk_new(world, Position, 10);
    test_assert(e != 0);

    ecs_bulk_add(world, Velocity, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 10);
    test_int(ctx.system, AddVelocity);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    int i;
    for (i = 0; i < 10; i ++) {
        test_int(ctx.e[i], e + i);
    }
    
    test_int(ctx.c[0][0], ecs_entity(Velocity));

    ecs_fini(world);
}

void Add_bulk_add_entity_comp() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e = ecs_bulk_new(world, Position, 10);
    test_assert(e != 0);

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    ecs_bulk_add_entity(world, ecs_entity(Velocity), &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( ecs_has(world, e + i, Velocity));

        const Position *p = ecs_get(world, e + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2);

        test_assert(ecs_get(world, e + i, Velocity) != NULL);
    }

    test_int( ecs_count(world, Position), 10);
    test_int( ecs_count(world, Velocity), 10);

    ecs_fini(world);
}

void Add_bulk_add_entity_tag() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_TAG(world, Tag);

    ecs_entity_t e = ecs_bulk_new(world, Position, 10);
    test_assert(e != 0);

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    ecs_bulk_add_entity(world, Tag, &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        test_assert( ecs_has_entity(world, e + i, Tag));

        const Position *p = ecs_get(world, e + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2);
    }

    test_int( ecs_count(world, Position), 10);
    test_int( ecs_count_entity(world, Tag), 10);
    
    ecs_fini(world);
}

void Add_bulk_add_entity_on_add() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TRIGGER(world, AddVelocity, EcsOnAdd, Velocity);

    Probe ctx = { 0 };
    ecs_set_context(world, &ctx);

    ecs_entity_t e = ecs_bulk_new(world, Position, 10);
    test_assert(e != 0);

    ecs_bulk_add_entity(world, ecs_entity(Velocity), &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    test_int(ctx.invoked, 1);
    test_int(ctx.count, 10);
    test_int(ctx.system, AddVelocity);
    test_int(ctx.column_count, 1);
    test_null(ctx.param);

    int i;
    for (i = 0; i < 10; i ++) {
        test_int(ctx.e[i], e + i);
    }
    
    test_int(ctx.c[0][0], ecs_entity(Velocity));

    ecs_fini(world);
}

void Add_bulk_add_entity_existing() {
    ecs_world_t *world = ecs_init();
    
    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_bulk_new(world, Position, 10);
    test_assert(e != 0);

    int i;
    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));
        ecs_set(world, e + i, Position, {i, i * 2});
    }

    ecs_bulk_add_entity(world, ecs_entity(Position), &(ecs_filter_t){
        .include = ecs_type(Position)
    });

    for (i = 0; i < 10; i ++) {
        test_assert( ecs_has(world, e + i, Position));

        const Position *p = ecs_get(world, e + i, Position);
        test_assert(p != NULL);
        test_int(p->x, i);
        test_int(p->y, i * 2);
    }

    test_int( ecs_count(world, Position), 10);

    ecs_fini(world);
}
