#include <cpp_api.h>

static
void install_test_abort() {
    ecs_os_set_api_defaults();
    ecs_os_api_t os_api = ecs_os_api;
    os_api.abort_ = test_abort;
    ecs_os_set_api(&os_api);
    ecs_tracing_enable(-2);
}

void World_multi_world_empty() {
    flecs::world *w1 = new flecs::world();
    delete w1;
    flecs::world *w2 = new flecs::world();
    delete w2;

    test_assert(true);
}

void World_multi_world_component() {
    flecs::world w1;
    flecs::world w2;

    auto p_1 = w1.component<Position>();
    auto v_1 = w1.component<Velocity>();
    auto v_2 = w2.component<Velocity>();
    auto m_2 = w2.component<Mass>();

    test_assert(v_1.id() == v_2.id());
    test_assert(p_1.id() != m_2.id());
    test_assert(m_2.id() > v_2.id());

    auto p_2 = w2.component<Position>();
    test_assert(p_1.id() == p_2.id());
}

namespace A {
    struct Comp {
        float x;
        float y;
    };
}

void World_multi_world_component_namespace() {
    flecs::world *w = new flecs::world();
    auto c = w->component<A::Comp>();
    auto id_1 = c.id();
    delete w;

    w = new flecs::world();
    c = w->component<A::Comp>();
    auto id_2 = c.id();

    test_assert(id_1 == id_2);
}

void World_type_id() {
    flecs::world w;

    auto p = w.component<Position>();

    test_assert(p.id() == flecs::type_id<Position>());
}

void World_different_comp_same_name() {
    flecs::world w;

    install_test_abort();
    test_expect_abort();

    w.component<Position>("Position");
    w.component<Velocity>("Position");
}
