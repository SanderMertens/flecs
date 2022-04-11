#include <symmetric_relations.h>
#include <iostream>

// Type for TradesWith relationship
struct TradesWith { };

int main(int, char*[]) {
    flecs::world ecs;

    // Register TradesWith as symmetric relationship. Symmetric relationships
    // go both ways, adding (R, B) to A will also add (R, A) to B.
    ecs.component<TradesWith>()
        .add(flecs::Symmetric);

    // Create two players
    auto player_1 = ecs.entity();
    auto player_2 = ecs.entity();

    // Add (TradesWith, player_2) to player_1. This also adds 
    // (TradesWith, player_1) to player_2.
    player_1.add<TradesWith>(player_2);

    // Log platoon of unit
    std::cout << "Player 1 trades with Player 2: " << 
        (player_1.has<TradesWith>(player_2) ? "true" : "false") << "\n";
    std::cout << "Player 2 trades with Player 1: " << 
        (player_2.has<TradesWith>(player_1) ? "true" : "false") << "\n";

    // Output:
    //  Player 1 trades with Player 2: true
    //  Player 2 trades with Player 1: true
}
