#include <on_demand_system.h>
#include <iostream>

struct Position {
    double x, y;
};

struct Velocity {
    double x, y;
};

struct Printable {};

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    flecs::world ecs(argc, argv);

    ecs.component<Position>();
    ecs.component<Velocity>();

    /* The 'Move' is marked as on_demand which means Flecs will only
     * run this system if there is interest in any of its [out] columns. In this
     * case the system will only be ran if there is interest in Position. */
    ecs.system<>().on_demand()
        .term<Position>().inout(flecs::Out)
        .term<Velocity>().inout(flecs::In)
        .iter([](flecs::iter& it){
            auto p = it.term<Position>(1);
            auto v = it.term<const Velocity>(2);

            for (auto row: it) {
                p[row].x += v[row].x;
                p[row].y += v[row].y;

                std::cout << "Moved " << it.entity(row).name() << " to {" <<
                    p[row].x << ", " << p[row].y << "}" << std::endl;                
            }
        });

    /* The 'PrintPosition' is a regular system with an [in] (const) column. This signals
     * that the system will not write Position, and relies on another system to
     * provide a value for it. If there are any OnDemand systems that provide
     * 'Position' as an output, they will be enabled. */
    auto PrintPosition = ecs.system<const Position, Printable>()
        .each([](flecs::entity e, const Position& p, Printable&) {
            std::cout << "Position of " << e.name() 
                << " is {" << p.x << ", " << p.y << "}" 
                << std::endl; 
        });

    /* Create dummy entity. Entity does not match with PrintPosition because it
     * does not have the Printable component */
    auto e = ecs.entity("MyEntity")
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    /* No systems will be executed. The PrintPosition system is enabled, but it
     * has no matching entities. As a result, there is no demand for the 
     * Position component, and the Move system won't be executed either, even
     * though the entity does match with it. */
    std::cout << "First iteration: PrintPosition is inactive" << std::endl;
    ecs.progress();

    /* Add Printable to the entity */
    e.add<Printable>();

    /* Both systems will now be executed. The entity matches with PrintPosition
     * meaning there is demand for Position, and thus the Move system will be
     * enabled. */
    std::cout << std::endl << "Second iteration: PrintPosition is active" << std::endl;
    ecs.progress();

    /* Disable the PrintPosition system. Now there is no longer demand for the
     * Position component, so the Move on-demand system will be disabled. */
    std::cout << std::endl << "Third iteration: PrintPosition is disabled" << std::endl;
    PrintPosition.disable();
    ecs.progress();
}
