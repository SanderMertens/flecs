#include <symmetric_relations.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    ecs_world_t *ecs = ecs_init_w_args(argc, argv);

    ECS_TAG(ecs, TradesWith);

    // Register TradesWith as symmetric relationship. Symmetric relationships
    // go both ways, adding (R, B) to A will also add (R, A) to B.
    ecs_add_id(ecs, TradesWith, EcsSymmetric);

    // Create two players
    ecs_entity_t player_1 = ecs_new_id(ecs);
    ecs_entity_t player_2 = ecs_new_id(ecs);

    // Add (TradesWith, player_2) to player_1. This also adds 
    // (TradesWith, player_1) to player_2.
    ecs_add_pair(ecs, player_1, TradesWith, player_2);

    // Log platoon of unit
    printf("Player 1 trades with Player 2: %s\n",
        (ecs_has_pair(ecs, player_1, TradesWith, player_2) ? "true" : "false"));
    printf("Player 2 trades with Player 1: %s\n",
        (ecs_has_pair(ecs, player_2, TradesWith, player_1) ? "true" : "false"));

    // Output:
    //  Player 1 trades with Player 2: true
    //  Player 2 trades with Player 1: true

    return ecs_fini(ecs);
}
