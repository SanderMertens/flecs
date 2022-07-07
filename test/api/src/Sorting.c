#include <api.h>
#include <stdlib.h>

static
int compare_position(
    ecs_entity_t e1,
    const void *ptr1,
    ecs_entity_t e2,
    const void *ptr2)
{
    const Position *p1 = ptr1;
    const Position *p2 = ptr2;
    return (p1->x > p2->x) - (p1->x < p2->x);
}

static
int compare_entity(
    ecs_entity_t e1,
    const void *ptr1,
    ecs_entity_t e2,
    const void *ptr2)
{
    return (e1 > e2) - (e1 < e2);
}

void Sorting_sort_by_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {3, 0});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {1, 0});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {5, 0});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {2, 0});
    ecs_entity_t e5 = ecs_set(world, 0, Position, {4, 0});

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position",
        .order_by_component = ecs_id(Position),
        .order_by = compare_position
    });

    ecs_iter_t it = ecs_query_iter(world, q);

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
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {3, 0});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {3, 0});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {2, 0});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {2, 0});
    ecs_entity_t e5 = ecs_set(world, 0, Position, {1, 0});

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position",
        .order_by_component = ecs_id(Position),
        .order_by = compare_position
    });

    ecs_iter_t it = ecs_query_iter(world, q);

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
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {2, 0});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {1, 0});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {2, 0});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {1, 0});
    ecs_entity_t e5 = ecs_set(world, 0, Position, {2, 0});
    ecs_entity_t e6 = ecs_set(world, 0, Position, {1, 0});

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position",
        .order_by_component = ecs_id(Position),
        .order_by = compare_position
    });

    ecs_iter_t it = ecs_query_iter(world, q);

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
    ecs_world_t *world = ecs_mini();

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

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position",
        .order_by_component = ecs_id(Position),
        .order_by = compare_position
    });

    ecs_iter_t it = ecs_query_iter(world, q);

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
    ecs_world_t *world = ecs_mini();

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

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position",
        .order_by_component = ecs_id(Position),
        .order_by = compare_position
    });

    ecs_iter_t it = ecs_query_iter(world, q);

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
    ecs_world_t *world = ecs_mini();

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

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position",
        .order_by = compare_entity
    });

    ecs_iter_t it = ecs_query_iter(world, q);

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
    ecs_world_t *world = ecs_mini();

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

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position, Velocity",
        .order_by_component = ecs_id(Position),
        .order_by = compare_position
    });

    ecs_iter_t it = ecs_query_iter(world, q);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 2);
    test_assert(it.entities[0] == e4);
    test_assert(it.entities[1] == e5);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);    
    test_assert(it.entities[0] == e3);

    test_assert(!ecs_query_next(&it));

    ecs_add(world, e1, Velocity);

    it = ecs_query_iter(world, q);

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

    it = ecs_query_iter(world, q);

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
    ecs_world_t *world = ecs_mini();

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

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position, Velocity",
        .order_by_component = ecs_id(Position),
        .order_by = compare_position
    });

    ecs_iter_t it = ecs_query_iter(world, q);

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

    it = ecs_query_iter(world, q);

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

    it = ecs_query_iter(world, q);

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

    it = ecs_query_iter(world, q);

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
    ecs_world_t *world = ecs_mini();

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

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position, Velocity",
        .order_by_component = ecs_id(Position),
        .order_by = compare_position
    });

    ecs_iter_t it = ecs_query_iter(world, q);

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

    it = ecs_query_iter(world, q);

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

    it = ecs_query_iter(world, q);

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
    ecs_world_t *world = ecs_mini();

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

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position, Velocity",
        .order_by_component = ecs_id(Position),
        .order_by = compare_position
    });

    ecs_iter_t it = ecs_query_iter(world, q);

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

    it = ecs_query_iter(world, q);

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
    Position *p = ecs_term(it, Position, 1);

    int32_t i;
    for (i = 0; i < it->count; i ++) {
        float x = p[i].x;
        p[i].x = p[i].y;
        p[i].y = x;
    }
}

