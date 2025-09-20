#include <cpp.h>

void quickstart_Concepts_World_01(void) {
    flecs::world world;

    // Do the ECS stuff

        test_assert(true);
}

void quickstart_Concepts_Entity_01(void) {
    flecs::world world;
    auto e = world.entity();
    e.is_alive(); // true!

    e.destruct();
    e.is_alive(); // false!

        test_assert(true);
}

void quickstart_Concepts_Entity_02(void) {
    flecs::world world;
    auto e = world.entity("Bob");

    std::cout << "Entity name: " << e.name() << std::endl;

        test_assert(true);
}

void quickstart_Concepts_Entity_03(void) {
    flecs::world world;
    auto e = world.lookup("Bob");

        test_assert(true);
}

void quickstart_Concepts_Component_01(void) {
    flecs::world world;
    auto e = world.entity();

    // Add a component. This creates the component in the ECS storage, but does not
    // assign it with a value.
    e.add<Velocity>();

    // Set the value for the Position & Velocity components. A component will be
    // added if the entity doesn't have it yet.
    e.set<Position>({10, 20})
     .set<Velocity>({1, 2});

    // Get a component
    const Position& p = e.get<Position>();

    // Get mutable:
    // Position& p = e.get_mut<Position>();

    // Get optional:
    // const Position* p = e.try_get<Position>();

    // Get optional mutable:
    // Position* p = e.try_get_mut<Position>();

    // Remove component
    e.remove<Position>();

        test_assert(true);
}

void quickstart_Concepts_Component_02(void) {
    flecs::world world;
    struct Serializable { };
    flecs::entity pos_e = world.entity<Position>();
    std::cout << "Name: " << pos_e.name() << std::endl;  // outputs 'Name: Position'

    // It's possible to add components like you would for any entity
    pos_e.add<Serializable>();

        test_assert(true);
}

void quickstart_Concepts_Component_03(void) {
    flecs::world world;
    flecs::entity pos_e = world.entity<Position>();

    const EcsComponent& c = pos_e.get<flecs::Component>();
    std::cout << "Component size: " << c.size << std::endl;

        test_assert(true);
}

void quickstart_Concepts_Tag_01(void) {
    flecs::world world;
    // Option 1: create Tag as empty struct
    struct Enemy { };

    // Create entity, add Enemy tag
    auto e = world.entity().add<Enemy>();
    e.has<Enemy>(); // true!

    e.remove<Enemy>();
    e.has<Enemy>(); // false!


    // Option 2: create Tag as entity
    auto Enemy = world.entity();

    // Create entity, add Enemy tag
    auto e2 = world.entity().add(Enemy);
    e2.has(Enemy); // true!

    e2.remove(Enemy);
    e2.has(Enemy); // false!

        test_assert(true);
}

void quickstart_Concepts_Pair_01(void) {
    flecs::world world;
    // Create Likes relationship as empty type (tag)
    struct Likes { };

    // Create a small graph with two entities that like each other
    auto Bob = world.entity();
    auto Alice = world.entity();

    Bob.add<Likes>(Alice); // Bob likes Alice
    Alice.add<Likes>(Bob); // Alice likes Bob
    Bob.has<Likes>(Alice); // true!

    Bob.remove<Likes>(Alice);
    Bob.has<Likes>(Alice); // false!

        test_assert(true);
}

void quickstart_Concepts_Pair_02(void) {
    flecs::world world;
    flecs::entity Bob = world.entity();
    flecs::id id = world.pair<Likes>(Bob);

        test_assert(true);
}

void quickstart_Concepts_Pair_03(void) {
    flecs::world world;
    flecs::entity Bob = world.entity();
    flecs::id id = world.pair<Likes>(Bob);
    if (id.is_pair()) {
        auto relationship = id.first();
        auto target = id.second();
    }

        test_assert(true);
}

void quickstart_Concepts_Pair_04(void) {
    flecs::world world;
    flecs::entity Apples = world.entity();
    flecs::entity Pears = world.entity();
    flecs::entity Eats = world.entity();
    flecs::entity Grows = world.entity();
    flecs::entity bob = world.entity();
    bob.add(Eats, Apples);
    bob.add(Eats, Pears);
    bob.add(Grows, Pears);

    bob.has(Eats, Apples); // true!
    bob.has(Eats, Pears);  // true!
    bob.has(Grows, Pears); // true!

        test_assert(true);
}

void quickstart_Concepts_Pair_05(void) {
    flecs::world world;
    flecs::entity Bob = world.entity();
    flecs::entity alice = world.entity();
    alice.add<Likes>(Bob);
    auto o = alice.target<Likes>(); // Returns Bob

        test_assert(true);
}

void quickstart_Concepts_Hierarchies_01(void) {
    flecs::world world;
    auto parent = world.entity();
    auto child = world.entity().child_of(parent);

    // Deleting the parent also deletes its children
    parent.destruct();

        test_assert(true);
}

