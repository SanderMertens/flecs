#include <save_to_file.h>
#include <iostream>
#include <fstream>

#define BUFFER_SIZE (36)
#define FILENAME "savegame.flecs"

/* Component types */
struct Position {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

void save_to_file(flecs::world& world, const char *filename) {
    flecs::reader reader(world);
    std::fstream file(filename, std::fstream::out | std::fstream::binary);

    char buffer[BUFFER_SIZE];
    std::size_t read;

    // Read from world until there is no more data, write buffer(s) to file
    while ((read = reader.read(buffer, BUFFER_SIZE))) {
        file.write(buffer, read);
    }

    file.close();
}

void load_from_file(flecs::world& world, const char *filename) {
    flecs::writer writer(world);
    std::fstream file(filename, std::fstream::in | std::fstream::binary);

    char buffer[BUFFER_SIZE];
    std::size_t read;

    // Read from file until there is no more data, write buffer(s) to world
    while ((read = file.read(buffer, BUFFER_SIZE).gcount())) {
        writer.write(buffer, read);
    }

    file.close();
}

int main(int argc, char *argv[]) {
    // Create world with some entities, save it to disk
    {
        flecs::world world(argc, argv);

        flecs::component<Position>(world, "Position");
        flecs::component<Velocity>(world, "Velocity");

        flecs::entity(world, "E1")
            .set<Position>({1, 2})
            .set<Velocity>({1, 1});

        flecs::entity(world, "E2")
            .set<Position>({3, 4})
            .set<Velocity>({1, 1});

        // Save entities to file
        save_to_file(world, FILENAME);
    }

    // Create new world, restore entities from disk
    {
        flecs::world world(argc, argv);

        // Load entities from file. Note that components don't have to be 
        // redefined, they are restored by the writer.
        load_from_file(world, FILENAME);

        // Define a system to do something with the restored entities
        flecs::system<Position, Velocity>(world)
            .action([](const flecs::iter& it, 
                flecs::column<Position> p, 
                flecs::column<Velocity> v) 
            {    
                for (auto row : it) {
                    p[row].x += v[row].x;
                    p[row].y += v[row].y;

                    std::cout << "Moved " << it.entity(row).name() << " to {"
                            << p[row].x << ", " << p[row].y << "}" << std::endl;
                }
            });

        world.progress();
    }
}
