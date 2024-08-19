#include <run.h>
#include <iostream>

struct Position { 
    double x, y;
};

struct Velocity { 
    double x, y;
};

struct Mass {
    double value;
};

int main(int, char *[]) {
    flecs::world ecs;

    // Create a query for Position, Velocity.
    flecs::query<Position, const Velocity> q = 
        ecs.query<Position, const Velocity>();

    // Create a few test entities for a Position, Velocity query
    ecs.entity("e1")
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    ecs.entity("e2")
        .set<Position>({10, 20})
        .set<Velocity>({3, 4});

    ecs.entity("e3")
        .set<Position>({10, 20})
        .set<Velocity>({4, 5})
        .set<Mass>({50});

    // The run() function provides a flecs::iter object which contains all sorts
    // of information on the entities currently being iterated.
    q.run([&](flecs::iter& it) {
        while (it.next()) {
            auto p = it.field<Position>(0);
            auto v = it.field<const Velocity>(1);

            // Print the table & number of entities matched in current callback
            std::cout << "Table [" << it.type().str() << "]" << std::endl;
            std::cout << " - number of entities: " << it.count() << std::endl;

            // Print information about the components being matched
            for (int8_t i = 0; i < it.field_count(); i ++) {
                std::cout << " - term " << i << ": " << std::endl;
                std::cout << "   - component: " << it.id(i).str() << std::endl;
                std::cout << "   - type size: " << it.size(i) << std::endl;
            }

            std::cout << std::endl;

            // Iterate entities
            for (auto i : it) {
                p[i].x += v[i].x;
                p[i].y += v[i].y;
                std::cout << " - " << it.entity(i).name() << 
                    ": {" << p[i].x << ", " << p[i].y << "}\n";
            }

            std::cout << std::endl;
        }
    });
}
