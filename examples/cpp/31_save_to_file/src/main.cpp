#include <save_to_file.h>
#include <iostream>
#include <fstream>

#define BUFFER_SIZE (36)
#define FILENAME "savegame.flecs"

/* Component types */
struct Position {
    double x, y;
};

struct Velocity {
    double x, y;
};

void save_to_file(flecs::world& ecs, const char *filename) {
    flecs::reader reader(ecs);
    std::fstream file(filename, std::fstream::out | std::fstream::binary);

    char buffer[BUFFER_SIZE];
    int64_t read;

    // Read from world until there is no more data, write buffer(s) to file
    while ((read = reader.read(buffer, BUFFER_SIZE))) {
        file.write(buffer, read);
    }

    file.close();
}

void load_from_file(flecs::world& ecs, const char *filename) {
    flecs::writer writer(ecs);
    std::fstream file(filename, std::fstream::in | std::fstream::binary);

    char buffer[BUFFER_SIZE];
    int64_t read;

    // Read from file until there is no more data, write buffer(s) to world
    while ((read = file.read(buffer, BUFFER_SIZE).gcount())) {
        writer.write(buffer, read);
    }

    file.close();
}

int main(int argc, char *argv[]) {
    // Create world with some entities, save it to disk
    {
        flecs::world ecs(argc, argv);

        ecs.entity("E1")
            .set<Position>({1, 2})
            .set<Velocity>({1, 1});

        ecs.entity("E2")
            .set<Position>({3, 4})
            .set<Velocity>({1, 1});

        // Save entities to file
        save_to_file(ecs, FILENAME);
    }

    // Create new world, restore entities from disk
    {
        flecs::world ecs(argc, argv);

        // Load entities from file. Note that components don't have to be 
        // redefined, they are restored by the writer.
        load_from_file(ecs, FILENAME);

        // Define a system to do something with the restored entities
        ecs.system<Position, const Velocity>()
            .each([](flecs::entity e, Position& p, const Velocity& v) {    
                p.x += v.x;
                p.y += v.y;
                std::cout << "Moved " << e.name() << " to {"
                        << p.x << ", " << p.y << "}" << std::endl;
            });

        ecs.progress();
    }
}
