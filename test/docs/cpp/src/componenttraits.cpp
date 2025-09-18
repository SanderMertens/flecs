#include <cpp.h>

void componenttraits_CanToggle_trait_01(void) {
    flecs::world world;
    world.component<Position>().add(flecs::CanToggle);

    flecs::entity e = world.entity().set(Position{10, 20});

    e.disable<Position>(); // Disable component
    test_assert(!e.enabled<Position>());

    e.enable<Position>(); // Enable component
    test_assert(e.enabled<Position>());
}

void componenttraits_Cleanup_traits_01(void) {
    flecs::world world;
    flecs::entity parent = world.entity();
    flecs::entity e = world.entity();
    struct MyComponent {
      flecs::entity e; // Not covered by cleanup traits
    };

    e.add(flecs::ChildOf, parent); // Covered by cleanup traits

        test_assert(true);
}

void componenttraits_derive_Component_01(void) {
    flecs::world world;
    flecs::entity Archer = world.entity();
    world.remove_all(Archer);

        test_assert(true);
}

void componenttraits_derive_Component_02(void) {
    flecs::world world;
    flecs::entity Archer = world.entity();
    world.remove_all(Archer);
    world.remove_all(Archer, flecs::Wildcard);
    world.remove_all(flecs::Wildcard, Archer);

        test_assert(true);
}

void componenttraits_OnDelete_Remove_01(void) {
    flecs::world world;
    struct Archer {};
    // Remove Archer from entities when Archer is deleted
    world.component<Archer>()
      .add(flecs::OnDelete, flecs::Remove);

    auto e = world.entity().add<Archer>();

    // This will remove Archer from e
    world.component<Archer>().destruct();

        test_assert(true);
}

void componenttraits_OnDelete_Delete_01(void) {
    flecs::world world;
    struct Archer {};
    // Delete entities with Archer when Archer is deleted
    world.component<Archer>()
      .add(flecs::OnDelete, flecs::Delete);

    auto e = world.entity().add<Archer>();

    // This will delete e
    world.component<Archer>().destruct();

        test_assert(true);
}

void componenttraits_OnDeleteTarget_Delete_01(void) {
    flecs::world world;
    struct ChildOf {};
    // Delete children when deleting parent
    world.component<ChildOf>()
      .add(flecs::OnDeleteTarget, flecs::Delete);

    auto p = world.entity();
    auto e = world.entity().add<ChildOf>(p);

    // This will delete both p and e
    p.destruct();

        test_assert(true);
}

void componenttraits_Cleanup_order_01(void) {
    flecs::world world;
    struct Node {};

    world.observer<Node>()
      .event(flecs::OnRemove)
      .each([](flecs::entity e, Node n) { });

    flecs::entity p = world.entity().add<Node>();
    flecs::entity c = world.entity().add<Node>().child_of(p);

        test_assert(true);
}

void componenttraits_DontFragment_trait_01(void) {
    flecs::world world;
    world.component<Position>().add(flecs::DontFragment);

        test_assert(true);
}

void componenttraits_Exclusive_trait_01(void) {
    flecs::world world;
    flecs::entity parent_a = world.entity();
    flecs::entity parent_b = world.entity();
    flecs::entity e = world.entity();
    e.child_of(parent_a);
    e.child_of(parent_b); // replaces (ChildOf, parent_a)

        test_assert(true);
}

void componenttraits_Exclusive_trait_02(void) {
    flecs::world world;
    flecs::entity MarriedTo = world.entity()
      .add(flecs::Exclusive);

        test_assert(true);
}

void componenttraits_Final_trait_01(void) {
    flecs::world ecs;
    auto e = ecs.entity()
      .add(flecs::Final);

    auto i = ecs.entity()
      .is_a(e); // not allowed

        test_assert(true);
}

void componenttraits_Inheritable_trait_01(void) {
    flecs::world world;
    struct Unit {};
    struct Warrior {};
    world.component<Unit>().add(flecs::Inheritable);

    auto q = world.query_builder()
      .with<Unit>()
      .build();

    world.component<Warrior>().is_a<Unit>();

    q.each([](flecs::entity unit) {
        // ...
    });

        test_assert(true);
}

