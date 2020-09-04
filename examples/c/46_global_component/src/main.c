#include <global_component.h>

typedef struct Position {
    float x;
    float y;
} Position;

/* Declare component handle globally so other functions can access it. The
 * header file contains an extern declaration for, so other source files
 * can use the component id as well. */
ECS_COMPONENT_DECLARE(Position);

ecs_entity_t create_entity(ecs_world_t *world) {
    /* Use component handle without passing it into the function */
    return ecs_new(world, Position);
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init();

    /* Assign component id to previously declared variables */
    ECS_COMPONENT_DEFINE(world, Position);

    ecs_entity_t e = create_entity(world);
    
    printf("Has Position: %s\n", 
        ecs_has(world, e, Position) ? "true" : "false");

    return ecs_fini(world);
}