void quickstart_Concepts_Hierarchies_02(void) {
    flecs::world world;
    auto parent = world.entity("parent");
    auto child = world.entity("child").child_of(parent);
    std::cout << child.path() << std::endl; // output: 'parent::child'

    world.lookup("parent::child"); // returns child
    parent.lookup("child"); // returns child

        test_assert(true);
}

void quickstart_Concepts_Hierarchies_03(void) {
    flecs::world world;
    auto q = world.query_builder<Position, Position>()
        .term_at(1).parent().cascade()
        .build();

    q.each([](Position& p, Position& p_parent) {
        // Do the thing
    });

        test_assert(true);
}

void quickstart_Concepts_Type_01(void) {
    flecs::world world;
    auto e = world.entity()
        .add<Position>()
        .add<Velocity>();

    std::cout << e.type().str() << std::endl; // output: 'Position,Velocity'

        test_assert(true);
}

void quickstart_Concepts_Type_02(void) {
    flecs::world world;
    auto e = world.entity();
    e.each([&](flecs::id id) {
        if (id == world.id<Position>()) {
            // Found Position component!
        }
    });

        test_assert(true);
}

void quickstart_Concepts_Type_03(void) {
    flecs::world world;
    world.component<Gravity>().add(flecs::Singleton);
    flecs::entity grav_e = world.entity<Gravity>();

    grav_e.set<Gravity>({9.81});

    const Gravity& g = grav_e.get<Gravity>();

        test_assert(true);
}

void quickstart_Concepts_Type_04(void) {
    flecs::world world;
    world.component<Gravity>().add(flecs::Singleton);
    world.query_builder<Velocity, Gravity>()
        .build();

        test_assert(true);
}

void quickstart_Concepts_Query_01(void) {
    flecs::world world;
    flecs::entity parent = world.entity();
    // For simple queries the world::each function can be used
    world.each([](Position& p, Velocity& v) { // flecs::entity argument is optional
        p.x += v.x;
        p.y += v.y;
    });

    // More complex queries can first be created, then iterated
    auto q = world.query_builder<Position>()
        .with(flecs::ChildOf, parent)
        .build();

    // Option 1: the each() callback iterates over each entity
    q.each([](flecs::entity e, Position& p) {
        std::cout << e.name() << ": {" << p.x << ", " << p.y << "}" << std::endl;
    });

    // Option 2: the run() callback offers more control over the iteration
    q.run([](flecs::iter& it) {
        while (it.next()) {
            auto p = it.field<Position>(0);

            for (auto i : it) {
                std::cout << it.entity(i).name()
                    << ": {" << p[i].x << ", " << p[i].y << "}" << std::endl;
            }
        }
    });

        test_assert(true);
}

void quickstart_Concepts_Query_02(void) {
    flecs::world world;
    flecs::query<> q = world.query_builder()
        .with(flecs::ChildOf, flecs::Wildcard)
        .with<Position>().oper(flecs::Not)
        .build();

    // Iteration code is the same

        test_assert(true);
}

void quickstart_Concepts_System_01(void) {
    flecs::world world;
    // Use each() function that iterates each individual entity
    auto move_sys = world.system<Position, Velocity>()
        .each([](flecs::iter& it, size_t, Position& p, Velocity& v) {
            p.x += v.x * it.delta_time();
            p.y += v.y * it.delta_time();
        });

        // Just like with queries, systems have both the run() and
        // each() methods to iterate entities.

    move_sys.run();

        test_assert(true);
}

void quickstart_Concepts_System_02(void) {
    flecs::world world;
    auto move_sys = world.system<>().each([](){});
    std::cout << "System: " << move_sys.name() << std::endl;
    move_sys.add(flecs::OnUpdate);
    move_sys.destruct();

        test_assert(true);
}

void quickstart_Concepts_Pipeline_01(void) {
    flecs::world world;
    auto move_sys = world.system<>().each([](){});
    move_sys.add(flecs::OnUpdate);
    move_sys.remove(flecs::PostUpdate);

        test_assert(true);
}

void quickstart_Concepts_Observer_01(void) {
    flecs::world world;
    world.observer<Position, Velocity>("OnSetPosition")
        .event(flecs::OnSet)
        // Callback code is same as system
        .each([](Position& p, Velocity& v) { });

    auto e = world.entity();     // Doesn't invoke the observer
    e.set<Position>({10, 20}); // Doesn't invoke the observer
    e.set<Velocity>({1, 2});   // Invokes the observer
    e.set<Position>({20, 30}); // Invokes the observer

        test_assert(true);
}

void quickstart_Concepts_Module_01(void) {
    struct my_module {
        my_module(flecs::world& world) {
            world.module<my_module>();

            // Define components, systems, triggers, ... as usual. They will be
            // automatically created inside the scope of the module.
        }
    };

    flecs::world world;

    // Import code
    world.import<my_module>();

        test_assert(true);
}
