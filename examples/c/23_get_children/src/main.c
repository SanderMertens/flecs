#include <get_children.h>

int indent = 0;

void print_tree(
    ecs_world_t *world, 
    ecs_entity_t entity) 
{
    printf("%*s%s\n", indent * 2, "", ecs_get_name(world, entity));

    indent ++;

    // Get an iterator to the children of the current entity
    ecs_iter_t it = ecs_scope_iter(world, entity);

    // Iterate all the tables that contain children for the parent
    while (ecs_scope_next(&it)) {
        for (int i = 0; i < it.count; i ++) {
            // Print the child, and recursively iterate
            print_tree(world, it.entities[i]);
        }
    }

    indent --;
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    // Create a simple hierarchy with 2 levels
    ECS_ENTITY(world, Parent, 0);
        ECS_ENTITY(world, Child1, CHILDOF | Parent);
            ECS_ENTITY(world, GrandChild, CHILDOF | Parent.Child1);
        ECS_ENTITY(world, Child2, CHILDOF | Parent);
        ECS_ENTITY(world, Child3, CHILDOF | Parent);

    print_tree(world, Parent);

    return ecs_fini(world);
}
