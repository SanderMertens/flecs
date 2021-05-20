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
    auto parent = ecs.entity("Parent").scope([&]{
        ecs.entity("Child1").scope([&]{
            ecs.entity("GrandChild");
        });

        ecs.entity("Child2");
        ecs.entity("Child3");
    });

    print_tree(parent);    
}
