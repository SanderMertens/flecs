#include <prefab.h>
#include <stdio.h>

typedef struct { double value; } Attack;
typedef struct { double value; } Defense;
typedef struct { double value; } FreightCapacity;
typedef struct { double value; } ImpulseSpeed;

typedef struct {
    double x; 
    double y; 
} Position;

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Position);
    ECS_COMPONENT(ecs, Attack);
    ECS_COMPONENT(ecs, Defense);
    ECS_COMPONENT(ecs, FreightCapacity);
    ECS_COMPONENT(ecs, ImpulseSpeed);
    ECS_TAG(ecs, HasFTL);

    // Create a prefab hierarchy. Prefabs are entities that by default are 
    // ignored by queries.

    ecs_entity_t spaceship = ecs_new_prefab(ecs, "SpaceShip");
    ecs_set(ecs, spaceship, ImpulseSpeed, {50});
    ecs_set(ecs, spaceship, Defense, {50});
    
    // By default components in an inheritance hierarchy are shared between
    // entities. The override function ensures that instances have a private
    // copy of the component.
    ecs_set_override(ecs, spaceship, Position, {0, 0});

        ecs_entity_t freighter = ecs_new_prefab(ecs, "Freighter");
        // This ensures the entity inherits all components from spaceship.
        ecs_add_pair(ecs, freighter, EcsIsA, spaceship);
        ecs_set(ecs, freighter, FreightCapacity, {100});
        ecs_set(ecs, freighter, Defense, {50});

            ecs_entity_t mammoth_freighter = ecs_new_prefab(ecs, "MammothFreighter");
            ecs_add_pair(ecs, mammoth_freighter, EcsIsA, freighter);
            ecs_set(ecs, mammoth_freighter, FreightCapacity, {500});
            ecs_set(ecs, mammoth_freighter, Defense, {300});

        ecs_entity_t frigate = ecs_new_prefab(ecs, "Frigate");
        ecs_add_pair(ecs, frigate, EcsIsA, spaceship);
        ecs_set(ecs, frigate, Attack, {100});
        ecs_set(ecs, frigate, Defense, {75});
        ecs_set(ecs, frigate, ImpulseSpeed, {125});


    // Create an entity from a prefab.
    // The instance will have a private copy of the Position component, because
    // of the override in the spaceship entity. All other components are shared.
    ecs_entity_t inst = ecs_new_entity(ecs, "my_mammoth_freighter");
    ecs_add_pair(ecs, inst, EcsIsA, mammoth_freighter);

    // Inspect the type of the entity. This outputs:
    //    Position,(Identifier,Name),(IsA,MammothFreighter)
    char *type_str = ecs_type_str(ecs, ecs_get_type(ecs, inst));
    printf("Instance type: [%s]\n", type_str);
    ecs_os_free(type_str);

    // Even though the instance doesn't have a private copy of ImpulseSpeed, we
    // can still get it using the regular API (outputs 50)
    const ImpulseSpeed *ptr = ecs_get(ecs, inst, ImpulseSpeed);
    printf("Impulse speed: %f\n", ptr->value);

    // Prefab components can be iterated like regular components:
    ecs_query_t *q = ecs_query_init(ecs, &(ecs_query_desc_t){
        .filter.terms = {
            // To select components from a prefab, use SuperSet
            { .id = ecs_id(ImpulseSpeed), .subj.set.mask = EcsSuperSet }, 
            { .id = ecs_id(Position) }
        }
    });

    ecs_iter_t it = ecs_query_iter(ecs, q);
    while (ecs_query_next(&it)) {
        ImpulseSpeed *is = ecs_term(&it, ImpulseSpeed, 1);
        Position *p = ecs_term(&it, Position, 2);

        for (int i = 0; i < it.count; i ++) {
            // Because prefab is shared between all entities, access it as a
            // pointer instead of as array.
            p[i].x += is->value;

            printf("%s: {%f, %f}\n", ecs_get_name(ecs, it.entities[i]), 
                p[i].x, p[i].y);
        }
    }

    return ecs_fini(ecs);
}
