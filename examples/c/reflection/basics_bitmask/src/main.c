#include <basics_bitmask.h>
#include <stdio.h>

typedef uint32_t Toppings;
static const uint32_t Bacon = 0x1;
static const uint32_t Lettuce = 0x2;
static const uint32_t Tomato = 0x4;

typedef struct {
    Toppings toppings;
} Sandwich;

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // Register component as usual
    ECS_COMPONENT(ecs, Toppings);
    ECS_COMPONENT(ecs, Sandwich);

    // Add reflection data to components
    ecs_bitmask_init(ecs, &(ecs_bitmask_desc_t){
        .entity.entity = ecs_id(Toppings), // Make sure to use existing id
        .constants = {
            { .name = "Bacon", .value = Bacon },
            { .name = "Lettuce", .value = Lettuce },
            { .name = "Tomato", .value = Tomato }
        }
    });

    ecs_struct_init(ecs, &(ecs_struct_desc_t){
        .entity.entity = ecs_id(Sandwich), // Make sure to use existing id
        .members = {
            { .name = "toppings", .type = ecs_id(Toppings) }
        }
    });

    // Create entity with Position as usual
    ecs_entity_t ent = ecs_new_id(ecs);
    ecs_set(ecs, ent, Sandwich, {Bacon | Lettuce});

    // Convert position component to flecs expression string
    const Sandwich *ptr = ecs_get(ecs, ent, Sandwich);
    char *str = ecs_ptr_to_expr(ecs, ecs_id(Sandwich), ptr);
    printf("%s\n", str); // {toppings: Bacon|Lettuce}
    ecs_os_free(str);

    ecs_fini(ecs);
}
