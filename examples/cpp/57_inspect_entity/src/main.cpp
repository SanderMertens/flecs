#include <inspect_entity.h>
#include <iostream>

using std::cout;
using std::endl;
using std::string;
using flecs::entity;
using flecs::IsA;

/* Components */
struct Position {
    double x, y;
};

struct Velocity {  
    double x, y;
};

struct Attack {
    double value;
};

struct Defense {
    double value;
};

struct MaxVelocity {
    double value;
};

// Tags
struct CanFly { };
struct CanFight { };
struct CanFire { };

// Ship states
struct ShipState {
    struct Idle { };
    struct Flying { };
    struct Fighting { };
};

string indent(std::uint32_t lvl) {
    return string(lvl * 2u, ' ');
}

// Visit all components
void visit_components(const entity& e, std::uint32_t lvl = 0) {
    // Iterate all components of entity
    e.each([&](flecs::id& id) {
        // Skip IsA relations
        if (id.has_relation(IsA)) {
            return;
        }

        cout << indent(lvl) << " - ";

        // Print role, if id has one
        if (id.has_role()) {
            cout << id.role_str() << " | ";
        }

        // Print relation, if id has one
        if (id.is_pair()) {
            cout << "(" << id.relation().name() << id.object().name() << ")";
        } else {
            cout << id.object().name();
        }

        cout << endl;
    });
}

// Recursively visit IsA relationships
void visit_is_a(const entity& e, std::uint32_t lvl = 0) {
    // Iterate all IsA relationships
    e.each(IsA, [&](const entity& object) {
        cout << indent(lvl) << "(IsA, " << object.name() << ")" << endl;
        visit_is_a(object, lvl + 1);
        visit_components(object, lvl + 1);
    });
}

int main(int argc, char *argv[]) {
    flecs::world ecs(argc, argv);

    // SpaceShip state machine
    auto ShipStateType = ecs.type().component<ShipState>()
        .add<ShipState::Idle>()
        .add<ShipState::Flying>()
        .add<ShipState::Fighting>();

    // Base SpaceShip
    auto SpaceShip = ecs.prefab("SpaceShip")
        .set<MaxVelocity>({100})
        .set_owned<Position>({0, 0})
        .set_owned<Velocity>({0, 0})
        .add<CanFly>()
        .add_switch(ShipStateType)
            .add_case<ShipState::Idle>();

    // Frigate
    auto Frigate = ecs.prefab("Frigate")
        .add(IsA, SpaceShip)
        .set<Attack>({100})
        .set<Defense>({75})
        .set<MaxVelocity>({150})
        .add<CanFight>();

    // Heavy Frigate
    auto HeavyFrigate = ecs.prefab("HeavyFrigate")
        .add(IsA, Frigate)
        .set<Attack>({150});

    // Frigate instance
    auto Rocinante = ecs.entity("Roci")
        .add(IsA, HeavyFrigate)
        .set<Position>({10, 20})
        .set<Velocity>({1, 1});


    // Start printing contents of Rocinante
    cout << endl;
    cout << Rocinante.name() << ": " << endl;

    // First, visit the IsA relationships to print the inheritance tree
    visit_is_a(Rocinante);

    // Print components of Rocinante itself
    cout << endl;
    cout << "Own components: " << endl;
    visit_components(Rocinante);
    cout << endl;
}
