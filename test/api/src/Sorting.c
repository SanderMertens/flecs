#include <api.h>

int compare_position(
    ecs_entity_t e1,
    void *ptr1,
    ecs_entity_t e2,
    void *ptr2)
{
    Position *p1 = ptr1;
    Position *p2 = ptr2;
    return p1->x - p2->x;
}

int compare_entity(
    ecs_entity_t e1,
    void *ptr1,
    ecs_entity_t e2,
    void *ptr2)
{
    return e1 - e2;
}

void Sorting_sort_by_component() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {3, 0});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {1, 0});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {5, 0});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {2, 0});
    ecs_entity_t e5 = ecs_set(world, 0, Position, {4, 0});

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_query_order_by(world, q, ecs_entity(Position), compare_position);

    ecs_iter_t it = ecs_query_iter(q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 5);

    test_assert(it.entities[0] == e2);
    test_assert(it.entities[1] == e4);
    test_assert(it.entities[2] == e1);
    test_assert(it.entities[3] == e5);
    test_assert(it.entities[4] == e3);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Sorting_sort_by_component_same_value_1() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {3, 0});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {3, 0});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {2, 0});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {2, 0});
    ecs_entity_t e5 = ecs_set(world, 0, Position, {1, 0});

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_query_order_by(world, q, ecs_entity(Position), compare_position);

    ecs_iter_t it = ecs_query_iter(q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 5);
    

    test_assert(it.entities[0] == e5);
    test_assert(it.entities[1] == e4);
    test_assert(it.entities[2] == e3);
    test_assert(it.entities[3] == e2);
    test_assert(it.entities[4] == e1);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Sorting_sort_by_component_same_value_2() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {2, 0});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {1, 0});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {2, 0});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {1, 0});
    ecs_entity_t e5 = ecs_set(world, 0, Position, {2, 0});
    ecs_entity_t e6 = ecs_set(world, 0, Position, {1, 0});

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_query_order_by(world, q, ecs_entity(Position), compare_position);

    ecs_iter_t it = ecs_query_iter(q);

    test_assert(ecs_query_next(&it));

    test_int(it.count, 6);
    test_assert(it.entities[0] == e4);
    test_assert(it.entities[1] == e6);
    test_assert(it.entities[2] == e2);
    test_assert(it.entities[3] == e1);
    test_assert(it.entities[4] == e3);
    test_assert(it.entities[5] == e5);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Sorting_sort_by_component_2_tables() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {3, 0});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {1, 0});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {5, 0});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {2, 0});
    ecs_entity_t e5 = ecs_set(world, 0, Position, {4, 0});

    ecs_add(world, e3, Velocity);
    ecs_add(world, e4, Velocity);
    ecs_add(world, e5, Velocity);

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_query_order_by(world, q, ecs_entity(Position), compare_position);

    ecs_iter_t it = ecs_query_iter(q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_assert(it.entities[0] == e2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_assert(it.entities[0] == e4);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_assert(it.entities[0] == e1);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 2);
    test_assert(it.entities[0] == e5);
    test_assert(it.entities[1] == e3);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Sorting_sort_by_component_3_tables() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {3, 0});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {1, 0});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {6, 0});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {2, 0});
    ecs_entity_t e5 = ecs_set(world, 0, Position, {4, 0});
    ecs_entity_t e6 = ecs_set(world, 0, Position, {5, 0});
    ecs_entity_t e7 = ecs_set(world, 0, Position, {7, 0});

    ecs_add(world, e5, Velocity);
    ecs_add(world, e6, Mass);
    ecs_add(world, e7, Mass);

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_query_order_by(world, q, ecs_entity(Position), compare_position);

    ecs_iter_t it = ecs_query_iter(q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 3);
    test_assert(it.entities[0] == e2);
    test_assert(it.entities[1] == e4);
    test_assert(it.entities[2] == e1);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_assert(it.entities[0] == e5);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_assert(it.entities[0] == e6);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);    
    test_assert(it.entities[0] == e3);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);    
    test_assert(it.entities[0] == e7);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}