void Sorting_sort_after_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);
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

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position, [in] Velocity",
        .order_by_component = ecs_id(Position),
        .order_by = compare_position
    });

    ecs_iter_t it = ecs_query_iter(world, q);

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

    it = ecs_query_iter(world, q);

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

    ecs_progress(world, 0);

    /* Second iteration, query now needs to check dirty admin to see if system
     * updated component */
    it = ecs_query_iter(world, q);

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

void Sorting_sort_after_query() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Mass);

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

    ecs_query_t *flip_q = ecs_query_new(world, "Position");
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position, [in] Velocity",
        .order_by_component = ecs_id(Position),
        .order_by = compare_position
    });

    ecs_iter_t it = ecs_query_iter(world, q);

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

    ecs_iter_t qit = ecs_query_iter(world, flip_q);
    while (ecs_query_next(&qit)) {
        FlipP(&qit);
    }

    /* First iteration, query will register monitor with table, so table is
     * always marked dirty */
    it = ecs_query_iter(world, q);

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

    qit = ecs_query_iter(world, flip_q);
    while (ecs_query_next(&qit)) {
        FlipP(&qit);
    }

    /* Second iteration, query now needs to check dirty admin to see if system
     * updated component */
    it = ecs_query_iter(world, q);

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

void Sorting_sort_by_component_move_pivot() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t e1 = ecs_set(world, 0, Position, {5, 0});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {10, 0});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {2, 0});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {1, 0});

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position",
        .order_by_component = ecs_id(Position),
        .order_by = compare_position
    });

    ecs_iter_t it = ecs_query_iter(world, q);

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
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position",
        .order_by_component = ecs_id(Position),
        .order_by = compare_position
    });

    for (int i = 0; i < 1000; i ++) {
        int32_t v = rand();
        ecs_set(world, 0, Position, {v});

        int32_t x = 0;

        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it)) {
            Position *p = ecs_term(&it, Position, 1);

            test_assert(it.count == (i + 1));

            int32_t j;
            for (j = 0; j < it.count; j ++) {  
                test_assert(x <= p[j].x);
                x = p[j].x;
            }
        }
    }

    ecs_fini(world);
}

void Sorting_sort_1000_entities_w_duplicates() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position",
        .order_by_component = ecs_id(Position),
        .order_by = compare_position
    });

    for (int i = 0; i < 500; i ++) {
        int32_t v = rand();
        ecs_set(world, 0, Position, {v});
        ecs_set(world, 0, Position, {v});

        int32_t x = 0;

        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it)) {
            Position *p = ecs_term(&it, Position, 1);

            test_assert(it.count == (i + 1) * 2);

            int32_t j;
            for (j = 0; j < it.count; j ++) {  
                test_assert(x <= p[j].x);
                x = p[j].x;
            }
        }
    }

    ecs_fini(world);
}

void Sorting_sort_1000_entities_again() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position",
        .order_by_component = ecs_id(Position),
        .order_by = compare_position
    });

    ecs_entity_t start = ecs_new(world, 0);

    for (int i = 0; i < 1000; i ++) {
        int32_t v = rand();
        ecs_set(world, i + start, Position, {v});

        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it)) {
            Position *p = ecs_term(&it, Position, 1);

            test_assert(it.count == (i + 1));

            int32_t j, x = 0;
            for (j = 0; j < it.count; j ++) {  
                test_assert(x <= p[j].x);
                x = p[j].x;
            }
        }
    }

    for (int i = 0; i < 1000; i ++) {
        int32_t v = rand();
        ecs_set(world, i + start, Position, {v});
    }

    int32_t x = 0;

    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) {
        Position *p = ecs_term(&it, Position, 1);

        test_assert(it.count == 1000);

        int32_t j;
        for (j = 0; j < it.count; j ++) {  
            test_assert(x <= p[j].x);
            x = p[j].x;
        }
    }    


    ecs_fini(world);
}

