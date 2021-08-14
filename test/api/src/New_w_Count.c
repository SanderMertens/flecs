#include <api.h>

void New_w_Count_empty() {
    ecs_world_t *world = ecs_init();

    const ecs_entity_t *ids = ecs_bulk_new(world, 0, 1000);
    test_assert(ids != NULL);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(ids[i] != 0);
    }

    ecs_fini(world);
}

void New_w_Count_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    const ecs_entity_t *ids = ecs_bulk_new(world, Position, 1000);
    test_assert(ids != NULL);
    
    int i;
    for (i = 0; i < 1000; i ++) {
        ecs_entity_t e = ids[i];
        test_assert(e != 0);
        test_assert(ecs_has(world, e, Position));
    }

    test_int(ecs_count(world, Position), 1000);

    ecs_fini(world);
}

void New_w_Count_tag() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Tag, 0);

    const ecs_entity_t *ids = ecs_bulk_new_w_id(world, Tag, 1000);
    test_assert(ids != NULL);
    test_int(ecs_count_id(world, Tag), 1000);

    int i;
    for (i = 0; i < 1000; i ++) {
        ecs_entity_t e = ids[i];
        test_assert(e != 0);
        test_assert(ecs_has_id(world, e, Tag));
    }

    ecs_fini(world);
}

static
void AddPosition(ecs_iter_t *it) {
    ecs_entity_t velocity = *(ecs_entity_t*)it->ctx;

    int i;
    for (i = it->count - 1; i >= 0; i --) {
        ecs_set_id(
            it->world, it->entities[i], velocity, 
            sizeof(Velocity), &(Velocity){2, 3});
    }
}

static
void SetPosition(ecs_iter_t *it) {
    Position *p = ecs_term(it, Position, 1);

    ecs_entity_t rotation = *(ecs_entity_t*)it->ctx;

    int i;
    for (i = it->count - 1; i >= 0; i --) {
        p[i].y ++;
        ecs_add_id(it->world, it->entities[i], rotation);
    }
}

void New_w_Count_new_w_data_1_comp() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    const ecs_entity_t *ids = ecs_bulk_new_w_data(world, 3, 
        &(ecs_ids_t){
            .array = (ecs_entity_t[]){ecs_id(Position)},
            .count = 1
        },
        (void*[]){
            (Position[]){
                {10, 20},
                {30, 40},
                {50, 60}
            }
        });

    test_assert(ecs_has(world, ids[0], Position));
    test_assert(ecs_has(world, ids[1], Position));
    test_assert(ecs_has(world, ids[2], Position));

    const Position *
    p = ecs_get(world, ids[0], Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get(world, ids[1], Position);
    test_int(p->x, 30);
    test_int(p->y, 40);

    p = ecs_get(world, ids[2], Position);
    test_int(p->x, 50);
    test_int(p->y, 60);

    ecs_fini(world);
}

void New_w_Count_new_w_data_2_comp() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    const ecs_entity_t *ids = ecs_bulk_new_w_data(world, 3, 
        &(ecs_ids_t){
            .array = (ecs_entity_t[]){ecs_id(Position), ecs_id(Velocity)},
            .count = 2
        },
        (void*[]){
            (Position[]){
                {10, 20},
                {30, 40},
                {50, 60}
            },
            (Velocity[]){
                {1, 2},
                {3, 4},
                {5, 6}
            }
        });

    test_assert(ecs_has(world, ids[0], Position));
    test_assert(ecs_has(world, ids[1], Position));
    test_assert(ecs_has(world, ids[2], Position));
    
    test_assert(ecs_has(world, ids[0], Velocity));
    test_assert(ecs_has(world, ids[1], Velocity));
    test_assert(ecs_has(world, ids[2], Velocity));

    const Position *
    p = ecs_get(world, ids[0], Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get(world, ids[1], Position);
    test_int(p->x, 30);
    test_int(p->y, 40);

    p = ecs_get(world, ids[2], Position);
    test_int(p->x, 50);
    test_int(p->y, 60);

    const Velocity *
    v = ecs_get(world, ids[0], Velocity);
    test_int(v->x, 1);
    test_int(v->y, 2);

    v = ecs_get(world, ids[1], Velocity);
    test_int(v->x, 3);
    test_int(v->y, 4);

    v = ecs_get(world, ids[2], Velocity);
    test_int(v->x, 5);
    test_int(v->y, 6);

    ecs_fini(world);
}

void New_w_Count_new_w_data_w_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    const ecs_entity_t *ids = ecs_bulk_new_w_data(world, 3, 
        &(ecs_ids_t){
            .array = (ecs_entity_t[]){Tag},
            .count = 1
        },
        (void*[]){
            NULL
        });

    test_assert(ecs_has(world, ids[0], Tag));
    test_assert(ecs_has(world, ids[1], Tag));
    test_assert(ecs_has(world, ids[2], Tag));

    ecs_fini(world);
}

