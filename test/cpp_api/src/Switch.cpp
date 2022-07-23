#include <cpp_api.h>

void Switch_add_case() {
    flecs::world world;

    auto Standing = world.entity("Standing");
    auto Walking = world.entity("Walking");
    auto Movement = world.entity().add(flecs::Union);

    auto e = world.entity()
        .add(Movement, Standing);
    test_assert(e.has(Movement, Standing));

    auto table = e.table();

    e.add(Movement, Walking);
    test_assert(e.table() == table);

    test_assert(e.has(Movement, Walking));
    test_assert(!e.has(Movement, Standing));
}

void Switch_get_case() {
    flecs::world world;

    auto Standing = world.entity("Standing");
    world.entity("Walking");
    auto Movement = world.entity().add(flecs::Union);

    auto e = world.entity()
        .add(Movement, Standing);
    test_assert(e.has(Movement, Standing));

    test_assert(e.target(Movement) == Standing);
}

void Switch_system_w_case() {
    flecs::world world;

    auto Standing = world.entity("Standing");
    auto Walking = world.entity("Walking");
    auto Movement = world.entity("Movement").add(flecs::Union);

    world.entity().add(Movement, Walking);
    world.entity().add(Movement, Walking);
    world.entity().add(Movement, Standing);

    int count = 0, invoke_count = 0;  
    world.system()
        .expr("(Movement, Walking)")
        .iter([&](flecs::iter it) {
            auto movement = it.field<flecs::entity_t>(1);

            invoke_count ++;
            for (auto i : it) {
                test_assert(movement[i] == Walking.id());
                count ++;
            }
        });

    world.progress();

    test_int(invoke_count, 2);
    test_int(count, 2);
}

void Switch_system_w_case_builder() {
    flecs::world world;

    auto Standing = world.entity("Standing");
    auto Walking = world.entity("Walking");
    auto Movement = world.entity().add(flecs::Union);

    world.entity().add(Movement, Walking);
    world.entity().add(Movement, Walking);
    world.entity().add(Movement, Standing);

    int count = 0, invoke_count = 0;  
    world.system()
        .term(Movement, Walking)
        .iter([&](flecs::iter it) {
            auto movement = it.field<flecs::entity_t>(1);

            invoke_count ++;
            for (auto i : it) {
                test_assert(movement[i] == Walking.id());
                count ++;
            }
        });

    world.progress();

    test_int(invoke_count, 2);
    test_int(count, 2);
}

void Switch_system_w_switch() {
    flecs::world world;

    auto Standing = world.entity("Standing");
    auto Walking = world.entity("Walking");
    auto Movement = world.entity("Movement").add(flecs::Union);

    auto e1 = world.entity().add(Movement, Walking);
    auto e2 = world.entity().add(Movement, Walking);
    auto e3 = world.entity().add(Movement, Standing);

    int count = 0, invoke_count = 0;  
    world.system()
        .expr("(Movement, *)")
        .iter([&](flecs::iter it) {
            flecs::column<flecs::entity_t> movement(it, 1);

            invoke_count ++;
            for (auto i : it) {
                if (it.entity(i) == e1 || it.entity(i) == e2) {
                    test_int(movement[i], Walking.id());
                } else if (it.entity(i) == e3) {
                    test_int(movement[i], Standing.id());   
                }
                count ++;
            }
        });

    world.progress();

    test_int(invoke_count, 1);
    test_int(count, 3);
}

struct Movement { };
struct Standing { };
struct Walking { };

void Switch_system_w_sw_type_builder() {
    flecs::world world;

    world.component<Movement>().add(flecs::Union);

    world.entity().add<Movement, Walking>();
    world.entity().add<Movement, Walking>();
    world.entity().add<Movement, Standing>();

    int count = 0, invoke_count = 0;  
    world.system<>()
        .term<Movement, Walking>()
        .iter([&](flecs::iter it) {
            auto movement = it.field<flecs::entity_t>(1);

            invoke_count ++;
            for (auto i : it) {
                test_assert(movement[i] == world.id<Walking>());
                count ++;
            }
        });

    world.progress();

    test_int(invoke_count, 2);
    test_int(count, 2);
}

void Switch_add_case_w_type() {
    flecs::world world;

    world.component<Movement>().add(flecs::Union);

    auto e = world.entity().add<Movement, Standing>();
    test_assert((e.has<Movement, Standing>()));

    e.add<Movement, Walking>();

    test_assert((e.has<Movement, Walking>()));
    test_assert((!e.has<Movement, Standing>()));
}

void Switch_add_switch_w_type() {
    flecs::world world;

    world.component<Movement>().add(flecs::Union);

    auto e = world.entity().add<Movement, Standing>();
    test_assert((e.has<Movement, Standing>()));

    e.add<Movement, Walking>();

    test_assert((e.has<Movement, Walking>()));
    test_assert((!e.has<Movement, Standing>()));
}

void Switch_add_remove_switch_w_type() {
    flecs::world world;

    world.component<Movement>().add(flecs::Union);

    auto e = world.entity().add<Movement, Standing>();
    test_assert(e.has<Movement>(flecs::Wildcard));
    test_assert((e.has<Movement, Standing>()));

    auto table = e.table();

    e.add<Movement, Walking>();

    test_assert((e.has<Movement, Walking>()));
    test_assert((!e.has<Movement, Standing>()));
    test_assert(e.table() == table);

    auto c = e.target<Movement>();
    test_assert(c != 0);
    test_assert(c == world.id<Walking>());

    e.remove<Movement>(flecs::Wildcard);
    test_assert(!e.has<Movement>(flecs::Wildcard));
    test_assert((!e.has<Movement, Walking>()));
    test_assert(e.table() != table);
}

enum Color {
    Red,
    Green,
    Blue
};

void Switch_switch_enum_type() {
    flecs::world world;

    world.component<Color>().add(flecs::Union);

    auto e = world.entity().add(Red);
    test_assert(e.has(Red));
    test_assert(!e.has(Green));
    test_assert(!e.has(Blue));
    test_assert(e.has<Color>(flecs::Wildcard));

    auto table = e.table();

    e.add(Green);
    test_assert(!e.has(Red));
    test_assert(e.has(Green));
    test_assert(!e.has(Blue));
    test_assert(e.has<Color>(flecs::Wildcard));
    test_assert(e.table() == table);

    e.add(Blue);
    test_assert(!e.has(Red));
    test_assert(!e.has(Green));
    test_assert(e.has(Blue));
    test_assert(e.has<Color>(flecs::Wildcard));
    test_assert(e.table() == table);

    e.remove<Color>();
    test_assert(!e.has(Red));
    test_assert(!e.has(Green));
    test_assert(!e.has(Blue));
    test_assert(!e.has<Color>(flecs::Wildcard));
    test_assert(e.table() != table);
}