void Sorting_sort_1000_entities_2_types() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position",
        .order_by_component = ecs_id(Position),
        .order_by = compare_position
    });

    for (int i = 0; i < 500; i ++) {
        int32_t v = rand();
        ecs_set(world, 0, Position, {v});
        ecs_entity_t e = ecs_set(world, 0, Position, {v});
        ecs_add(world, e, Velocity);

        ecs_iter_t it = ecs_query_iter(world, q);
        int32_t count = 0, x = 0;
        while (ecs_query_next(&it)) {
            Position *p = ecs_term(&it, Position, 1);

            count += it.count;

            int32_t j;
            for (j = 0; j < it.count; j ++) {  
                test_assert(x <= p[j].x);
                x = p[j].x;
            }
        }

        test_int(count, (i + 1) * 2);
    }

    ecs_fini(world);
}

void Sorting_sort_1000_entities_2_types_again() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position",
        .order_by_component = ecs_id(Position),
        .order_by = compare_position
    });

    ecs_entity_t start = ecs_new(world, 0);

    for (int i = 0; i < 1000; i ++) {
        int32_t v = rand();
        ecs_set(world, i + start, Position, {v});

        if (!(i % 2)) {
            ecs_add(world, i + start, Velocity);
        }

        ecs_iter_t it = ecs_query_iter(world, q);
        while (ecs_query_next(&it)) {
            Position *p = ecs_term(&it, Position, 1);

            int32_t j, x = 0;
            for (j = 0; j < it.count; j ++) {  
                test_assert(x <= p[j].x);
                x = p[j].x;
            }
        }
    }

    for (int i = 0; i < 1000; i ++) {
        int32_t v = rand();
        ecs_set(world, i + start, Position, {v});
    }

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0, x = 0;
    while (ecs_query_next(&it)) {
        Position *p = ecs_term(&it, Position, 1);

        count += it.count;

        int32_t j;
        for (j = 0; j < it.count; j ++) {  
            test_assert(x <= p[j].x);
            x = p[j].x;
        }
    }

    test_int(count, 1000);

    ecs_fini(world);
}

void Sorting_sort_1000_entities_add_type_after_sort() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position",
        .order_by_component = ecs_id(Position),
        .order_by = compare_position
    });

    ecs_entity_t start = ecs_new(world, 0);

    for (int i = 0; i < 500; i ++) {
        int32_t v = rand();
        ecs_set(world, i + start, Position, {v});

        ecs_iter_t it = ecs_query_iter(world, q);
        int32_t count = 0;
        while (ecs_query_next(&it)) {
            Position *p = ecs_term(&it, Position, 1);

            count += it.count;

            int32_t j, x = 0;
            for (j = 0; j < it.count; j ++) {  
                test_assert(x <= p[j].x);
                x = p[j].x;
            }
        }

        test_int(count, i + 1);
    }

    for (int i = 0; i < 500; i ++) {
        int32_t v = rand();
        ecs_set(world, i + start, Position, {v});
    }

    for (int i = 0; i < 500; i ++) {
        int32_t v = rand();
        ecs_set(world, i + start + 500, Position, {v});
        ecs_add(world, i + start + 500, Velocity);

        ecs_iter_t it = ecs_query_iter(world, q);
        int32_t count = 0, x = 0;
        while (ecs_query_next(&it)) {
            Position *p = ecs_term(&it, Position, 1);

            count += it.count;

            int32_t j;
            for (j = 0; j < it.count; j ++) {  
                test_assert(x <= p[j].x);
                x = p[j].x;
            }
        }

        test_int(count, i + 500 + 1);
    }    

    ecs_fini(world);
}

