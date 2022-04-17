#include <relation_component.h>
#include <stdio.h>

// This example shows how relations can be combined with components to attach
// data to a relationship.

// Some demo components:

typedef struct {
  double amount;
} Requires;

typedef struct {
    double timeout;
} Expires;

typedef struct {
    double x;
    double y;
} Position;

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_COMPONENT(ecs, Requires);
    ECS_COMPONENT(ecs, Expires);
    ECS_COMPONENT(ecs, Position);

    ECS_TAG(ecs, Gigawatts);
    ECS_TAG(ecs, MustHave);

    // When one element of a pair is a component and the other element is a tag, 
    // the pair assumes the type of the component. 
    ecs_entity_t e1 = ecs_new_id(ecs);
    ecs_set_pair(ecs, e1, Requires, Gigawatts, {1.21});
    const Requires *r = ecs_get_pair(ecs, e1, Requires, Gigawatts);
    printf("requires: %f\n", r->amount);

    // The component can be either the first or second part of a pair:
    ecs_entity_t e2 = ecs_new_id(ecs);
    ecs_set_pair_second(ecs, e2, Gigawatts, Requires, {1.21});
    r = ecs_get_pair_second(ecs, e2, Gigawatts, Requires);
    printf("requires: %f\n", r->amount);

    // Note that <Requires, Gigawatts> and <Gigawatts, Requires> are two 
    // different pairs, and can be added to an entity at the same time.

    // If both parts of a pair are components, the pair assumes the type of
    // the first element:
    ecs_entity_t e3 = ecs_new_id(ecs);
    ecs_set_pair(ecs, e3, Expires, ecs_id(Position), {0.5});
    const Expires *e = ecs_get_pair(ecs, e3, Expires, ecs_id(Position));
    printf("expires: %f\n", e->timeout);

    // You can prevent a pair from assuming the type of a component by adding
    // the Tag property to a relationship:
    ecs_add_id(ecs, MustHave, EcsTag);

    // Even though Position is a component, <MustHave, Position> contains no
    // data because MustHave has the Tag property.
    ecs_new_w_pair(ecs, MustHave, ecs_id(Position));

    // The id::type_id method can be used to find the component type for a pair:
    ecs_entity_t type_id;
    char *type_str;
    
    type_id = ecs_get_typeid(ecs, ecs_pair(ecs_id(Requires), Gigawatts));
    type_str = ecs_get_fullpath(ecs, type_id);
    printf("%s\n", type_str);
    ecs_os_free(type_str);

    type_id = ecs_get_typeid(ecs, ecs_pair(Gigawatts, ecs_id(Requires)));
    type_str = ecs_get_fullpath(ecs, type_id);
    printf("%s\n", type_str);
    ecs_os_free(type_str);

    type_id = ecs_get_typeid(ecs, ecs_pair(ecs_id(Expires), ecs_id(Position)));
    type_str = ecs_get_fullpath(ecs, type_id);
    printf("%s\n", type_str);
    ecs_os_free(type_str);

    type_id = ecs_get_typeid(ecs, ecs_pair(MustHave, ecs_id(Position)));
    type_str = ecs_get_fullpath(ecs, type_id);
    printf("%s\n", type_str);
    ecs_os_free(type_str);

    // Output:
    //  requires: 1.210000
    //  requires: 1.210000
    //  expires: 0.500000
    //  Requires
    //  Requires
    //  Expires
    //  0
}
