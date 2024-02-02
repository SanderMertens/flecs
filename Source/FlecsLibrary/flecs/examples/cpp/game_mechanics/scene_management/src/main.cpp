#include <scene_management.h>
#include <iostream>
#include <string>

// This example shows one possible way to implement scene management
// using pipelines.

// Scene relationships/tags
struct ActiveScene { }; // Represents the current scene
struct SceneRoot { }; // Parent for all entities unique to the scene

// Scenes
using Pipeline = flecs::entity;
struct MenuScene { Pipeline pip; };
struct GameScene { Pipeline pip; };

// Components for Example
struct Position { float x, y; };
struct Button { std::string text; };
struct Character { bool alive; };
struct Health { int amount; };

// Removes all entities who are children of
// the current scene root.
// (NOTE: should use defer_begin() / defer_end())
void reset_scene(flecs::world& ecs) {
    ecs.delete_with(flecs::ChildOf, ecs.entity<SceneRoot>());
}

void menu_scene(flecs::iter& it, size_t, ActiveScene) {
    std::cout << "\n>> ActiveScene has changed to `MenuScene`\n\n";

    flecs::world ecs = it.world();
    flecs::entity scene = ecs.component<SceneRoot>();
    
    reset_scene(ecs);

    // Creates a start menu button
    // when we enter the menu scene.
    ecs.entity("Start Button")
        .set(Button{ "Play the Game!" })
        .set(Position{ 50, 50 })
        .child_of(scene);

    ecs.set_pipeline(ecs.get<MenuScene>()->pip);
}

void game_scene(flecs::iter& it, size_t, ActiveScene) {
    std::cout << "\n>> ActiveScene has changed to `GameScene`\n\n";

    flecs::world ecs = it.world();
    flecs::entity scene = ecs.component<SceneRoot>();
    
    reset_scene(ecs);

    // Creates a player character
    // when we enter the game scene.
    ecs.entity("Player")
        .set(Character{ })
        .set(Health{ 2 })
        .set(Position{ 0, 0 })
        .child_of(scene);

    ecs.set_pipeline(ecs.get<GameScene>()->pip);
}

void init_scenes(flecs::world& ecs) {
    // Can only have one active scene
    // in a game at a time.
    ecs.component<ActiveScene>()
        .add(flecs::Exclusive);

    // Each scene gets a pipeline that
    // runs the associated systems plus
    // all other scene-agnostic systems.
    flecs::entity menu = ecs.pipeline()
            .with(flecs::System)
            .without<GameScene>() // Use "without()" of the other scenes
                                  // so that we can run every system that
                                  // doesn't have a scene attached to it.
            .build();

    flecs::entity game = ecs.pipeline()
            .with(flecs::System)
            .without<MenuScene>()
            .build();

    // Set pipeline entities on the scenes
    // to easily find them later with get().
    ecs.set<MenuScene>({ menu });
    ecs.set<GameScene>({ game });

    // Observer to call scene change logic for
    // MenuScene when added to the ActiveScene.
    ecs.observer<ActiveScene>("Scene Change to Menu")
        .event(flecs::OnAdd)
        .second<MenuScene>()
        .each(menu_scene);

    // Observer to call scene change logic for
    // GameScene when added to the ActiveScene.
    ecs.observer<ActiveScene>("Scene Change to Game")
        .event(flecs::OnAdd)
        .second<GameScene>()
        .each(game_scene);
}

void init_systems(flecs::world& ecs) {
    // Will run every time regardless of the
    // current scene we're in.
    ecs.system<const Position>("Print Position")
        .each([](flecs::entity e, const Position& p) {
            // Prints out the position of the
            // entity.
            std::cout << e.name() << ": {" << p.x << ", " << p.y << "}\n";
        });

    // Will only run when the game scene is
    // currently active.
    ecs.system<Health>("Characters Lose Health")
        .kind<GameScene>()
        .each([](Health& h) {
            // Prints out the character's health
            // and then decrements it by one.
            std::cout << h.amount << " health remaining\n";
            h.amount--;
        });

    // Will only run when the menu scene is
    // currently active.
    ecs.system<const Button>("Print Menu Button Text")
        .kind<MenuScene>()
        .each([](const Button& b) {
            // Prints out the text of the menu
            // button.
            std::cout << "Button says \"" << b.text << "\"\n";
        });
}

int main(int, char *[]) {
    flecs::world ecs;

    init_scenes(ecs);
    init_systems(ecs);

    ecs.add<ActiveScene, MenuScene>();
    ecs.progress();

    ecs.add<ActiveScene, GameScene>();
    ecs.progress();
    ecs.progress();
    ecs.progress();

    ecs.add<ActiveScene, MenuScene>();
    ecs.progress();

    ecs.add<ActiveScene, GameScene>();
    ecs.progress();
    ecs.progress();
    ecs.progress();

    // Output
    // >> ActiveScene has changed to `MenuScene`

    // Start Button: {50, 50}
    // Button says "Play the Game!"

    // >> ActiveScene has changed to `GameScene`

    // Player: {0, 0}
    // 2 health remaining
    // Player: {0, 0}
    // 1 health remaining
    // Player: {0, 0}
    // 0 health remaining

    // >> ActiveScene has changed to `MenuScene`

    // Start Button: {50, 50}
    // Button says "Play the Game!"

    // >> ActiveScene has changed to `GameScene`

    // Player: {0, 0}
    // 2 health remaining
    // Player: {0, 0}
    // 1 health remaining
    // Player: {0, 0}
    // 0 health remaining
}
