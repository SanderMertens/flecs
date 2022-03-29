#include <setting_variables.h>
#include <iostream>

// This example extends the component_inheritance example, and shows how
// we can use a single rule to match units from different players and platoons
// by setting query variables before we iterate.
//
// The units in this example belong to a platoon, with the platoons belonging
// to a player.

// Unit datamodel
struct Unit { };
struct CombatUnit : Unit { };
struct MeleeUnit : CombatUnit { };
struct RangedUnit : CombatUnit { };
struct Warrior : MeleeUnit { };
struct Wizard : RangedUnit { };
struct Marksman : RangedUnit { };

// Player/Platoon tags
struct Player { };
struct Platoon { };

static const int PlayerCount = 100;
static const int PlatoonsPerPlayer = 3;

int main(int, char *[]) {
    flecs::world ecs;

    // See component_inheritance example
    ecs.component<CombatUnit>().is_a<Unit>();
    ecs.component<MeleeUnit>().is_a<CombatUnit>();
    ecs.component<RangedUnit>().is_a<CombatUnit>();

    ecs.component<Warrior>().is_a<MeleeUnit>();
    ecs.component<Wizard>().is_a<RangedUnit>();
    ecs.component<Marksman>().is_a<RangedUnit>();

    // Populate store with players and platoons
    for (int p = 0; p < PlayerCount; p ++) {
        flecs::entity player;

        // Give first player a name so we can look it up later
        if (p == 0) {
            player = ecs.entity("MyPlayer");
        } else {
            player = ecs.entity();
        }

        // Add player tag so we can query for all players if we want to
        player.add<Player>();

        for (int pl = 0; pl < PlatoonsPerPlayer; pl ++) {
            auto platoon = ecs.entity().add<Player>(player);

            // Add platoon tag so we can query for all platoons if we want to
            platoon.add<Platoon>();

            // Add warriors, wizards and marksmen to the platoon
            ecs.entity().add<Warrior>().add<Platoon>(platoon);
            ecs.entity().add<Marksman>().add<Platoon>(platoon);
            ecs.entity().add<Wizard>().add<Platoon>(platoon);
        }
    }

    // Create a rule to find all RangedUnits for a platoon/player. The 
    // equivalent query in the query DSL would look like this:
    //   (Platoon, $Platoon), Player($Platoon, $Player)
    //
    // The way to read how this query is evaluated is:
    // - find all entities with (Platoon, *), store * in _Platoon
    // - check if _Platoon has (Player, *), store * in _Player
    auto r = ecs.rule_builder<RangedUnit>()
        .term<Platoon>().obj().var("Platoon")
        .term<Player>().subj().var("Platoon").obj().var("Player")
        .build();

    // If we would iterate this rule it would return all ranged units for all
    // platoons & for all players. We can limit the results to just a single
    // platoon or a single player setting a variable beforehand. In this example
    // we'll just find all platoons & ranged units for a single player.

    int player_var = r.find_var("Player");
    int platoon_var = r.find_var("Platoon");

    // Iterate rule, limit the results to units of MyPlayer
    r.iter().set_var(player_var, ecs.lookup("MyPlayer")) 
        .each([&](flecs::iter& it, size_t index, RangedUnit) {
            flecs::entity unit = it.entity(index);
            std::cout << "Unit " << unit.path() << " of class " 
                      << it.id(1).str() << " in platoon "
                      << it.get_var(platoon_var).path() << " for player "
                      << it.get_var(player_var).path() << "\n";
        });

    r.destruct();

    // Output:
    //  Unit ::499 of class Wizard in platoon ::496 for player ::MyPlayer
    //  Unit ::503 of class Wizard in platoon ::500 for player ::MyPlayer
    //  Unit ::507 of class Wizard in platoon ::504 for player ::MyPlayer
    //  Unit ::498 of class Marksman in platoon ::496 for player ::MyPlayer
    //  Unit ::502 of class Marksman in platoon ::500 for player ::MyPlayer
    //  Unit ::506 of class Marksman in platoon ::504 for player ::MyPlayer

    // Try removing the set_var call, this will cause the iterator to return
    // all units in all platoons for all players.
}
