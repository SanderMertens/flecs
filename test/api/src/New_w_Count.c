#include <api.h>

void New_w_Count_empty() {
    ecs_world_t *world = ecs_init();

    ecs_entity_t e = ecs_bulk_new(world, 0, 1000);
    test_assert(e != 0);

    ecs_fini(world);
}

void New_w_Count_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e = ecs_bulk_new(world, Position, 1000);
    test_assert(e != 0);
    test_assert(ecs_has(world, e, Position));
    test_int(ecs_count(world, Position), 1000);

    ecs_fini(world);
}

void New_w_Count_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, MyType, Position);

    ecs_entity_t e = ecs_bulk_new(world, MyType, 1000);
    test_assert(e != 0);
    test_int(ecs_count(world, MyType), 1000);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(ecs_has(world, e + i, Position));
    }

    ecs_fini(world);
}

void New_w_Count_type_of_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, MyType, Position, Velocity);

    ecs_entity_t e = ecs_bulk_new(world, MyType, 1000);
    test_assert(e != 0);
    test_int(ecs_count(world, MyType), 1000);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(ecs_has(world, e + i, Position));
        test_assert(ecs_has(world, e + i, Velocity));
    }

    ecs_fini(world);
}

void New_w_Count_type_w_type() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, Type_1, Position);
    ECS_TYPE(world, Type_2, AND | Type_1);

    ecs_entity_t e = ecs_bulk_new(world, Type_2, 1000);
    test_assert(e != 0);
    test_int(ecs_count(world, Type_2), 1000);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(ecs_has(world, e + i, Position));
    }

    ecs_fini(world);
}

void New_w_Count_type_w_2_types() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type_1, Position);
    ECS_TYPE(world, Type_2, Velocity);
    ECS_TYPE(world, Type_3, AND | Type_1, AND | Type_2);

    ecs_entity_t e = ecs_bulk_new(world, Type_3, 1000);
    test_assert(e != 0);
    test_int(ecs_count(world, Type_3), 1000);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(ecs_has(world, e + i, Position));
        test_assert(ecs_has(world, e + i, Velocity));
    }

    ecs_fini(world);
}

void New_w_Count_type_mixed() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type_1, Position);
    ECS_TYPE(world, Type_2, AND | Type_1, Velocity);

    ecs_entity_t e = ecs_bulk_new(world, Type_2, 1000);
    test_assert(e != 0);
    test_int(ecs_count(world, Type_2), 1000);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(ecs_has(world, e + i, Position));
        test_assert(ecs_has(world, e + i, Velocity));
    }

    ecs_fini(world);
}

void New_w_Count_tag() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Tag, 0);

    ecs_entity_t e = ecs_bulk_new_w_entity(world, Tag, 1000);
    test_assert(e != 0);
    test_int(ecs_count_entity(world, Tag), 1000);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(ecs_has_entity(world, e + i, Tag));
    }

    ecs_fini(world);
}

void New_w_Count_type_w_tag() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Tag, 0);
    ECS_TYPE(world, Type, Tag);

    ecs_entity_t e = ecs_bulk_new(world, Type, 1000);
    test_assert(e != 0);
    test_int(ecs_count(world, Type), 1000);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(ecs_has_entity(world, e + i, Tag));
    }

    ecs_fini(world);
}

void New_w_Count_type_w_2_tags() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Tag_1, 0);
    ECS_ENTITY(world, Tag_2, 0);
    ECS_TYPE(world, Type, Tag_1, Tag_2);

    ecs_entity_t e = ecs_bulk_new(world, Type, 1000);
    test_assert(e != 0);
    test_int(ecs_count(world, Type), 1000);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(ecs_has_entity(world, e + i, Tag_1));
        test_assert(ecs_has_entity(world, e + i, Tag_2));
    }

    ecs_fini(world);
}

void New_w_Count_type_w_tag_mixed() {
    ecs_world_t *world = ecs_init();

    ECS_ENTITY(world, Tag_1, 0);
    ECS_ENTITY(world, Tag_2, 0);
    ECS_COMPONENT(world, Position);
    ECS_TYPE(world, Type, Tag_1, Tag_2, Position);

    ecs_entity_t e = ecs_bulk_new(world, Type, 1000);
    test_assert(e != 0);
    test_int(ecs_count(world, Type), 1000);

    int i;
    for (i = 0; i < 1000; i ++) {
        test_assert(ecs_has_entity(world, e + i, Tag_1));
        test_assert(ecs_has_entity(world, e + i, Tag_2));
        test_assert(ecs_has(world, e + i, Position));
    }

    ecs_fini(world);
}

static
void AddPosition(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p, 1);

    ecs_entity_t velocity = *(ecs_entity_t*)it->param;

    int i;
    for (i = it->count - 1; i >= 0; i --) {
        ecs_set_ptr_w_entity(
            it->world, it->entities[i], velocity, 
            sizeof(Velocity), &(Velocity){2, 3});
    }
}

static
void SetPosition(ecs_iter_t *it) {
    ECS_COLUMN(it, Position, p, 1);

    ecs_entity_t rotation = *(ecs_entity_t*)it->param;

    int i;
    for (i = it->count - 1; i >= 0; i --) {
        p[i].y ++;
        ecs_add_entity(it->world, it->entities[i], rotation);
    }
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

    ecs_set(world, AddPosition, EcsContext, {&ecs_entity(Velocity)});
    ecs_set(world, SetPosition, EcsContext, {&ecs_entity(Rotation)});

    ecs_entity_t e = ecs_bulk_new_w_data(world, 3, 
        &(ecs_entities_t){
            .array = (ecs_entity_t[]){
                ecs_entity(Position)
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

    test_assert(e != 0);
    test_int(on_movable_count, 3);

    int i;
    for (i = 0; i < 3; i ++) {
        test_assert( ecs_has(world, e + i, Position) );
        test_assert( ecs_has(world, e + i, Velocity) );
        test_assert( ecs_has(world, e + i, Rotation) );

        const Position *p = ecs_get(world, e + i, Position);
        test_assert(p != NULL);
        test_int(p->x, 10 + 20 * i);
        test_int(p->y, 20 + i * 20 + 1);

        const Velocity *v = ecs_get(world, e + i, Velocity);
        test_assert(v != NULL);
        test_int(v->x, 2);
        test_int(v->y, 3);        
    }

    ecs_fini(world);
}