void Sorting_sort_by_entity() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {3, 0});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {1, 0});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {5, 0});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {2, 0});
    ecs_entity_t e5 = ecs_set(world, 0, Position, {4, 0});

    ecs_add(world, e2, Velocity);
    ecs_add(world, e4, Velocity);
    ecs_add(world, e5, Velocity);

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_query_order_by(world, q, 0, compare_entity);

    ecs_iter_t it = ecs_query_iter(q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_assert(it.entities[0] == e1);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_assert(it.entities[0] == e2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_assert(it.entities[0] == e3);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 2);
    test_assert(it.entities[0] == e4);
    test_assert(it.entities[1] == e5);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Sorting_sort_after_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {3, 0});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {1, 0});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {6, 0});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {2, 0});
    ecs_entity_t e5 = ecs_set(world, 0, Position, {5, 0});

    ecs_add(world, e3, Velocity);
    ecs_add(world, e4, Velocity);
    ecs_add(world, e5, Velocity);

    ecs_add(world, e3, Mass);

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity");
    ecs_query_order_by(world, q, ecs_entity(Position), compare_position);

    ecs_iter_t it = ecs_query_iter(q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 2);
    test_assert(it.entities[0] == e4);
    test_assert(it.entities[1] == e5);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);    
    test_assert(it.entities[0] == e3);

    test_assert(!ecs_query_next(&it));

    ecs_add(world, e1, Velocity);

    it = ecs_query_iter(q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 3);
    test_assert(it.entities[0] == e4);
    test_assert(it.entities[1] == e1);
    test_assert(it.entities[2] == e5);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);    
    test_assert(it.entities[0] == e3);

    test_assert(!ecs_query_next(&it)); 

    ecs_add(world, e2, Velocity);   
    ecs_add(world, e2, Mass);

    it = ecs_query_iter(q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_assert(it.entities[0] == e2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 3);
    test_assert(it.entities[0] == e4);
    test_assert(it.entities[1] == e1);
    test_assert(it.entities[2] == e5);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);    
    test_assert(it.entities[0] == e3);

    test_assert(!ecs_query_next(&it));     

    ecs_fini(world);
}

void Sorting_sort_after_remove() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {3, 0});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {1, 0});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {6, 0});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {2, 0});
    ecs_entity_t e5 = ecs_set(world, 0, Position, {5, 0});

    ecs_add(world, e1, Velocity);
    ecs_add(world, e2, Velocity);   

    ecs_add(world, e3, Velocity);
    ecs_add(world, e4, Velocity);
    ecs_add(world, e5, Velocity);

    ecs_add(world, e2, Mass);
    ecs_add(world, e3, Mass);

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity");
    ecs_query_order_by(world, q, ecs_entity(Position), compare_position);

    ecs_iter_t it = ecs_query_iter(q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_assert(it.entities[0] == e2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 3);
    test_assert(it.entities[0] == e4);
    test_assert(it.entities[1] == e1);
    test_assert(it.entities[2] == e5);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);    
    test_assert(it.entities[0] == e3);

    test_assert(!ecs_query_next(&it));  

    ecs_remove(world, e1, Velocity);

    it = ecs_query_iter(q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_assert(it.entities[0] == e2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 2);
    test_assert(it.entities[0] == e4);
    test_assert(it.entities[1] == e5);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);    
    test_assert(it.entities[0] == e3);

    test_assert(!ecs_query_next(&it)); 

    ecs_remove(world, e2, Mass); 

    it = ecs_query_iter(q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 3);
    test_assert(it.entities[0] == e2);
    test_assert(it.entities[1] == e4);
    test_assert(it.entities[2] == e5);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);    
    test_assert(it.entities[0] == e3);

    test_assert(!ecs_query_next(&it));

    ecs_remove(world, e2, Velocity); 

    it = ecs_query_iter(q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 2);
    test_assert(it.entities[0] == e4);
    test_assert(it.entities[1] == e5);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);    
    test_assert(it.entities[0] == e3);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Sorting_sort_after_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {3, 0});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {1, 0});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {6, 0});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {2, 0});
    ecs_entity_t e5 = ecs_set(world, 0, Position, {5, 0});

    ecs_add(world, e1, Velocity);
    ecs_add(world, e2, Velocity);   

    ecs_add(world, e3, Velocity);
    ecs_add(world, e4, Velocity);
    ecs_add(world, e5, Velocity);

    ecs_add(world, e2, Mass);
    ecs_add(world, e3, Mass);

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity");
    ecs_query_order_by(world, q, ecs_entity(Position), compare_position);

    ecs_iter_t it = ecs_query_iter(q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_assert(it.entities[0] == e2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 3);
    test_assert(it.entities[0] == e4);
    test_assert(it.entities[1] == e1);
    test_assert(it.entities[2] == e5);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);    
    test_assert(it.entities[0] == e3);

    test_assert(!ecs_query_next(&it));  

    ecs_delete(world, e1);

    it = ecs_query_iter(q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_assert(it.entities[0] == e2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 2);
    test_assert(it.entities[0] == e4);
    test_assert(it.entities[1] == e5);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);    
    test_assert(it.entities[0] == e3);

    test_assert(!ecs_query_next(&it)); 

    ecs_delete(world, e2); 

    it = ecs_query_iter(q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 2);
    test_assert(it.entities[0] == e4);
    test_assert(it.entities[1] == e5);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);    
    test_assert(it.entities[0] == e3);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Sorting_sort_after_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {3, 0});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {1, 0});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {6, 0});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {2, 0});
    ecs_entity_t e5 = ecs_set(world, 0, Position, {5, 0});

    ecs_add(world, e1, Velocity);
    ecs_add(world, e2, Velocity);   

    ecs_add(world, e3, Velocity);
    ecs_add(world, e4, Velocity);
    ecs_add(world, e5, Velocity);

    ecs_add(world, e2, Mass);
    ecs_add(world, e3, Mass);

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity");
    ecs_query_order_by(world, q, ecs_entity(Position), compare_position);

    ecs_iter_t it = ecs_query_iter(q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_assert(it.entities[0] == e2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 3);
    test_assert(it.entities[0] == e4);
    test_assert(it.entities[1] == e1);
    test_assert(it.entities[2] == e5);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);    
    test_assert(it.entities[0] == e3);

    test_assert(!ecs_query_next(&it));  

    ecs_set(world, e1, Position, {7, 0});

    it = ecs_query_iter(q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_assert(it.entities[0] == e2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 2);
    test_assert(it.entities[0] == e4);
    test_assert(it.entities[1] == e5);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);    
    test_assert(it.entities[0] == e3);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);    
    test_assert(it.entities[0] == e1);

    test_assert(!ecs_query_next(&it)); 

    ecs_fini(world);
}

