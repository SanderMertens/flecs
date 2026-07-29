#include <script.h>

int main(int, char *[]) {
    flecs::world ecs;

    assert(ecs_script_run(ecs, "main", "e {}", NULL) == 0);
    assert(ecs.lookup("e") != 0);

    return 0;
}
