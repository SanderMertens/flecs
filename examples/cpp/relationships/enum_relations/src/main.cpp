#include <enum_relations.h>
#include <stdio.h>

// When an enumeration constant is added to an entity, it is added as a relationship
// pair where the relationship is the enum type, and the target is the constant. For
// example, this statement:
//   e.add(Color::Red)
//
// adds this relationship:
//   (Color, Color::Red)
//
// Enums are registered as exclusive relationships, which means that adding an
// enum constant will replace the previous constant for that enumeration:
//   e.add(Color::Green)
//
//  will replace Color::Red with Color::Green

// Regular enumerations are supported
enum Tile {
    Grass, 
    Sand, 
    Stone
};

// Enum classes are supported
enum class TileStatus {
    Free,
    Occupied,
};

int main(int, char *[]) {
    flecs::world ecs;

    // Create an entity with (Tile, Stone) and (TileStatus, Free) relationships
    flecs::entity tile = ecs.entity()
        .add(Tile::Stone)
        .add(TileStatus::Free);

    // (Tile, Tile.Stone), (TileStatus, TileStatus.Free)
    printf("%s\n", tile.type().str().c_str());

    // Replace (TileStatus, Free) with (TileStatus, Occupied)
    tile.add(TileStatus::Occupied);

    // (Tile, Tile.Stone), (TileStatus, TileStatus.Occupied)
    printf("%s\n", tile.type().str().c_str());

    // Check if the entity has the Tile relationship and the Tile::Stone pair
    printf("%s\n", tile.has<Tile>() ? "true" : "false"); // true
    printf("%s\n", tile.has(Tile::Stone) ? "true" : "false"); // true

    // Get the current value of the enum
    Tile v = tile.get_constant<Tile>();
    printf("%s\n", (v == Tile::Stone) ? "true" : "false"); // true

    // Create a few more entities that we can query
    ecs.entity().add(Tile::Grass).add(TileStatus::Free);
    ecs.entity().add(Tile::Sand).add(TileStatus::Occupied);

    // Iterate all entities with a Tile relationship
    ecs.query_builder()
        .with<Tile>(flecs::Wildcard)
        .build()
        .each([&](flecs::iter& it, size_t) {
            flecs::entity tile_constant = it.pair(0).second();
            printf("%s\n", tile_constant.path().c_str());
        });

    // Outputs:
    //  ::Tile::Stone
    //  ::Tile::Grass
    //  ::Tile::Sand

    // Iterate only occupied tiles
    ecs.query_builder()
        .with<Tile>(flecs::Wildcard)
        .with(TileStatus::Occupied)
        .build()
        .each([&](flecs::iter& it, size_t) {
            flecs::entity tile_constant = it.pair(0).second();
            printf("%s\n", tile_constant.path().c_str());
        });

    // Outputs:
    //  ::Tile::Stone
    //  ::Tile::Sand

    // Remove any instance of the TileStatus relationship
    tile.remove<TileStatus>(flecs::Wildcard);
}
