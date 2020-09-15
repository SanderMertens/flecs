#include <cpp_api.h>

struct Trait {
    int value;
};

void ImplicitComponents_add() {
    flecs::world world;

    auto e = flecs::entity(world).add<Position>();

    test_str(e.type().str().c_str(), "Position");
    test_assert(e.has<Position>());

    auto position = world.lookup("Position");
    test_assert(position.id() != 0);
}

void ImplicitComponents_remove() {
    flecs::world world;

    auto e = flecs::entity(world).remove<Position>();

    test_assert(!e.has<Position>());

    auto position = world.lookup("Position");
    test_assert(position.id() != 0);
}

void ImplicitComponents_has() {
    flecs::world world;

    auto e = flecs::entity(world);
    test_assert(!e.has<Position>());

    auto position = world.lookup("Position");
    test_assert(position.id() != 0);
}

void ImplicitComponents_set() {
    flecs::world world;

    auto e = flecs::entity(world).set<Position>({10, 20});

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

    auto e = flecs::entity(world);

    auto *p = e.get<Position>();
    test_assert(p == nullptr);

    auto position = world.lookup("Position");
    test_assert(position.id() != 0);
}

void ImplicitComponents_add_trait() {
    flecs::world world;

    auto e = flecs::entity(world).add_trait<Trait, Position>();

    test_str(e.type().str().c_str(), "TRAIT|Trait>Position");
    test_assert((e.has_trait<Trait, Position>()));

    auto position = world.lookup("Position");
    test_assert(position.id() != 0);

    auto trait = world.lookup("Trait");
    test_assert(trait.id() != 0);    
}

void ImplicitComponents_remove_trait() {
    flecs::world world;

    auto e = flecs::entity(world).remove_trait<Position, Trait>();

    test_assert((!e.has_trait<Position, Trait>()));

    auto position = world.lookup("Position");
    test_assert(position.id() != 0);

    auto trait = world.lookup("Trait");
    test_assert(trait.id() != 0);   
}

void ImplicitComponents_module() {
    flecs::world world;

    flecs::module<Position>(world);

    auto position = world.lookup("Position");
    test_assert(position.id() != 0);
}

void ImplicitComponents_system() {
    flecs::world world;

    flecs::system<Position, Velocity>(world)
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

    flecs::system<Rotation*, Mass*>(world)
        .each([&](flecs::entity e, Rotation* r, Mass* m) {
            if (r) {
                rotation_count ++;
            }
            if (m) {
                mass_count ++;
            }
        });

    flecs::entity(world).set<Rotation>({10});
    flecs::entity(world).set<Mass>({20});

    flecs::entity(world)
        .set<Rotation>({30})
        .set<Mass>({40});

    auto rotation = world.lookup("Rotation");
    test_assert(rotation.id() != 0);

    auto mass = world.lookup("Mass");
    test_assert(mass.id() != 0);  

    auto rcomp = flecs::component<Rotation>(world);
    test_assert(rcomp == rotation);

    auto mcomp = flecs::component<Mass>(world);
    test_assert(mcomp == mass);    

    world.progress();

    test_int(rotation_count, 2);
    test_int(mass_count, 2);
}

void ImplicitComponents_system_const() {
    flecs::world world;

    int count = 0;
    flecs::system<Position, const Velocity>(world)
        .each([&](flecs::entity e, Position& p, const Velocity& v) {
            p.x += v.x;
            p.y += v.y;
            count ++;
        });

    auto position = world.lookup("Position");
    test_assert(position.id() != 0);

    auto velocity = world.lookup("Velocity");
    test_assert(velocity.id() != 0);  

    auto e = flecs::entity(world)
        .set<Position>({10, 20})
        .set<Velocity>({1, 2});

    auto pcomp = flecs::component<Position>(world);
    test_assert(pcomp == position);

    auto vcomp = flecs::component<Velocity>(world);
    test_assert(vcomp == velocity);    

    world.progress();

    test_int(count, 1);

    const Position *p = e.get<Position>();
    test_int(p->x, 11);
    test_int(p->y, 22);
}

void ImplicitComponents_query() {
    flecs::world world;

    auto q = flecs::query<Position, Velocity>(world);

    q.each([](flecs::entity e, Position& p, Velocity &v) { });

    auto position = world.lookup("Position");
    test_assert(position.id() != 0);

    auto velocity = world.lookup("Velocity");
    test_assert(velocity.id() != 0);  
}

void ImplicitComponents_implicit_name() {
    flecs::world world;

    auto pcomp = flecs::component<Position>(world);

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
    flecs::_::component_info<Position>::reset();

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
    flecs::_::component_info<Foo::Position>::reset();

    world.entity()
        .add<Foo::Position>();

    test_assert(flecs::type_id<Foo::Position>() == comp_1.id());
}

static int position_ctor_invoked = 0;

ECS_CTOR(Position, ptr, {
    position_ctor_invoked ++;
});

void ImplicitComponents_reinit_w_lifecycle() {
    flecs::world world;

    auto comp_1 = world.pod_component<Position>();

    test_assert(flecs::type_id<Position>() == comp_1.id());

    // Explicitly register constructor
    EcsComponentLifecycle cl{};
    cl.ctor = ecs_ctor(Position);
    ecs_set_component_actions_w_entity(world.c_ptr(), comp_1.id(), &cl);

    auto e = world.entity()
        .add<Position>();
    test_assert(e.has<Position>());
    test_int(position_ctor_invoked, 1);

    // Reset component id using internals (currently the only way to simulate
    // registration across translation units)
    flecs::_::component_info<Position>::reset();

    e = world.entity()
        .add<Position>();
    test_assert(e.has<Position>());
    test_int(position_ctor_invoked, 2);

    test_assert(flecs::type_id<Position>() == comp_1.id());
}
