#include <iostream>
#include "../../distr/flecs.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <flecs_script_file>" << std::endl;
        return 1;
    }

    flecs::world world;

    // Run the Flecs script file
    if (world.script_run_file(argv[1]) != 0) {
        std::cerr << "Error running script" << std::endl;
        return 1;
    }

    std::cout << "Script '" << argv[1] << "' executed successfully." << std::endl;

    return 0;
}
