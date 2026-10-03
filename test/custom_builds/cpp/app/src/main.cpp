#include <app.h>

int main(int, char *[]) {
    flecs::world ecs;

    ecs.system().run([](flecs::iter& it) {
        it.stage().quit();
    });
    
    return ecs.app().run();
}
