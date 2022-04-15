#include <cpp_api.h>

void Trigger_on_add() {
    flecs::world world;

    int invoked = 0;

    world.trigger<Position>()
        .event(flecs::OnAdd)
        .each([&](flecs::entity e, Position& p) {
            invoked ++;
        });

    world.entity()
        .add<Position>();

    test_int(invoked, 1);
}

void Trigger_on_remove() {
    flecs::world world;

    int invoked = 0;

    world.trigger<Position>()
        .event(flecs::OnRemove)
        .each([&](flecs::entity e, Position& p) {
            invoked ++;
        });

    auto e = world.entity()
        .add<Position>();

    test_int(invoked, 0);
    
    e.remove<Position>();

    test_int(invoked, 1);
}

struct MyTag { };

void Trigger_on_add_tag_action() {
    flecs::world world;

    int invoked = 0;

    world.trigger<MyTag>()
        .event(flecs::OnAdd)
        .iter([&](flecs::iter it, MyTag*) {
            invoked ++;
        });

    world.entity()
        .add<MyTag>();

    test_int(invoked, 1);
}

void Trigger_on_add_tag_iter() {
    flecs::world world;

    int invoked = 0;

    world.trigger<MyTag>()
        .event(flecs::OnAdd)
        .iter([&](flecs::iter it, MyTag*) {
            invoked ++;
        });

    world.entity()
        .add<MyTag>();

    test_int(invoked, 1);
}

void Trigger_on_add_tag_each() {
    flecs::world world;

    int invoked = 0;

    world.trigger<MyTag>()
        .event(flecs::OnAdd)
        .each([&](flecs::entity e, MyTag) {
            invoked ++;
        });

    world.entity()
        .add<MyTag>();

    test_int(invoked, 1);
}

void Trigger_trigger_w_self() {
    flecs::world world;

    auto self = world.entity();

    bool invoked = false;

    world.trigger<Position>()
        .event(flecs::OnAdd)
        .self(self)
        .iter([&](flecs::iter& it) {
            test_assert(it.self() == self);
            invoked = true;
        });

    world.entity().set<Position>({10, 20});

    test_bool(invoked, true);
}

void Trigger_on_add_id() {
   flecs::world world;

    int invoked = 0;

    world.trigger<>().id<Tag>()
        .event(flecs::OnAdd)
        .each([&](flecs::entity e) {
            invoked ++;
        });

    auto e = world.entity().add<Tag>();

    test_int(invoked, 1);
    
    e.remove<Tag>();

    test_int(invoked, 1); 
}

void Trigger_on_add_id_arg() {
   flecs::world world;

    int invoked = 0;

    auto tag = world.component<Tag>();

    world.trigger<>()
        .id(tag)
        .event(flecs::OnAdd)
        .each([&](flecs::entity e) {
            invoked ++;
        });

    auto e = world.entity().add(tag);

    test_int(invoked, 1);
    
    e.remove(tag);

    test_int(invoked, 1);
}

void Trigger_on_add_expr() {
    flecs::world world;

    int invoked = 0;

    world.component<Tag>();

    world.trigger<>().expr("Tag")
        .event(flecs::OnAdd)
        .each([&](flecs::entity e) {
            invoked ++;
        });

    auto e = world.entity().add<Tag>();

    test_int(invoked, 1);
    
    e.remove<Tag>();

    test_int(invoked, 1);
}

void Trigger_create_w_no_template_args() {
    flecs::world world;

    auto entity = world.entity();

    int invoked = 0;

    world.trigger()
        .id<Position>()
        .event(flecs::OnAdd)
        .each([&](flecs::entity e) {
            invoked ++;
            test_assert(e == entity);
        });

    entity.add<Position>();

    test_int(invoked, 1);
}

void Trigger_yield_existing() {
    flecs::world world;

    struct TagA { };
    struct TagB { };

    auto e1 = world.entity().add<TagA>();
    auto e2 = world.entity().add<TagA>();
    auto e3 = world.entity().add<TagA>().add<TagB>();

    int32_t count = 0;

    world.trigger<TagA>()
        .event(flecs::OnAdd)
        .yield_existing()
        .each([&](flecs::entity e, TagA) {
            if (e == e1) count ++;
            if (e == e2) count += 2;
            if (e == e3) count += 3;
        });

    test_int(count, 6);
}

void Trigger_default_ctor() {
    flecs::world world;

    struct TagA { };

    flecs::trigger o;
    test_assert(o == 0);

    int32_t count = 0;
    o = world.trigger<TagA>()
        .event(flecs::OnAdd)
        .each([&](flecs::entity e, TagA) {
            count ++;
        });
    
    world.entity().add<TagA>();
    
    test_int(count, 1);
}

void Trigger_entity_ctor() {
    flecs::world world;

    struct TagA { };

    flecs::trigger o = world.trigger<TagA>()
        .event(flecs::OnAdd)
        .each([&](flecs::entity e, TagA) { });
    
    flecs::entity oe = o;

    flecs::trigger eo = world.trigger(oe);
    test_assert(eo == o);
}
