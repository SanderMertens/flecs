#include <query_console.h>
#include <iostream>

// This example shows how to use the query DSL parser to evaluate queries that
// are entered at runtime. It also shows how to define entities in Plecs, which
// is a language optimized for describing ECS entities and components.

int main(int, char *[]) {
    flecs::world ecs;

    // Load entity definitions from the plecs file
    ecs.parse_plecs_file("db.plecs");


}
