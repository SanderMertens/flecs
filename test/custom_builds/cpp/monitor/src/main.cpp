#include <monitor.h>

int main(int, char *[]) {
    flecs::world ecs;

    ecs.import<flecs::stats>();

    return 0;
}
