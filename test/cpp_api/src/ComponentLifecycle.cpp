#include <cpp_api.h>

#include <string>

int POD::ctor_invoked = 0;
int POD::dtor_invoked = 0;
int POD::copy_invoked = 0;
int POD::move_invoked = 0;

class Str {
public:
    std::string value;
};

void ComponentLifecycle_ctor_on_add() {
    flecs::world world;

    flecs::component<POD>(world, "POD");

    auto e = flecs::entity(world).add<POD>();
    test_assert(e.id() != 0);
    test_assert(e.has<POD>());

    const POD *pod = e.get<POD>();
    test_assert(pod != NULL);
    test_int(POD::ctor_invoked, 1);
    test_int(POD::dtor_invoked, 0);
    test_int(POD::copy_invoked, 0);
    test_int(POD::move_invoked, 0);
    
    test_int(pod->value, 10);
}

void ComponentLifecycle_dtor_on_remove() {
    flecs::world world;

    flecs::component<POD>(world, "POD");

    auto e = flecs::entity(world).add<POD>();
    test_assert(e.id() != 0);
    test_assert(e.has<POD>());
    test_int(POD::ctor_invoked, 1);

    e.remove<POD>();
    test_assert(!e.has<POD>());
    test_int(POD::ctor_invoked, 1);
    test_int(POD::dtor_invoked, 1);
    test_int(POD::copy_invoked, 0);
    test_int(POD::move_invoked, 0);
}

void ComponentLifecycle_copy_on_set() {
    flecs::world world;

    flecs::component<POD>(world, "POD");

    auto e = flecs::entity(world);
    test_assert(e.id() != 0);

    e.set<POD>({20});
    test_assert(e.has<POD>());
    test_int(POD::ctor_invoked, 2);
    test_int(POD::dtor_invoked, 1);
    test_int(POD::copy_invoked, 0);
    test_int(POD::move_invoked, 1);
}

void ComponentLifecycle_copy_on_override() {
    flecs::world world;

    flecs::component<POD>(world, "POD");

    auto base = flecs::entity(world);
    test_assert(base.id() != 0);

    base.set<POD>({10});
    test_int(POD::ctor_invoked, 2);
    test_int(POD::dtor_invoked, 1);
    test_int(POD::copy_invoked, 0);
    POD::ctor_invoked = 0;
    POD::dtor_invoked = 0;
    POD::copy_invoked = 0;
    POD::move_invoked = 0;

    auto e = flecs::entity(world);
    test_assert(e.id() != 0);
    
    e.add(flecs::IsA, base);
    test_int(POD::ctor_invoked, 0);

    e.add<POD>();
    test_int(POD::ctor_invoked, 1);
    test_int(POD::dtor_invoked, 0);
    test_int(POD::copy_invoked, 1);
    test_int(POD::move_invoked, 0);

    const POD *pod = e.get<POD>();
    test_assert(pod != NULL);
    test_int(pod->value, 10);
}

void ComponentLifecycle_non_pod_add() {
    flecs::world world;

    flecs::component<Str>(world, "Str");

    auto e = flecs::entity(world).add<Str>();
    test_assert(e.id() != 0);
    test_assert(e.has<Str>());

    const Str *str = e.get<Str>();
    test_assert(str != NULL);
    test_assert(str->value == "");
}

void ComponentLifecycle_non_pod_remove() {
    flecs::world world;

    flecs::component<Str>(world, "Str");

    auto e = flecs::entity(world).add<Str>();
    test_assert(e.id() != 0);
    test_assert(e.has<Str>());

    e.remove<Str>();
    test_assert(!e.has<Str>());
}

void ComponentLifecycle_non_pod_set() {
    flecs::world world;

    flecs::component<Str>(world, "Str");

    auto e = flecs::entity(world)
        .set<Str>({"Hello World"});
    test_assert(e.id() != 0);
    test_assert(e.has<Str>());

    const Str *str = e.get<Str>();
    test_assert(str != NULL);
    test_assert(str->value == "Hello World");
}

void ComponentLifecycle_non_pod_override() {
    flecs::world world;

    flecs::component<Str>(world, "Str");

    auto base = flecs::entity(world);
    test_assert(base.id() != 0);

    base.set<Str>({"Hello World"});

    auto e = flecs::entity(world);
    test_assert(e.id() != 0);
    
    e.add(flecs::IsA, base);

    e.add<Str>();

    const Str *str = e.get<Str>();
    test_assert(str != NULL);
    test_assert(str->value == "Hello World");
}