void New_w_Count_new_w_data_w_comp_and_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Tag);
    ECS_COMPONENT(world, Position);

    const ecs_entity_t *ids = ecs_bulk_new_w_data(world, 3, 
        &(ecs_ids_t){
            .array = (ecs_entity_t[]){ecs_id(Position), Tag},
            .count = 2
        },
        (void*[]){
            (Position[]){
                {10, 20},
                {30, 40},
                {50, 60}
            },
            NULL
        });

    test_assert(ecs_has(world, ids[0], Position));
    test_assert(ecs_has(world, ids[1], Position));
    test_assert(ecs_has(world, ids[2], Position));

    test_assert(ecs_has(world, ids[0], Tag));
    test_assert(ecs_has(world, ids[1], Tag));
    test_assert(ecs_has(world, ids[2], Tag));

    const Position *
    p = ecs_get(world, ids[0], Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get(world, ids[1], Position);
    test_int(p->x, 30);
    test_int(p->y, 40);

    p = ecs_get(world, ids[2], Position);
    test_int(p->x, 50);
    test_int(p->y, 60);

    ecs_fini(world);
}

void New_w_Count_new_w_data_pair() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pair);
    ECS_COMPONENT(world, Position);
    
    ecs_entity_t pair_id = ecs_pair(Pair, ecs_id(Position));

    const ecs_entity_t *ids = ecs_bulk_new_w_data(world, 3, 
        &(ecs_ids_t){
            .array = (ecs_entity_t[]){pair_id},
            .count = 1
        },
        (void*[]){
            (Position[]){
                {10, 20},
                {30, 40},
                {50, 60}
            },
            NULL
        });

    test_assert(ecs_has_id(world, ids[0], pair_id));
    test_assert(ecs_has_id(world, ids[1], pair_id));
    test_assert(ecs_has_id(world, ids[2], pair_id));

    const Position *
    p = ecs_get_id(world, ids[0], pair_id);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get_id(world, ids[1], pair_id);
    test_int(p->x, 30);
    test_int(p->y, 40);

    p = ecs_get_id(world, ids[2], pair_id);
    test_int(p->x, 50);
    test_int(p->y, 60);

    ecs_fini(world);
}

void New_w_Count_new_w_data_pair_tag() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pair);
    ECS_TAG(world, Tag);
    
    ecs_entity_t pair_id = ecs_pair(Pair, Tag);

    const ecs_entity_t *ids = ecs_bulk_new_w_data(world, 3, 
        &(ecs_ids_t){
            .array = (ecs_entity_t[]){pair_id},
            .count = 1
        },
        (void*[]){
            NULL
        });

    test_assert(ecs_has_id(world, ids[0], pair_id));
    test_assert(ecs_has_id(world, ids[1], pair_id));
    test_assert(ecs_has_id(world, ids[2], pair_id));

    ecs_fini(world);
}

ECS_CTOR(Velocity, ptr, {
    ptr->x = 0;
    ptr->y = 0;
});

void New_w_Count_new_w_data_2_comp_1_not_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_set_component_actions(world, Velocity, {
        .ctor = ecs_ctor(Velocity)
    });

    const ecs_entity_t *ids = ecs_bulk_new_w_data(world, 3, 
        &(ecs_ids_t){
            .array = (ecs_entity_t[]){ecs_id(Position), ecs_id(Velocity)},
            .count = 2
        },
        (void*[]){
            (Position[]){
                {10, 20},
                {30, 40},
                {50, 60}
            },
            NULL
        });

    test_assert(ecs_has(world, ids[0], Position));
    test_assert(ecs_has(world, ids[1], Position));
    test_assert(ecs_has(world, ids[2], Position));
    
    test_assert(ecs_has(world, ids[0], Velocity));
    test_assert(ecs_has(world, ids[1], Velocity));
    test_assert(ecs_has(world, ids[2], Velocity));

    const Position *
    p = ecs_get(world, ids[0], Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get(world, ids[1], Position);
    test_int(p->x, 30);
    test_int(p->y, 40);

    p = ecs_get(world, ids[2], Position);
    test_int(p->x, 50);
    test_int(p->y, 60);

    const Velocity *
    v = ecs_get(world, ids[0], Velocity);
    test_int(v->x, 0);
    test_int(v->y, 0);

    v = ecs_get(world, ids[1], Velocity);
    test_int(v->x, 0);
    test_int(v->y, 0);

    v = ecs_get(world, ids[2], Velocity);
    test_int(v->x, 0);
    test_int(v->y, 0);

    ecs_fini(world);
}

static int32_t on_movable_count = 0;

