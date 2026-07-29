#include <frame.h>

int main(int, char *[]) {
    flecs::world ecs;

    ecs_ftime_t dt = ecs.frame_begin(0.5f);
    assert(dt > 0.4f);
    assert(dt < 0.6f);
    ecs.frame_end();

    return 0;
}
