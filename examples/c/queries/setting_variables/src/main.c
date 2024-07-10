#include <setting_variables.h>
#include <stdio.h>

// This example extends the component_inheritance example, and shows how
// we can use a single query to match units from different players and platoons
// by setting query variables before we iterate.
//
// The units in this example belong to a platoon, with the platoons belonging
// to a player.

static const int PlayerCount = 100;
static const int PlatoonsPerPlayer = 3;

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    // Unit datamodel - see component_inheritance example
    ECS_TAG(ecs, Unit);
    ECS_ENTITY(ecs, CombatUnit, (IsA, Unit));
    ECS_ENTITY(ecs, MeleeUnit,  (IsA, CombatUnit));
    ECS_ENTITY(ecs, RangedUnit, (IsA, CombatUnit));

    ECS_ENTITY(ecs, Warrior,    (IsA, MeleeUnit));
    ECS_ENTITY(ecs, Wizard,     (IsA, RangedUnit));
    ECS_ENTITY(ecs, Marksman,   (IsA, RangedUnit));
    ECS_ENTITY(ecs, Builder,    (IsA, Unit));

    // Player/Platoon tags
    ECS_TAG(ecs, Player);
    ECS_TAG(ecs, Platoon);

    // Populate store with players and platoons
    for (int p = 0; p < PlayerCount; p ++) {
        ecs_entity_t player;

        // Give first player a name so we can look it up later
        if (p == 0) {
            player = ecs_entity(ecs, { .name = "MyPlayer" });
        } else {
            player = ecs_new(ecs);
        }

        // Add player tag so we can query for all players if we want to
        ecs_add(ecs, player, Player);

        for (int pl = 0; pl < PlatoonsPerPlayer; pl ++) {
            ecs_entity_t platoon = ecs_new_w_pair(ecs, Player, player);

            // Add platoon tag so we can query for all platoons if we want to
            ecs_add(ecs, platoon, Platoon);

            // Add warriors, wizards and marksmen to the platoon
            ecs_entity_t warrior = ecs_new_w(ecs, Warrior);
            ecs_add_pair(ecs, warrior, Platoon, platoon);

            ecs_entity_t marksman = ecs_new_w(ecs, Marksman);
            ecs_add_pair(ecs, marksman, Platoon, platoon);

            ecs_entity_t wizard = ecs_new_w(ecs, Wizard);
            ecs_add_pair(ecs, wizard, Platoon, platoon);
        }
    }

    // Create a query to find all RangedUnits for a platoon/player. The 
    // equivalent query in the query DSL would look like this:
    //   (Platoon, $Platoon), Player($Platoon, $Player)
    //
    // The way to read how this query is evaluated is:
    // - find all entities with (Platoon, *), store * in $platoon
    // - check if $platoon has (Player, *), store * in $player
    ecs_query_t *q = ecs_query(ecs, {
        .terms = {
            { .first.id = RangedUnit },
            { .first.id = Platoon, .second.name = "$platoon" },
            { .first.id = Player,  .src.name     = "$platoon", .second.name = "$player" }
        }
    });

    // If we would iterate this query it would return all ranged units for all
    // platoons & for all players. We can limit the results to just a single
    // platoon or a single player setting a variable beforehand. In this example
    // we'll just find all platoons & ranged units for a single player.

    int player_var = ecs_query_find_var(q, "player");
    int platoon_var = ecs_query_find_var(q, "platoon");

    // Iterate query, limit the results to units of MyPlayer
    ecs_iter_t it = ecs_query_iter(ecs, q);
    ecs_iter_set_var(&it, player_var, ecs_lookup(ecs, "MyPlayer"));

    while (ecs_query_next(&it)) {
        ecs_entity_t player = ecs_iter_get_var(&it, player_var);
        ecs_entity_t platoon = ecs_iter_get_var(&it, platoon_var);
        char *platoon_str = ecs_get_path(ecs, platoon);
        char *class_str = ecs_id_str(ecs, ecs_field_id(&it, 0));

        for (int i = 0; i < it.count; i ++) {
            char *unit_str = ecs_get_path(ecs, it.entities[i]);
            printf("Unit %s of class %s in platoon %s for player %s\n", 
                unit_str, class_str, platoon_str,
                ecs_get_name(ecs, player));
            ecs_os_free(unit_str);
        }

        ecs_os_free(class_str);
        ecs_os_free(platoon_str);
    }

    ecs_query_fini(q);

    // Output
    //  Unit 513 of class Wizard in platoon 510 for player MyPlayer
    //  Unit 517 of class Wizard in platoon 514 for player MyPlayer
    //  Unit 521 of class Wizard in platoon 518 for player MyPlayer
    //  Unit 512 of class Marksman in platoon 510 for player MyPlayer
    //  Unit 516 of class Marksman in platoon 514 for player MyPlayer
    //  Unit 520 of class Marksman in platoon 518 for player MyPlayer

    return ecs_fini(ecs);
}
