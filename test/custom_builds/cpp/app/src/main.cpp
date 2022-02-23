#include <app.h>

int main(int, char *[]) {
    flecs::world ecs;

    ecs.system().iter([](flecs::iter& it) {
        it.world().quit();
    });
    
    return ecs.app().run();
}
