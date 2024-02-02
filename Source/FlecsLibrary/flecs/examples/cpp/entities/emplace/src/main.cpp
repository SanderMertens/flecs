#include <emplace.h>
#include <iostream>

// The emplace method constructs a component directly in the storage vs. a 
// regular set, which moves the value into an already constructed value in the
// storage. This reduces the overhead of creating a new component.
//
// Components that do not have a default constructor need to be created with
// emplace, as they can't be constructed automatically by flecs.

struct NoDefaultCtor {
    NoDefaultCtor(double x_, double y_) : x(x_), y(y_) { }

    double x, y;
};

int main(int, char *[]) {
    flecs::world ecs;

    flecs::entity e = ecs.entity()
        .emplace<NoDefaultCtor>(10.0, 20.0);

    const NoDefaultCtor *ptr = e.get<NoDefaultCtor>();
    std::cout << "{" << ptr->x << ", " << ptr->y << "}" << "\n";

    // Output
    //  {10, 20}
}