static
void FlipP(ecs_iter_t *it) {
    Position *p = ecs_column(it, Position, 1);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        float x = p[i].x;
        p[i].x = p[i].y;
        p[i].y = x;
    }
}

void Sorting_sort_after_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ECS_SYSTEM(world, FlipP, EcsOnUpdate, Position);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {3, 5});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {1, 2});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {6, 6});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {2, 1});
    ecs_entity_t e5 = ecs_set(world, 0, Position, {5, 3});

    ecs_add(world, e1, Velocity);
    ecs_add(world, e2, Velocity);   

    ecs_add(world, e3, Velocity);
    ecs_add(world, e4, Velocity);
    ecs_add(world, e5, Velocity);

    ecs_add(world, e2, Mass);
    ecs_add(world, e3, Mass);

    ecs_query_t *q = ecs_query_new(world, "Position, Velocity");
    ecs_query_order_by(world, q, ecs_entity(Position), compare_position);

    ecs_iter_t it = ecs_query_iter(q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_assert(it.entities[0] == e2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 3);
    test_assert(it.entities[0] == e4);
    test_assert(it.entities[1] == e1);
    test_assert(it.entities[2] == e5);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);    
    test_assert(it.entities[0] == e3);

    test_assert(!ecs_query_next(&it));

    ecs_progress(world, 0);

    it = ecs_query_iter(q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_assert(it.entities[0] == e4);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_assert(it.entities[0] == e2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 2);    
    test_assert(it.entities[0] == e5);
    test_assert(it.entities[1] == e1);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);    
    test_assert(it.entities[0] == e3);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Sorting_sort_by_component_move_pivot() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {5, 0});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {10, 0});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {2, 0});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {1, 0});

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_query_order_by(world, q, ecs_entity(Position), compare_position);

    ecs_iter_t it = ecs_query_iter(q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 4);

    test_assert(it.entities[0] == e4);
    test_assert(it.entities[1] == e3);
    test_assert(it.entities[2] == e1);
    test_assert(it.entities[3] == e2);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Sorting_sort_1000_entities() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position");
    ecs_query_order_by(world, q, ecs_entity(Position), compare_position);

    for (int i = 0; i < 1000; i ++) {
        int32_t v = rand();
        ecs_set(world, 0, Position, {v});
        ecs_set(world, 0, Position, {v});

        ecs_iter_t it = ecs_query_iter(q);
        while (ecs_query_next(&it)) {
            Position *p = ecs_column(&it, Position, 1);

            test_assert(it.count == (i + 1) * 2);

            int32_t j, x = 0;
            for (j = 0; j < it.count; j ++) {  
                test_assert(x <= p[j].x);
                x = p[j].x;
            }
        }
    }

    ecs_fini(world);
}
