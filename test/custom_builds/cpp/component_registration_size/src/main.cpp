#include <component_registration_size.h>

int main(int, char *[]) {
    flecs::world world;
    component_registration_size_register_300_components(world);
    return 0;
}
