#include <get_children.h>
#include <iostream>

void print_tree(flecs::entity entity) 
{
    static size_t indent = 0;

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
    flecs::world ecs(argc, argv);

    // Create a simple hierarchy with 2 levels
    auto Parent = ecs.entity("Parent");
        auto Child1 = ecs.entity("Child1").add(flecs::ChildOf, Parent);
            ecs.entity("GrandChild").add(flecs::ChildOf, Child1);
        ecs.entity("Child2").add(flecs::ChildOf, Parent);
        ecs.entity("Child3").add(flecs::ChildOf, Parent);
    
    print_tree(Parent);    
}
