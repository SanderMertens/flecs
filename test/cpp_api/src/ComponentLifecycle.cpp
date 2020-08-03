#include <cpp_api.h>

class POD {
public:
    POD() {
        ctor_invoked ++;
        value = 10;
    }

    POD(int v) {
        ctor_invoked ++;
        value = v;
    }    

    ~POD() {
        dtor_invoked ++;
    }

    POD& operator=(const POD& obj) {
        copy_invoked ++;
        this->value = obj.value;
        return *this;
    }

    POD& operator=(POD&& obj) {
        move_invoked ++;
        this->value = obj.value;
        return *this;
    }   

    int value;

    static int ctor_invoked;
    static int dtor_invoked;
    static int copy_invoked;
    static int move_invoked;
};

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
    test_int(POD::copy_invoked, 1);
    test_int(POD::move_invoked, 0);
}

void ComponentLifecycle_copy_on_override() {
    flecs::world world;

    flecs::component<POD>(world, "POD");

    auto base = flecs::entity(world);
    test_assert(base.id() != 0);

    base.set<POD>({10});
    test_int(POD::ctor_invoked, 2);
    test_int(POD::dtor_invoked, 1);
    test_int(POD::copy_invoked, 1);
    POD::ctor_invoked = 0;
    POD::dtor_invoked = 0;
    POD::copy_invoked = 0;
    POD::move_invoked = 0;

    auto e = flecs::entity(world);
    test_assert(e.id() != 0);
    
    e.add_instanceof(base);
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

void ComponentLifecycle_move_on_merge() {
    flecs::world world;

    flecs::component<POD>(world, "POD");

    auto e = flecs::entity(world);
    test_assert(e.id() != 0);

    e.set<POD>({10});
    test_int(POD::ctor_invoked, 2);
    test_int(POD::dtor_invoked, 1);
    test_int(POD::copy_invoked, 1);
    test_int(POD::move_invoked, 0);

    POD::ctor_invoked = 0;
    POD::dtor_invoked = 0;
    POD::copy_invoked = 0;
    POD::move_invoked = 0;

    flecs::system<POD>(world)
        .each([](flecs::entity e_arg, POD& pod) {
            e_arg.set<POD>({20});
            test_int(POD::ctor_invoked, 2); // 1 for construction in stage
            test_int(POD::dtor_invoked, 1);
            test_int(POD::copy_invoked, 2); // 1 for copy to stage
            test_int(POD::move_invoked, 0);

            POD::ctor_invoked = 0;
            POD::dtor_invoked = 0;
            POD::copy_invoked = 0;
            POD::move_invoked = 0;            
        });
        
    world.progress();

    test_int(POD::ctor_invoked, 1); // construct new value
    test_int(POD::dtor_invoked, 1); // destruct old value
    test_int(POD::copy_invoked, 0);
    test_int(POD::move_invoked, 1); // move staged value to main stage

    // The reason why the main stage value is destructed before merging is so
    // that entities can be merged in bulk, vs copying values one by one from
    // the stage to the main stage
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
    
    e.add_instanceof(base);

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
