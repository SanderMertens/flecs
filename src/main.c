/* This is a managed file. Do not delete this comment. */

#include <reflecs/reflecs.h>
#include <reflecs/vector.h>
#include <stdio.h>
#include <reflecs/map.h>

#include <unistd.h>

typedef struct Location {
    int x, y;
} Location;

typedef struct Speed {
    int x, y;
} Speed;

void Move(
    EcsEntity *system,
    EcsEntity *e,
    void *data[])
{
    Location *location = data[0];
    Speed *speed = data[1];
    location->x += speed->x;
    location->y += speed->y;
    printf("%s: location = %d, %d\n", ecs_idof(e), location->x, location->y);
}

int main(int argc, char *argv[]) {
    EcsWorld *world = ecs_world_new();

    /* Create Location and Speed components */
    ECS_COMPONENT(world, Location);
    ECS_COMPONENT(world, Speed);

    /* Create Move system that depends on Location and Speed */
    ECS_SYSTEM(world, Move, Location, Speed);

    /* Create new entity, add and initialize 'Location' component */
    EcsEntity *my_car = ecs_new(world, "my_car");
    Location *location_data = ecs_add(my_car, Location);
    location_data->x = 10;
    location_data->y = 20;

    /* Add and initialize 'Speed' to entity */
    Speed *speed_data = ecs_add(my_car, Speed);
    speed_data->x = 5;
    speed_data->y = 5;

    /* Process all systems in the world */
    ecs_world_progress(world);

    return 0;
}
