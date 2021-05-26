#include <sorting.h>
#include <iostream>

struct Position {
    double x, y;
};

// Order by x member of Position */
int compare_position(
    flecs::entity_t e1,
    const Position *p1,
    flecs::entity_t e2,
    const Position *p2)
{
    (void)e1;
    (void)e2;
    return (p1->x > p2->x) - (p1->x < p2->x);
}

// Iterate query, printed values will be ordered
void print_query(flecs::query<Position>& q) {
    q.each([](flecs::entity, Position& p) {
        std::cout << "{" << p.x << "," << p.y << "}" << std::endl;
    });
}

int main(int argc, char *argv[]) {
    flecs::world ecs(argc, argv);

    // Create entities, set Position in random order
    auto e = ecs.entity().set<Position>({1, 0});
    ecs.entity().set<Position>({6, 0});
    ecs.entity().set<Position>({2, 0});
    ecs.entity().set<Position>({5, 0});
    ecs.entity().set<Position>({4, 0});

    // Create a system with sorting enabled
    auto sys = ecs.system<Position>()
        .order_by(compare_position)
        .each([](Position &p) {
            std::cout << "{" << p.x << "," << p.y << "}" << std::endl;
        });

    // Create a query for component Position
    auto q = ecs.query<Position>();

    // Order by Position component
    q.order_by(compare_position);

    // Iterate query, print values of Position
    std::cout << "-- First iteration" << std::endl;
    print_query(q);

    // Change the value of one entity, invalidating the order
    e.set<Position>({7, 0});

    // Iterate query again, printed values are still ordered
    std::cout << "-- Second iteration" << std::endl;
    print_query(q);

    // Create new entity to show that data is also sorted for system
    ecs.entity().set<Position>({3, 0});
    
    // Run system, output will be sorted
    std::cout << "-- System iteration" << std::endl;
    sys.run();
}    