void Sorting_sort_1500_entities_3_types() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position",
        .order_by_component = ecs_id(Position),
        .order_by = compare_position
    });

    for (int i = 0; i < 500; i ++) {
        ecs_set(world, 0, Position, {rand()});
        ecs_entity_t e = ecs_set(world, 0, Position, {rand()});
        ecs_add(world, e, Velocity);

        e = ecs_set(world, 0, Position, {rand()});
        ecs_add(world, e, Mass);

        ecs_iter_t it = ecs_query_iter(world, q);
        int32_t count = 0, x = 0;
        while (ecs_query_next(&it)) {
            Position *p = ecs_term(&it, Position, 1);

            count += it.count;

            int32_t j;
            for (j = 0; j < it.count; j ++) {  
                test_assert(x <= p[j].x);
                x = p[j].x;
            }
        }

        test_int(count, (i + 1) * 3);
    }

    ecs_fini(world);
}

void Sorting_sort_2000_entities_4_types() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position",
        .order_by_component = ecs_id(Position),
        .order_by = compare_position
    });

    for (int i = 0; i < 500; i ++) {
        ecs_set(world, 0, Position, {rand()});

        ecs_entity_t e = ecs_set(world, 0, Position, {rand()});
        ecs_add(world, e, Velocity);

        e = ecs_set(world, 0, Position, {rand()});
        ecs_add(world, e, Mass);

        e = ecs_set(world, 0, Position, {rand()});
        ecs_add(world, e, Velocity);
        ecs_add(world, e, Mass);        

        ecs_iter_t it = ecs_query_iter(world, q);
        int32_t count = 0, x = 0;
        while (ecs_query_next(&it)) {
            Position *p = ecs_term(&it, Position, 1);

            count += it.count;

            int32_t j;
            for (j = 0; j < it.count; j ++) {  
                test_assert(x <= p[j].x);
                x = p[j].x;
            }
        }

        test_int(count, (i + 1) * 4);
    }

    ecs_fini(world);
}

void Sorting_sort_shared_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);

    ecs_entity_t base_1 = ecs_set(world, 0, Position, {0, 0});
    ecs_entity_t base_2 = ecs_set(world, 0, Position, {3, 0});
    ecs_entity_t base_3 = ecs_set(world, 0, Position, {7, 0});

    ecs_entity_t e1 = ecs_set(world, 0, Position, {6, 0});
    ecs_entity_t e2 = ecs_set(world, 0, Position, {1, 0});
    ecs_entity_t e3 = ecs_set(world, 0, Position, {5, 0});
    ecs_entity_t e4 = ecs_set(world, 0, Position, {2, 0});
    ecs_entity_t e5 = ecs_set(world, 0, Position, {4, 0});
    ecs_entity_t e6 = ecs_new_w_pair(world, EcsIsA, base_3);
    ecs_entity_t e7 = ecs_new_w_pair(world, EcsIsA, base_2);
    ecs_entity_t e8 = ecs_new_w_pair(world, EcsIsA, base_1);
    ecs_entity_t e9 = ecs_new_w_pair(world, EcsIsA, base_1);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position(self|super)",
        .filter.instanced = true,
        .order_by_component = ecs_id(Position),
        .order_by = compare_position,
    });

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_assert(it.entities[0] == base_1);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 2);
    test_assert(it.entities[0] == e8);
    test_assert(it.entities[1] == e9);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 3);
    test_assert(it.entities[0] == e2);
    test_assert(it.entities[1] == e4);
    test_assert(it.entities[2] == base_2);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_assert(it.entities[0] == e7);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 4);
    test_assert(it.entities[0] == e5);
    test_assert(it.entities[1] == e3);
    test_assert(it.entities[2] == e1);
    test_assert(it.entities[3] == base_3);

    test_assert(ecs_query_next(&it));
    test_int(it.count, 1);
    test_assert(it.entities[0] == e6);

    test_assert(!ecs_query_next(&it));

    ecs_fini(world);
}

