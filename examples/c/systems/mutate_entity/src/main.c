#include <mutate_entity.h>
#include <stdio.h>

typedef struct {
    double value;
} Timeout;

// System that deletes an entity after a timeout expires
void Expire(ecs_iter_t *it) {
    Timeout *t = ecs_field(it, Timeout, 1);

    for (int i = 0; i < it->count; i ++) {
        t[i].value -= (double)it->delta_time;
        if (t[i].value <= 0) {
            // When deleting the entity, use the world provided by the iterator.

            // To make sure that the storage doesn't change while a system is
            // iterating entities, and multiple threads can safely access the
            // data, mutations (like a delete) are added to a command queue and
            // executed when it's safe to do so.
 
             // A system should not use the world pointer that is provided by the
             // ecs_init function, as this will throw an error that the world is
             // in readonly mode (try replacing it->world with it->real_world).
            ecs_delete(it->world, it->entities[i]);
            printf("Expire: %s deleted!\n", ecs_get_name(
                it->world, it->entities[i]));
        }
    }
}

// System that prints remaining expiry time
void PrintExpire(ecs_iter_t *it) {
    Timeout *t = ecs_field(it, Timeout, 1);

    for (int i = 0; i < it->count; i ++) {
        printf("PrintExpire: %s has %.2f seconds left\n", ecs_get_name(
            it->world, it->entities[i]), t[i].value);
    }
}

// Observer that triggers when the component is actually removed
void Expired(ecs_iter_t *it) {
    for (int i = 0; i < it->count; i ++) {
        printf("Expired: %s actually deleted\n", ecs_get_name(
            it->world, it->entities[i]));
    }
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(world, Timeout);
    ECS_SYSTEM(world, Expire, EcsOnUpdate, Timeout);
    ECS_SYSTEM(world, PrintExpire, EcsOnUpdate, [in] Timeout);
    ECS_OBSERVER(world, Expired, EcsOnRemove, Timeout);

    // Create entity that expires after 3 seconds
    ecs_entity_t e = ecs_set_name(world, 0, "MyEntity");
    ecs_set(world, e, Timeout, { 3.0 });
    
    ecs_set_target_fps(world, 1);

    while ( ecs_progress(world, 0)) {
        // If entity is no longer alive, exit
        if (!ecs_is_alive(world, e)) {
            break;
        }

        printf("Tick...\n");
    }

    // Output:
    //  PrintExpire: MyEntity has 2.00 seconds left
    //  Tick...
    //  PrintExpire: MyEntity has 0.98 seconds left
    //  Tick...
    //  Expire: MyEntity deleted!
    //  PrintExpire: MyEntity has -0.02 seconds left
    //  Expired: MyEntity actually deleted

    return ecs_fini(world);
}
