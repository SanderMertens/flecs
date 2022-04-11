#include <basics.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_TAG(ecs, Eats);

    // Entity used for Grows relation
    ecs_entity_t grows = ecs_new_entity(ecs, "Grows");

    // Relation objects
    ecs_entity_t apples = ecs_new_entity(ecs, "Apples");
    ecs_entity_t pears = ecs_new_entity(ecs, "Pears");

    // Create an entity with 3 relations. Relations are like regular components,
    // but combine two types/identifiers into an (relation, object) pair.
    ecs_entity_t bob = ecs_new_entity(ecs, "Bob");
    // Pairs can be constructed from a type and entity
    ecs_add_pair(ecs, bob, Eats, apples);
    ecs_add_pair(ecs, bob, Eats, pears);
    // Pairs can also be constructed from two entity ids
    ecs_add_pair(ecs, bob, grows, pears);

    // Has can be used with relations as well
    printf("Bob eats apples? %d\n", ecs_has_pair(ecs, bob, Eats, apples));

    // Wildcards can be used to match relations
    printf("Bob grows food? %d\n", ecs_has_pair(ecs, bob, grows, EcsWildcard));

    // Print the type of the entity. Should output:
    //   (Identifier,Name),(Eats,Apples),(Eats,Pears),(Grows,Pears)
    char *type_str = ecs_type_str(ecs, ecs_get_type(ecs, bob));
    printf("Bob's type: [%s]\n", type_str);
    ecs_os_free(type_str);

    return ecs_fini(ecs);
}