void Sorting_sort_2_entities_2_types() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position",
        .order_by_component = ecs_id(Position),
        .order_by = compare_position
    });

    ecs_set(world, 0, Position, {rand()});

    ecs_entity_t e = ecs_set(world, 0, Position, {rand()});
    ecs_add(world, e, Velocity); 

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0;
    while (ecs_query_next(&it)) {
        Position *p = ecs_term(&it, Position, 1);

        count += it.count;

        int32_t j, x = 0;
        for (j = 0; j < it.count; j ++) {  
            test_assert(x <= p[j].x);
            x = p[j].x;
        }
    }

    test_int(count, 2);

    ecs_fini(world);
}

void Sorting_sort_3_entities_3_types() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position",
        .order_by_component = ecs_id(Position),
        .order_by = compare_position
    });

    ecs_set(world, 0, Position, {rand()});

    ecs_entity_t e = ecs_set(world, 0, Position, {rand()});
    ecs_add(world, e, Velocity);

    e = ecs_set(world, 0, Position, {rand()});
    ecs_add(world, e, Mass);   

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0;
    while (ecs_query_next(&it)) {
        Position *p = ecs_term(&it, Position, 1);

        count += it.count;

        int32_t j, x = 0;
        for (j = 0; j < it.count; j ++) {  
            test_assert(x <= p[j].x);
            x = p[j].x;
        }
    }

    test_int(count, 3);

    ecs_fini(world);
}

void Sorting_sort_3_entities_3_types_2() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Foo);
    ECS_TAG(world, Bar);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Tag",
        .order_by = compare_entity
    });

    ecs_entity_t e1 = ecs_new(world, Tag);
    ecs_entity_t e2 = ecs_new(world, Tag);
    ecs_entity_t e3 = ecs_new(world, Tag);

    ecs_add_id(world, e1, Foo);
    ecs_add_id(world, e2, Bar);

    ecs_iter_t it = ecs_query_iter(world, q);
    ecs_entity_t e = 0;
    int32_t count = 0;
    while (ecs_query_next(&it)) {
        count += it.count;

        int32_t i;
        for (i = 0; i < it.count; i ++) {  
            test_assert(e < it.entities[i]);
            e = it.entities[i];
            test_assert(e == e1 || e == e2 || e == e3);
        }
    }

    test_int(count, 3);

    ecs_fini(world);
}

void Sorting_sort_4_entities_4_types() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_COMPONENT(world, Mass);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "Position",
        .order_by_component = ecs_id(Position),
        .order_by = compare_position
    });

    ecs_set(world, 0, Position, {rand()});

    ecs_entity_t e = ecs_set(world, 0, Position, {rand()});
    ecs_add(world, e, Velocity);

    e = ecs_set(world, 0, Position, {rand()});
    ecs_add(world, e, Mass);

    e = ecs_set(world, 0, Position, {rand()});
    ecs_add(world, e, Velocity);
    ecs_add(world, e, Mass);     

    ecs_iter_t it = ecs_query_iter(world, q);
    int32_t count = 0;
    while (ecs_query_next(&it)) {
        Position *p = ecs_term(&it, Position, 1);

        count += it.count;

        int32_t j, x = 0;
        for (j = 0; j < it.count; j ++) {  
            test_assert(x <= p[j].x);
            x = p[j].x;
        }
    }

    test_int(count, 4);

    ecs_fini(world);
}

void Sorting_sort_w_tags_only() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.terms = {{Tag}},
        .order_by = compare_entity
    });

    ecs_entity_t root = ecs_new_id(world);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, root);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, root);

    ecs_add(world, e2, Tag);
    ecs_add(world, e1, Tag);

    ecs_iter_t it = ecs_query_iter(world, q);
    test_assert(ecs_query_next(&it));
    test_int(it.count, 2);
    test_int(it.entities[0], e1);
    test_int(it.entities[1], e2);

    ecs_fini(world);
}

void Sorting_sort_childof_marked() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.terms = {{Tag}},
        .order_by = compare_entity
    });

    ecs_entity_t root = ecs_new_id(world);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsChildOf, root);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsChildOf, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsChildOf, root);

    ecs_add(world, e3, Tag);
    ecs_add(world, e1, Tag);

    // Trigger sorting
    ecs_query_iter(world, q);

    ecs_delete(world, root);

    test_assert(!ecs_is_alive(world, root));
    test_assert(!ecs_is_alive(world, e1));
    test_assert(!ecs_is_alive(world, e2));
    test_assert(!ecs_is_alive(world, e3));

    ecs_fini(world);
}

