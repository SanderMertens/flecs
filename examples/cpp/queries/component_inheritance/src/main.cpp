#include <component_inheritance.h>
#include <iostream>

// This example shows how queries can be used to match simple inheritance trees.

struct Unit { };
struct CombatUnit : Unit { };
struct MeleeUnit : CombatUnit { };
struct RangedUnit : CombatUnit { };

struct Warrior : MeleeUnit { };
struct Wizard : RangedUnit { };
struct Marksman : RangedUnit { };
struct Builder : Unit { };

int main(int, char *[]) {
    flecs::world ecs;

    // Make the ECS aware of the inheritance relationships. Note that IsA
    // relationship used here is the same as in the prefab example.
    ecs.component<CombatUnit>().is_a<Unit>();
    ecs.component<MeleeUnit>().is_a<CombatUnit>();
    ecs.component<RangedUnit>().is_a<CombatUnit>();

    ecs.component<Warrior>().is_a<MeleeUnit>();
    ecs.component<Wizard>().is_a<RangedUnit>();
    ecs.component<Marksman>().is_a<RangedUnit>();
    ecs.component<Builder>().is_a<Unit>();

    // Create a few units
    ecs.entity("warrior_1").add<Warrior>();
    ecs.entity("warrior_2").add<Warrior>();

    ecs.entity("marksman_1").add<Marksman>();
    ecs.entity("marksman_2").add<Marksman>();

    ecs.entity("wizard_1").add<Wizard>();
    ecs.entity("wizard_2").add<Wizard>();

    ecs.entity("builder_1").add<Builder>();
    ecs.entity("builder_2").add<Builder>();

    // Create a query to find all ranged units
    flecs::query<RangedUnit> q = ecs.query<RangedUnit>();

    // Iterate the query
    q.each([](flecs::entity e, RangedUnit) {
        std::cout << "Unit " << e.name() << " found\n";
    });

    // Output:
    //   Unit wizard_1 found
    //   Unit wizard_2 found
    //   Unit marksman_1 found
    //   Unit marksman_2 found
}
