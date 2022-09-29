#include <cpp_api.h>

struct Pair {
    int value;
};

void ImplicitComponents_add() {
    flecs::world world;

    auto e = world.entity().add<Position>();

    test_str(e.type().str().c_str(), "Position");
    test_assert(e.has<Position>());

    auto position = world.lookup("Position");
    test_assert(position.id() != 0);
}

void ImplicitComponents_remove() {
    flecs::world world;

    auto e = world.entity().remove<Position>();

    test_assert(!e.has<Position>());

    auto position = world.lookup("Position");
    test_assert(position.id() != 0);
}

void ImplicitComponents_has() {
    flecs::world world;

    auto e = world.entity();
    test_assert(!e.has<Position>());

    auto position = world.lookup("Position");
    test_assert(position.id() != 0);
}

void ImplicitComponents_set() {
    flecs::world world;

    auto e = world.entity().set<Position>({10, 20});

    test_str(e.type().str().c_str(), "Position");
    test_assert(e.has<Position>());
    auto *p = e.get<Position>();
    test_int(p->x, 10);
    test_int(p->y, 20);

    auto position = world.lookup("Position");
    test_assert(position.id() != 0);
}

void ImplicitComponents_get() {
    flecs::world world;

    auto e = world.entity();

    auto *p = e.get<Position>();
    test_assert(p == nullptr);

    auto position = world.lookup("Position");
    test_assert(position.id() != 0);
}

void ImplicitComponents_add_pair() {
    flecs::world world;

    auto e = world.entity().add<Pair, Position>();

    test_str(e.type().str().c_str(), "(Pair,Position)");
    test_assert((e.has<Pair, Position>()));

    auto position = world.lookup("Position");
    test_assert(position.id() != 0);

    auto pair = world.lookup("Pair");
    test_assert(pair.id() != 0);    
}

void ImplicitComponents_remove_pair() {
    flecs::world world;

    auto e = world.entity().remove<Position, Pair>();

    test_assert((!e.has<Position, Pair>()));

    auto position = world.lookup("Position");
    test_assert(position.id() != 0);

    auto pair = world.lookup("Pair");
    test_assert(pair.id() != 0);   
}

void ImplicitComponents_module() {
    flecs::world world;

    world.module<Position>();

    auto position = world.lookup("Position");
    test_assert(position.id() != 0);
}

void ImplicitComponents_system() {
    flecs::world world;

    world.system<Position, Velocity>()
        .each([](flecs::entity e, Position& p, Velocity& v) {
        });

    auto position = world.lookup("Position");
    test_assert(position.id() != 0);

    auto velocity = world.lookup("Velocity");
    test_assert(velocity.id() != 0);    
}

void ImplicitComponents_system_optional() {
    flecs::world world;

    int rotation_count = 0;
    int mass_count = 0;

    world.system<Rotation*, Mass*>()
        .each([&](flecs::entity e, Rotation* r, Mass* m) {
            if (r) {
                rotation_count ++;
            }
            if (m) {
                mass_count ++;
            }
        });

    world.entity().set<Rotation>({10});
    world.entity().set<Mass>({20});

    world.entity()
        .set<Rotation>({30})
        .set<Mass>({40});

    auto rotation = world.lookup("Rotation");
    test_assert(rotation.id() != 0);

    auto mass = world.lookup("Mass");
    test_assert(mass.id() != 0);  

    auto rcomp = world.component<Rotation>();
    test_assert(rcomp == rotation);

    auto mcomp = world.component<Mass>();
    test_assert(mcomp == mass);    

    world.progress();

    test_int(rotation_count, 2);
    test_int(mass_count, 2);
}

void ImplicitComponents_system_const() {
    flecs::world world;

    int count = 0;
    world.system<Position, const Velocity>()
        .each([&](flecs::entity e, Position& p, const Velocity& v) {
            p.x += v.x;
            p.y += v.y;
            count ++;
        });

    auto position = world.lookup("Position");
    test_assert(position.id() != 0);

    auto velocity = world.lookup("Velocity");
    test_assert(velocity.id() != 0);  

    auto e = world.entity()
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    auto pcomp = world.component<Position>();
    test_assert(pcomp == position);

    auto vcomp = world.component<Velocity>();
    test_assert(vcomp == velocity);    

    world.progress();

    test_int(count, 1);

    const Position *p = e.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);
}

void ImplicitComponents_query() {
    flecs::world world;

    auto q = world.query<Position, Velocity>();

    q.each([](flecs::entity e, Position& p, Velocity &v) { });

    auto position = world.lookup("Position");
    test_assert(position.id() != 0);

    auto velocity = world.lookup("Velocity");
    test_assert(velocity.id() != 0);  
}

void ImplicitComponents_implicit_name() {
    flecs::world world;

    auto pcomp = world.component<Position>();

    auto position = world.lookup("Position");
    test_assert(position.id() != 0);

    test_assert(pcomp == position);
}

void ImplicitComponents_reinit() {
    flecs::world world;

    auto comp_1 = world.component<Position>();

    test_assert(flecs::type_id<Position>() == comp_1.id());

    // Reset component id using internals (currently the only way to simulate
    // registration across translation units)
    flecs::_::cpp_type<Position>::reset();

    world.entity()
        .add<Position>();

    test_assert(flecs::type_id<Position>() == comp_1.id());
}

namespace Foo {
    struct Position {
        float x;
        float y;
    };
}

