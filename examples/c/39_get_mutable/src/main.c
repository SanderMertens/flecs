#include <get_mutable.h>

typedef struct Counter {
    int value;
} Counter;

void PrintCounterAdd(ecs_iter_t *it) {
    for (int i = 0; i < it->count; i ++) {
        printf("%s: Counter added\n", ecs_get_name(it->world, it->entities[i]));
    }
}

void PrintCounterSet(ecs_iter_t *it) {
    Counter *counter = ecs_term(it, Counter, 1);

    for (int i = 0; i < it->count; i ++) {
        printf("%s: Counter set to %d\n", 
            ecs_get_name(it->world, it->entities[i]),
            counter[i].value);
    }
}

void inc(ecs_world_t *world, ecs_entity_t e, ecs_entity_t ecs_id(Counter)) {
    bool is_added = false;
    
    Counter *c = ecs_get_mut(world, e, Counter, &is_added);
    if (is_added) {
        c->value = 0;
    }

    c->value ++;

    ecs_modified(world, e, Counter);
}

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Counter);
    ECS_SYSTEM(world, PrintCounterAdd, EcsOnAdd, Counter);
    ECS_SYSTEM(world, PrintCounterSet, EcsOnSet, Counter);

    ECS_ENTITY(world, MyEntity, 0);

    inc(world, MyEntity, ecs_id(Counter));
    inc(world, MyEntity, ecs_id(Counter));

    /* Cleanup */
    return ecs_fini(world);
}