void Sorting_sort_isa_marked() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.terms = {{Tag}},
        .order_by = compare_entity
    });

    ecs_entity_t root = ecs_new_id(world);
    ecs_entity_t e1 = ecs_new_w_pair(world, EcsIsA, root);
    ecs_entity_t e2 = ecs_new_w_pair(world, EcsIsA, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, EcsIsA, root);

    ecs_add(world, e3, Tag);
    ecs_add(world, e1, Tag);

    // Trigger sorting
    ecs_query_iter(world, q);

    ecs_delete(world, root);

    test_assert(!ecs_is_alive(world, root));
    test_int(1, ecs_get_type(world, e1)->count);
    test_assert(ecs_has(world, e1, Tag));
    test_int(1, ecs_get_type(world, e2)->count);
    test_assert(ecs_has_pair(world, e2, EcsIsA, e1));
    test_int(1, ecs_get_type(world, e3)->count);
    test_assert(ecs_has(world, e3, Tag));

    ecs_fini(world);
}

void Sorting_sort_relation_marked() {
    ecs_world_t *world = ecs_mini();

    ECS_TAG(world, Tag);
    ECS_TAG(world, Rel);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.terms = {{Tag}},
        .order_by = compare_entity
    });

    ecs_entity_t root = ecs_new_id(world);
    ecs_entity_t e1 = ecs_new_w_pair(world, Rel, root);
    ecs_entity_t e2 = ecs_new_w_pair(world, Rel, e1);
    ecs_entity_t e3 = ecs_new_w_pair(world, Rel, root);

    ecs_add(world, e3, Tag);
    ecs_add(world, e1, Tag);

    // Trigger sorting
    ecs_query_iter(world, q);

    ecs_delete(world, root);

    test_assert(!ecs_is_alive(world, root));
    test_int(1, ecs_get_type(world, e1)->count);
    test_assert(ecs_has(world, e1, Tag));
    test_int(1, ecs_get_type(world, e2)->count);
    test_assert(ecs_has_pair(world, e2, Rel, e1));
    test_int(1, ecs_get_type(world, e3)->count);
    test_assert(ecs_has(world, e3, Tag));

    ecs_fini(world);
}

static bool dummy_compare_invoked = false;

static
int dummy_compare(
    ecs_entity_t e1,
    const void *ptr1,
    ecs_entity_t e2,
    const void *ptr2)
{
    dummy_compare_invoked = true;
    return (e1 > e2) - (e1 < e2);
}

void Sorting_dont_resort_after_set_unsorted_component() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "[in] Position",
        .order_by_component = ecs_id(Position),
        .order_by = dummy_compare
    });

    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_set(world, e1, Position, {0, 0});
    ecs_set(world, e1, Velocity, {0, 0});

    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_set(world, e2, Position, {0, 0});
    ecs_set(world, e2, Velocity, {0, 0});

    // Initial sort
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) { }
    test_bool(dummy_compare_invoked, true); 
    dummy_compare_invoked = false;

    // No changes, shouldn't sort
    it = ecs_query_iter(world, q);
    test_bool(dummy_compare_invoked, false);

    // No change in sorted component, shouldn't sort
    ecs_modified(world, e2, Velocity);
    ecs_query_iter(world, q);
    test_bool(dummy_compare_invoked, false);

    // Change in sorted component, should sort
    ecs_modified(world, e2, Position);
    ecs_query_iter(world, q);
    test_bool(dummy_compare_invoked, true);

    ecs_fini(world);
}

