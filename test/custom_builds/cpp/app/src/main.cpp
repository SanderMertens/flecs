#include <app.h>

int main(int, char *[]) {
    flecs::world ecs;

    ecs.system().run([](flecs::iter& it) {
        it.world().quit();
    });
    
    return ecs.app().run();
}