void ComponentLifecycle_get_mut_new() {
    flecs::world world;

    flecs::component<POD>(world, "POD");

    auto e = flecs::entity(world);
    test_assert(e.id() != 0);

    POD* value = e.get_mut<POD>();
    test_assert(value != NULL);

    POD::ctor_invoked = 1;
    POD::dtor_invoked = 0;
    POD::copy_invoked = 0;
    POD::move_invoked = 0;

    e.modified<POD>();

    POD::ctor_invoked = 1;
    POD::dtor_invoked = 0;
    POD::copy_invoked = 0;
    POD::move_invoked = 0;    
}

void ComponentLifecycle_get_mut_existing() {
    flecs::world world;

    flecs::component<POD>(world, "POD");

    auto e = flecs::entity(world);
    test_assert(e.id() != 0);

    POD* value = e.get_mut<POD>();
    test_assert(value != NULL);

    POD::ctor_invoked = 1;
    POD::dtor_invoked = 0;
    POD::copy_invoked = 0;
    POD::move_invoked = 0;

    value = e.get_mut<POD>();
    test_assert(value != NULL);

    /* Repeated calls to get_mut should not invoke constructor */
    POD::ctor_invoked = 1;
    POD::dtor_invoked = 0;
    POD::copy_invoked = 0;
    POD::move_invoked = 0;
}

void ComponentLifecycle_pod_component() {
    flecs::world world;

    flecs::pod_component<POD>(world, "POD");

    auto e = flecs::entity(world).add<POD>();
    test_assert(e.id() != 0);
    test_assert(e.has<POD>());

    const POD *pod = e.get<POD>();
    test_assert(pod != NULL);

    e.remove<POD>();
    test_assert(!e.has<POD>());

    /* Component is registered as pod, no lifecycle actions should be invoked */
    test_int(POD::ctor_invoked, 0);
    test_int(POD::dtor_invoked, 0);
    test_int(POD::copy_invoked, 0);
    test_int(POD::move_invoked, 0);
}

void ComponentLifecycle_relocatable_component() {
    flecs::world world;

    flecs::relocatable_component<POD>(world, "POD");

    auto e = flecs::entity(world).add<POD>();
    test_assert(e.id() != 0);
    test_assert(e.has<POD>());

    const POD *pod = e.get<POD>();
    test_assert(pod != NULL);

    test_int(pod->value, 10);

    /* Component is registered as relocatable, ctor/dtor/copy are registered,
     * but move is not. */
    test_int(POD::ctor_invoked, 1);
    test_int(POD::dtor_invoked, 0);
    test_int(POD::copy_invoked, 0);
    test_int(POD::move_invoked, 0);

    /* Add another entity, this moves the existing component, but should not
     * invoke move assignment */
    flecs::entity(world).add<POD>();
    test_int(POD::ctor_invoked, 2);
    test_int(POD::move_invoked, 0);     
}

void ComponentLifecycle_implicit_component() {
    flecs::world world;

    auto e = flecs::entity(world).add<POD>();
    test_assert(e.id() != 0);
    test_assert(e.has<POD>());
    test_int(POD::ctor_invoked, 1);

    const POD *pod = e.get<POD>();
    test_assert(pod != NULL);

    test_int(pod->value, 10);

    test_int(POD::ctor_invoked, 1);
    test_int(POD::dtor_invoked, 0);
    test_int(POD::copy_invoked, 0);
    test_int(POD::move_invoked, 0);

    flecs::entity(world).add<POD>();
    flecs::entity(world).add<POD>();
    test_int(POD::ctor_invoked, 5);
    test_int(POD::move_invoked, 2); 
}

void ComponentLifecycle_implicit_after_query() {
    flecs::world world;

    world.query<POD>();

    auto e = flecs::entity(world).add<POD>();
    test_assert(e.id() != 0);
    test_assert(e.has<POD>());
    test_int(POD::ctor_invoked, 1);

    const POD *pod = e.get<POD>();
    test_assert(pod != NULL);

    test_int(pod->value, 10);

    test_int(POD::ctor_invoked, 1);
    test_int(POD::dtor_invoked, 0);
    test_int(POD::copy_invoked, 0);
    test_int(POD::move_invoked, 0);

    flecs::entity(world).add<POD>();
    flecs::entity(world).add<POD>();
    test_int(POD::ctor_invoked, 5);
    test_int(POD::move_invoked, 2); 
}