void ImplicitComponents_reinit_scoped() {
    flecs::world world;

    auto comp_1 = world.component<Foo::Position>();

    test_assert(flecs::type_id<Foo::Position>() == comp_1.id());

    // Reset component id using internals (currently the only way to simulate
    // registration across translation units)
    flecs::_::cpp_type<Foo::Position>::reset();

    world.entity()
        .add<Foo::Position>();

    test_assert(flecs::type_id<Foo::Position>() == comp_1.id());
}

static int position_ctor_invoked = 0;

ECS_CTOR(Position, ptr, {
    position_ctor_invoked ++;
})

void ImplicitComponents_reinit_w_lifecycle() {
    flecs::world world;

    auto comp_1 = world.component<Position>();

    test_assert(flecs::type_id<Position>() == comp_1.id());

    // Explicitly register constructor
    ecs_type_hooks_t cl{};
    cl.ctor = ecs_ctor(Position);
    ecs_set_hooks_id(world.c_ptr(), comp_1.id(), &cl);

    auto e = world.entity()
        .add<Position>();
    test_assert(e.has<Position>());
    test_int(position_ctor_invoked, 1);

    // Reset component id using internals (currently the only way to simulate
    // registration across translation units)
    flecs::_::cpp_type<Position>::reset();

    e = world.entity()
        .add<Position>();
    test_assert(e.has<Position>());
    test_int(position_ctor_invoked, 2);

    test_assert(flecs::type_id<Position>() == comp_1.id());
}

void ImplicitComponents_first_use_in_system() {
    flecs::world world;

    world.system<Position>()
        .each([](flecs::entity e, Position& p) {
            e.add<Velocity>();
        });

    auto e = world.entity().add<Position>();

    world.progress();

    test_assert(e.has<Velocity>());
}

namespace ns {
    struct NsTag { };
}

void ImplicitComponents_first_use_tag_in_system() {
    flecs::world world;

    world.system<Position>()
        .each([](flecs::entity e, Position& p) {
            e.add<Tag>();
            e.add<ns::NsTag>();
        });

    auto e = world.entity().add<Position>();

    world.progress();

    test_assert(e.has<Tag>());
}

enum Color {
    Red,
    Green,
    Blue
};

void ImplicitComponents_first_use_enum_in_system() {
    flecs::world world;

    world.system<Position>()
        .each([](flecs::entity e, Position& p) {
            e.add<Tag>();
            e.add(Color::Green);
        });

    auto e = world.entity().add<Position>();

    world.progress();

    test_assert(e.has<Position>());
    test_assert(e.has<Tag>());
    test_assert(e.has(Color::Green));

    test_assert(world.component<Color>().has(flecs::Exclusive));
}

void ImplicitComponents_use_const() {
    flecs::world world;

    world.use<const Position>();

    auto e = world.entity()
        .set<Position>({10, 20});

    test_assert(e.has<Position>());

    const Position *p = e.get<Position>();
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void ImplicitComponents_use_const_w_stage() {
    flecs::world world;

    world.use<const Velocity>();

    auto e = world.entity()
        .set<Position>({10, 20});

    world.system<Position>()
        .each([](flecs::entity e, Position&) {
            e.set<Velocity>({1, 2});
        });

    world.progress();

    test_assert(e.has<Velocity>());

    const Velocity *v = e.get<Velocity>();
    test_int(v->x, 1);
    test_int(v->y, 2);
}

void ImplicitComponents_use_const_w_threads() {
    flecs::world world;

    world.use<const Velocity>();

    auto e = world.entity()
        .set<Position>({10, 20});

    world.system<Position>()
        .each([](flecs::entity e, Position&) {
            e.set<Velocity>({1, 2});
        });

    world.set_threads(2);

    world.progress();

    test_assert(e.has<Velocity>());

    const Velocity *v = e.get<Velocity>();
    test_int(v->x, 1);
    test_int(v->y, 2);
}

void ImplicitComponents_implicit_base() {
    flecs::world world;

    auto v = world.use<Position>();

    test_int(v.id(), flecs::type_id<Position>());
    test_int(v.id(), flecs::type_id<const Position>());
    test_int(v.id(), flecs::type_id<Position*>());
    test_int(v.id(), flecs::type_id<Position&>());
}

void ImplicitComponents_implicit_const() {
    flecs::world world;

    auto v = world.use<const Position>();

    test_int(v.id(), flecs::type_id<Position>());
    test_int(v.id(), flecs::type_id<const Position>());
    test_int(v.id(), flecs::type_id<Position*>());
    test_int(v.id(), flecs::type_id<Position&>());
}

void ImplicitComponents_implicit_ref() {
    flecs::world world;

    auto v = world.use<Position&>();

    test_int(v.id(), flecs::type_id<Position>());
    test_int(v.id(), flecs::type_id<const Position>());
    test_int(v.id(), flecs::type_id<Position*>());
    test_int(v.id(), flecs::type_id<Position&>());
}

void ImplicitComponents_implicit_ptr() {
    flecs::world world;

    auto v = world.use<Position*>();

    test_int(v.id(), flecs::type_id<Position>());
    test_int(v.id(), flecs::type_id<const Position>());
    test_int(v.id(), flecs::type_id<Position*>());
    test_int(v.id(), flecs::type_id<Position&>());
}

void ImplicitComponents_implicit_const_ref() {
    flecs::world world;

    auto v = world.use<const Position&>();

    test_int(v.id(), flecs::type_id<Position>());
    test_int(v.id(), flecs::type_id<const Position>());
    test_int(v.id(), flecs::type_id<Position*>());
    test_int(v.id(), flecs::type_id<Position&>());
}