void componenttraits_OneOf_trait_01(void) {
    flecs::world world;
    // Enforce that target of relationship is child of Food
    auto Food = world.entity().add(flecs::OneOf);
    auto Apples = world.entity().child_of(Food);
    auto Fork = world.entity();

    // This is ok, Apples is a child of Food
    auto a = world.entity().add(Food, Apples);

    // This is not ok, Fork is not a child of Food
    auto b = world.entity().add(Food, Fork);

        test_assert(true);
}

void componenttraits_OneOf_trait_02(void) {
    flecs::world world;
    // Enforce that target of relationship is child of Food
    auto Food = world.entity();
    auto Eats = world.entity().add(flecs::OneOf, Food);
    auto Apples = world.entity().child_of(Food);
    auto Fork = world.entity();

    // This is ok, Apples is a child of Food
    auto a = world.entity().add(Eats, Apples);

    // This is not ok, Fork is not a child of Food
    auto b = world.entity().add(Eats, Fork);

        test_assert(true);
}

void componenttraits_Override_01(void) {
    flecs::world ecs;
    // Register component with trait. Optional, since this is the default behavior.
    ecs.component<Mass>().add(flecs::OnInstantiate, flecs::Override);

    flecs::entity base = ecs.entity().set(Mass { 100 });
    flecs::entity inst = ecs.entity().is_a(base); // Mass is copied to inst

    test_assert(inst.owns<Mass>());
    test_assert(base.try_get<Mass>() != inst.try_get<Mass>());
}

void componenttraits_Inherit_01(void) {
    flecs::world ecs;
    // Register component with trait
    ecs.component<Mass>().add(flecs::OnInstantiate, flecs::Inherit);

    ecs_entity_t base = ecs.entity().set(Mass, { 100 });
    ecs_entity_t inst = ecs.entity().is_a(base);

    test_assert(inst.has<Mass>());
    test_assert(!inst.owns<Mass>());
    test_assert(base.try_get<Mass>() != inst.try_get<Mass>());
}

void componenttraits_DontInherit_01(void) {
    flecs::world ecs;
    // Register component with trait
    ecs.component<Mass>().add(flecs::OnInstantiate, flecs::DontInherit);

    ecs_entity_t base = ecs.entity().set(Mass, { 100 });
    ecs_entity_t inst = ecs.entity().is_a(base);

    test_assert(!inst.has<Mass>());
    test_assert(!inst.owns<Mass>());
    test_assert(inst.try_get<Mass>() == nullptr);
}

void componenttraits_OrderedChildren_trait_01(void) {
    flecs::world world;
    flecs::entity parent = world.entity().add(flecs::OrderedChildren);

    flecs::entity child_1 = world.entity().child_of(parent);
    flecs::entity child_2 = world.entity().child_of(parent);
    flecs::entity child_3 = world.entity().child_of(parent);

    // Adding/removing components usually changes the order in which children are
    // iterated, but with the OrderedChildren trait order is preserved.
    child_2.set(Position{10, 20});

    parent.children([](flecs::entity child) {
        // 1st result: child_1
        // 2nd result: child_2
        // 3rd result: child_3
    });

        test_assert(true);
}

void componenttraits_PairIsTag_trait_01(void) {
    flecs::world ecs;
    struct Serializable { }; // Tag, contains no data

    struct Position {
      float x, y;
    };

    auto e = ecs.entity()
      .set<Position>({10, 20})
      .add<Serializable, Position>(); // Because Serializable is a tag, the pair
                                      // has a value of type Position

    // Gets value from Position component
    const Position& p = e.get<Position>();

    // Gets (unintended) value from (Serializable, Position) pair
    const Position& p = e.get<Serializable, Position>();

        test_assert(true);
}

void componenttraits_derive_Component_01(void) {
    flecs::world ecs;
    struct Serializable { };
    // Ensure that Serializable never contains data
    ecs.component<Serializable>()
      .add(flecs::PairIsTag);

    auto e = ecs.entity()
      .set<Position>({10, 20})
      .add<Serializable, Position>(); // Because Serializable marked as a Tag, no
                                      // data is added for the pair even though
                                      // Position is a component

    // Gets value from Position component
    const Position& p = e.get<Position>();

    // This no longer works, the pair has no data
    const Position& p = e.get<Serializable, Position>();

        test_assert(true);
}

