#include <pipeline_no_timer.h>
#include <iostream>

int main(int, char *[]) {
    flecs::world ecs;
    ecs.progress();
}
