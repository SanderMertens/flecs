#include <get_children.h>
#include <iostream>

void print_tree(flecs::entity entity) 
{
    static int indent = 0;

    std::cout << std::string(indent * 2, ' ') << entity.name() << std::endl;

    indent ++;

    for (auto children : entity.children()) {
        for (auto i : children) {
            // Print the child, and recursively iterate
            print_tree(children.entity(i));
        }
    }

    indent --;
}

int main(int argc, char *argv[]) {
    flecs::world world(argc, argv);

    // Create a simple hierarchy with 2 levels
    auto Parent = flecs::entity(world, "Parent");
        auto Child1 = flecs::entity(world, "Child1").add_childof(Parent);
            flecs::entity(world, "GrandChild").add_childof(Child1);
        flecs::entity(world, "Child2").add_childof(Parent);
        flecs::entity(world, "Child3").add_childof(Parent);
    
    print_tree(Parent);    
}
