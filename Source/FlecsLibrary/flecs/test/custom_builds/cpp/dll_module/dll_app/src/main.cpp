#include <dll_app.h>

int main(int argc, char *argv[]) {
    flecs::world world(argc, argv);
    
    world.import<movement::module>();
}
