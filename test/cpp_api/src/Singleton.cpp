#include <cpp_api.h>

void Singleton_set_get_singleton() {
    flecs::world world;

    world.set<Position>({10, 20});

    const Position *p = world.get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Singleton_get_mut_singleton() {
    flecs::world world;

    Position *p_mut = world.get_mut<Position>();
    p_mut->x = 10;
    p_mut->y = 20;

    const Position *p = world.get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Singleton_patch_singleton() {
    flecs::world world;

    world.patch<Position>([](Position& p) {
        p.x = 10;
        p.y = 20;
    });

    const Position *p = world.get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Singleton_singleton_system() {
    flecs::world world;

    world.set<Position>({10, 20});

    world.system<>(nullptr, "$Position")
        .iter([](flecs::iter it) {
            auto p = it.column<Position>(1);
            test_int(p->x, 10);
            test_int(p->y, 20);

            p->x ++;
            p->y ++;
        });

    world.progress();

    const Position *p = world.get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 11);
    test_int(p->y, 21);
}