void Sorting_dont_resort_after_set_unsorted_component_w_tag() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Velocity);

    ecs_entity_t Tag = ecs_new_low_id(world); // wiggle id between components

    ECS_COMPONENT(world, Position);

    /* Because Position's id is larger than the Tag, its type index in the
     * storage table is different from the actual table. Ensure that this 
     * doesn't break the test. */

    test_assert(Tag < ecs_id(Position));

    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "[in] Position",
        .order_by_component = ecs_id(Position),
        .order_by = dummy_compare
    });

    ecs_entity_t e1 = ecs_new_w_id(world, Tag);
    ecs_set(world, e1, Position, {0, 0});
    ecs_set(world, e1, Velocity, {0, 0});

    ecs_entity_t e2 = ecs_new_w_id(world, Tag);
    ecs_set(world, e2, Position, {0, 0});
    ecs_set(world, e2, Velocity, {0, 0});

    // Initial sort
    ecs_iter_t it = ecs_query_iter(world, q);
    while (ecs_query_next(&it)) { }
    test_bool(dummy_compare_invoked, true);
    dummy_compare_invoked = false;

    // No changes, shouldn't sort
    ecs_query_iter(world, q);
    test_bool(dummy_compare_invoked, false);

    // No change in sorted component, shouldn't sort
    ecs_modified(world, e2, Velocity);
    ecs_query_iter(world, q);
    test_bool(dummy_compare_invoked, false);

    // Change in sorted component, should sort
    ecs_modified(world, e2, Position);
    ecs_query_iter(world, q);
    test_bool(dummy_compare_invoked, true);

    ecs_fini(world);
}

void Sorting_dont_resort_after_set_unsorted_component_w_tag_w_out_term() {
    ecs_world_t *world = ecs_mini();

    ECS_COMPONENT(world, Velocity);

    ecs_entity_t Tag = ecs_new_low_id(world); // wiggle id between components

    ECS_COMPONENT(world, Position);

    /* Same as previous testcase, but with query out column vs. modified */

    test_assert(Tag < ecs_id(Position));

    // Sorted query
    ecs_query_t *q = ecs_query_init(world, &(ecs_query_desc_t){
        .filter.expr = "[in] Position",
        .order_by_component = ecs_id(Position),
        .order_by = dummy_compare
    });

    // Dummy queries that mutate
    ecs_query_t *q_a = ecs_query_new(world, "Position"); // [inout]
    ecs_query_t *q_b = ecs_query_new(world, "[out] Position");
    ecs_query_t *q_c = ecs_query_new(world, "[out] Velocity");

    ecs_entity_t e1 = ecs_new_w_id(world, Tag);
    ecs_set(world, e1, Position, {0, 0});
    ecs_set(world, e1, Velocity, {0, 0});

    ecs_entity_t e2 = ecs_new_w_id(world, Tag);
    ecs_set(world, e2, Position, {0, 0});
    ecs_set(world, e2, Velocity, {0, 0});

    // Initial sort
    ecs_iter_t itq = ecs_query_iter(world, q);
    test_bool(dummy_compare_invoked, true); 
    while (ecs_query_next(&itq)) { }
    dummy_compare_invoked = false;

    // No changes, shouldn't sort
    ecs_query_iter(world, q);
    test_bool(dummy_compare_invoked, false);

    // No change in sorted component, shouldn't sort
    { ecs_iter_t it = ecs_query_iter(world, q_c); while (ecs_query_next(&it)); }
    ecs_query_iter(world, q);
    test_bool(dummy_compare_invoked, false);

    // Change in sorted component (inout), should sort
    { ecs_iter_t it = ecs_query_iter(world, q_a); while (ecs_query_next(&it)); }
    itq = ecs_query_iter(world, q);
    test_bool(dummy_compare_invoked, true);
    while (ecs_query_next(&itq)) { }
    dummy_compare_invoked = false;

    // Change in sorted component (out), should sort
    { ecs_iter_t it = ecs_query_iter(world, q_b); while (ecs_query_next(&it)); }
    ecs_query_iter(world, q);
    test_bool(dummy_compare_invoked, true);
    dummy_compare_invoked = false;

    ecs_fini(world);
}