void componenttraits_Relationship_trait_01(void) {
    flecs::world world;
    struct Likes { };
    struct Apples { };

    world.component<Likes>().add(flecs::Relationship);

    flecs::entity e = world.entity()
      .add<Likes>()          // Panic, 'Likes' is not used as relationship
      .add<Apples, Likes>()  // Panic, 'Likes' is not used as relationship
      .add<Likes, Apples>(); // OK

        test_assert(true);
}

void componenttraits_derive_Component_01(void) {
    flecs::world world;
    struct Likes { };
    struct Loves { };

    world.component<Likes>().add(flecs::Relationship);

    // Even though Likes is marked as relationship and used as target here, this 
    // won't panic as With is marked as trait.
    world.component<Loves>().add(flecs::With, world.component<Likes>());

        test_assert(true);
}

void componenttraits_Singleton_trait_01(void) {
    flecs::world world;
    struct TimeOfDay { int hour; };
    world.component<TimeOfDay>().add(flecs::Singleton);

    world.set(TimeOfDay{0});

    // This is the same as adding TimeOfDay to itself:
    world.component<TimeOfDay>().set(TimeOfDay{0});

        test_assert(true);
}

void componenttraits_Singleton_trait_02(void) {
    flecs::world world;
    struct TimeOfDay { int hour; };
    world.component<TimeOfDay>().add(flecs::Singleton);

    // Automatically matches TimeOfDay as singleton
    auto q = world.query<Position, Velocity, const TimeOfDay>();

    // Is the same as
    auto q = world.query_builder<Position, Velocity, const TimeOfDay>()
      .term_at(2).src<TimeOfDay>()
      .build();

        test_assert(true);
}

void componenttraits_Sparse_trait_01(void) {
    flecs::world world;
    world.component<Position>().add(flecs::Sparse);

        test_assert(true);
}

void componenttraits_Symmetric_trait_01(void) {
    flecs::world ecs;
    auto MarriedTo = world.entity().add(flecs::Symmetric);
    auto Bob = ecs.entity();
    auto Alice = ecs.entity();
    Bob.add(MarriedTo, Alice); // Also adds (MarriedTo, Bob) to Alice

        test_assert(true);
}

void componenttraits_Target_trait_01(void) {
    flecs::world world;
    struct Likes { };
    struct Apples { };

    world.component<Apples>().add(flecs::Target);

    flecs::entity e = world.entity()
      .add<Apples>()         // Panic, 'Apples' is not used as target
      .add<Apples, Likes>()  // Panic, 'Apples' is not used as target
      .add<Likes, Apples>(); // OK

        test_assert(true);
}

void componenttraits_Trait_trait_01(void) {
    flecs::world world;
    struct Serializable { };

    world.component<Serializable>().add(flecs::Trait);

        test_assert(true);
}

void componenttraits_Transitive_trait_01(void) {
    flecs::world world;
    auto LocatedIn = world.entity();
    auto Manhattan = world.entity();
    auto NewYork = world.entity();
    auto USA = world.entity();

    Manhattan.add(LocatedIn, NewYork);
    NewYork.add(LocatedIn, USA);

        test_assert(true);
}

void componenttraits_Transitive_trait_02(void) {
    flecs::world world;
    flecs::entity LocatedIn = world.entity();
    LocatedIn.add(flecs::Transitive);

        test_assert(true);
}

void componenttraits_With_trait_01(void) {
    flecs::world world;
    auto Responsibility = world.entity();
    auto Power = world.entity().add(flecs::With, Responsibility);

    // Create new entity that has both Power and Responsibility
    auto e = world.entity().add(Power);

        test_assert(true);
}

void componenttraits_With_trait_02(void) {
    flecs::world world;
    auto Likes = world.entity();
    auto Loves = world.entity().add(flecs::With, Likes);
    auto Pears = world.entity();

    // Create new entity with both (Loves, Pears) and (Likes, Pears)
    auto e = world.entity().add(Loves, Pears);

        test_assert(true);
}