static
void OnMovable(ecs_iter_t *it) {
    on_movable_count += it->count;
}

void New_w_Count_new_w_on_add_on_set_monitor() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Rotation);

    ECS_TRIGGER(world, AddPosition, EcsOnAdd, Position);
    ECS_SYSTEM(world, SetPosition, EcsOnSet, Position);
    ECS_SYSTEM(world, OnMovable, EcsMonitor, Position, Velocity);

    ecs_trigger_init(world, &(ecs_trigger_desc_t){
        .entity = {AddPosition}, .ctx = &ecs_id(Velocity)
    });
    ecs_system_init(world, &(ecs_system_desc_t){
        .entity = {SetPosition}, .ctx = &ecs_id(Rotation)
    });

    const ecs_entity_t *ids = ecs_bulk_new_w_data(world, 3, 
        &(ecs_ids_t){
            .array = (ecs_entity_t[]){
                ecs_id(Position)
            }, 
            .count = 1
        },    
        (void*[]){ 
            (Position[]){
                {10, 20},
                {30, 40},
                {50, 60}
            }
        }
    );

    test_assert(ids != NULL);
    test_int(on_movable_count, 3);

    int i;
    for (i = 0; i < 3; i ++) {
        ecs_entity_t e = ids[i];
        test_assert(e != 0);
        test_assert( ecs_has(world, e, Position) );
        test_assert( ecs_has(world, e, Velocity) );
        test_assert( ecs_has(world, e, Rotation) );

        const Position *p = ecs_get(world, e, Position);
        test_assert(p != NULL);
        test_int(p->x, 10 + 20 * i);
        test_int(p->y, 20 + i * 20 + 1);

        const Velocity *v = ecs_get(world, e, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 2);
        test_int(v->y, 3);        
    }

    ecs_fini(world);
}

void New_w_Count_new_w_data_override_set_comp() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t base = ecs_set(world, 0, Position, {100, 200});

    const ecs_entity_t *ids = ecs_bulk_new_w_data(world, 3, 
        &(ecs_ids_t){
            .array = (ecs_entity_t[]){ecs_id(Position), ecs_pair(EcsIsA, base)},
            .count = 2
        },
        (void*[]){
            (Position[]){
                {10, 20},
                {30, 40},
                {50, 60}
            },
            NULL
        });

    test_assert(ecs_has(world, ids[0], Position));
    test_assert(ecs_has(world, ids[1], Position));
    test_assert(ecs_has(world, ids[2], Position));

    test_assert(ecs_has_pair(world, ids[0], EcsIsA, base));
    test_assert(ecs_has_pair(world, ids[1], EcsIsA, base));
    test_assert(ecs_has_pair(world, ids[2], EcsIsA, base));

    const Position *
    p = ecs_get(world, ids[0], Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get(world, ids[1], Position);
    test_int(p->x, 30);
    test_int(p->y, 40);

    p = ecs_get(world, ids[2], Position);
    test_int(p->x, 50);
    test_int(p->y, 60);

    ecs_fini(world);
}

void New_w_Count_new_w_data_override_set_pair() {
    ecs_world_t *world = ecs_init();

    ECS_TAG(world, Pair);
    ECS_COMPONENT(world, Position);

    ecs_entity_t pair_id = ecs_pair(Pair, ecs_id(Position));

    ecs_entity_t base = ecs_new(world, 0);
    Position *ptr = ecs_get_mut_id(world, base, pair_id, NULL);
    test_assert(ptr != NULL);
    ptr->x = 100;
    ptr->y = 200;

    const ecs_entity_t *ids = ecs_bulk_new_w_data(world, 3, 
        &(ecs_ids_t){
            .array = (ecs_entity_t[]){ecs_id(Position), ecs_pair(EcsIsA, base)},
            .count = 2
        },
        (void*[]){
            (Position[]){
                {10, 20},
                {30, 40},
                {50, 60}
            },
            NULL
        });

    test_assert(ecs_has(world, ids[0], Position));
    test_assert(ecs_has(world, ids[1], Position));
    test_assert(ecs_has(world, ids[2], Position));

    test_assert(ecs_has_pair(world, ids[0], EcsIsA, base));
    test_assert(ecs_has_pair(world, ids[1], EcsIsA, base));
    test_assert(ecs_has_pair(world, ids[2], EcsIsA, base));

    const Position *
    p = ecs_get(world, ids[0], Position);
    test_int(p->x, 10);
    test_int(p->y, 20);

    p = ecs_get(world, ids[1], Position);
    test_int(p->x, 30);
    test_int(p->y, 40);

    p = ecs_get(world, ids[2], Position);
    test_int(p->x, 50);
    test_int(p->y, 60);

    ecs_fini(world);
}
