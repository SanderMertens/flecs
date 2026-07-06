#include <cpp.h>

struct TraitDontFragment {
    static constexpr bool dont_fragment = true;
    float x, y;
};

struct TraitOnInstantiateOverride {
    static constexpr auto on_instantiate = flecs::on_instantiate::override;
    float x, y;
};

struct TraitOnInstantiateInherit {
    static constexpr auto on_instantiate = flecs::on_instantiate::inherit;
    float x, y;
};

struct TraitOnInstantiateDontInherit {
    static constexpr auto on_instantiate = flecs::on_instantiate::dont_inherit;
    float x, y;
};

struct TraitOnInstantiateSpecialized {
    float x, y;
};

template <>
struct flecs::on_instantiate_trait<TraitOnInstantiateSpecialized> {
    static constexpr bool declared = true;
    static constexpr flecs::on_instantiate value =
        flecs::on_instantiate::inherit;
};

struct TraitDontFragmentDontInherit {
    static constexpr bool dont_fragment = true;
    static constexpr auto on_instantiate = flecs::on_instantiate::dont_inherit;
    float x, y;
};

struct TraitNone {
    float x, y;
};

void ComponentTraits_dont_fragment_explicit(void) {
    flecs::world world;

    auto c = world.component<TraitDontFragment>();
    test_assert(c.has(flecs::DontFragment));
    test_assert(!c.has(flecs::OnInstantiate, flecs::Wildcard));
}

void ComponentTraits_dont_fragment_implicit(void) {
    flecs::world world;

    auto e = world.entity().set<TraitDontFragment>({10, 20});
    test_assert(e.has<TraitDontFragment>());

    flecs::entity c(world, world.id<TraitDontFragment>());
    test_assert(c.has(flecs::DontFragment));
    test_assert(!c.has(flecs::OnInstantiate, flecs::Wildcard));
}

void ComponentTraits_on_instantiate_override_explicit(void) {
    flecs::world world;

    auto c = world.component<TraitOnInstantiateOverride>();
    test_assert(c.has(flecs::OnInstantiate, flecs::Override));
    test_assert(!c.has(flecs::DontFragment));
}

void ComponentTraits_on_instantiate_override_implicit(void) {
    flecs::world world;

    auto e = world.entity().set<TraitOnInstantiateOverride>({10, 20});
    test_assert(e.has<TraitOnInstantiateOverride>());

    flecs::entity c(world, world.id<TraitOnInstantiateOverride>());
    test_assert(c.has(flecs::OnInstantiate, flecs::Override));
    test_assert(!c.has(flecs::DontFragment));
}

void ComponentTraits_on_instantiate_inherit_explicit(void) {
    flecs::world world;

    auto c = world.component<TraitOnInstantiateInherit>();
    test_assert(c.has(flecs::OnInstantiate, flecs::Inherit));
    test_assert(!c.has(flecs::DontFragment));
}

void ComponentTraits_on_instantiate_inherit_implicit(void) {
    flecs::world world;

    auto e = world.entity().set<TraitOnInstantiateInherit>({10, 20});
    test_assert(e.has<TraitOnInstantiateInherit>());

    flecs::entity c(world, world.id<TraitOnInstantiateInherit>());
    test_assert(c.has(flecs::OnInstantiate, flecs::Inherit));
    test_assert(!c.has(flecs::DontFragment));
}

void ComponentTraits_on_instantiate_dont_inherit_explicit(void) {
    flecs::world world;

    auto c = world.component<TraitOnInstantiateDontInherit>();
    test_assert(c.has(flecs::OnInstantiate, flecs::DontInherit));
    test_assert(!c.has(flecs::DontFragment));
}

void ComponentTraits_on_instantiate_dont_inherit_implicit(void) {
    flecs::world world;

    auto e = world.entity().set<TraitOnInstantiateDontInherit>({10, 20});
    test_assert(e.has<TraitOnInstantiateDontInherit>());

    flecs::entity c(world, world.id<TraitOnInstantiateDontInherit>());
    test_assert(c.has(flecs::OnInstantiate, flecs::DontInherit));
    test_assert(!c.has(flecs::DontFragment));
}

void ComponentTraits_on_instantiate_specialized_explicit(void) {
    flecs::world world;

    auto c = world.component<TraitOnInstantiateSpecialized>();
    test_assert(c.has(flecs::OnInstantiate, flecs::Inherit));
    test_assert(!c.has(flecs::DontFragment));
}

void ComponentTraits_on_instantiate_specialized_implicit(void) {
    flecs::world world;

    auto e = world.entity().set<TraitOnInstantiateSpecialized>({10, 20});
    test_assert(e.has<TraitOnInstantiateSpecialized>());

    flecs::entity c(world, world.id<TraitOnInstantiateSpecialized>());
    test_assert(c.has(flecs::OnInstantiate, flecs::Inherit));
    test_assert(!c.has(flecs::DontFragment));
}

void ComponentTraits_dont_fragment_dont_inherit_explicit(void) {
    flecs::world world;

    auto c = world.component<TraitDontFragmentDontInherit>();
    test_assert(c.has(flecs::DontFragment));
    test_assert(c.has(flecs::OnInstantiate, flecs::DontInherit));
}

void ComponentTraits_dont_fragment_dont_inherit_implicit(void) {
    flecs::world world;

    auto e = world.entity().set<TraitDontFragmentDontInherit>({10, 20});
    test_assert(e.has<TraitDontFragmentDontInherit>());

    flecs::entity c(world, world.id<TraitDontFragmentDontInherit>());
    test_assert(c.has(flecs::DontFragment));
    test_assert(c.has(flecs::OnInstantiate, flecs::DontInherit));
}

void ComponentTraits_no_traits_explicit(void) {
    flecs::world world;

    auto c = world.component<TraitNone>();
    test_assert(!c.has(flecs::DontFragment));
    test_assert(!c.has(flecs::OnInstantiate, flecs::Wildcard));
}

void ComponentTraits_no_traits_implicit(void) {
    flecs::world world;

    auto e = world.entity().set<TraitNone>({10, 20});
    test_assert(e.has<TraitNone>());

    flecs::entity c(world, world.id<TraitNone>());
    test_assert(!c.has(flecs::DontFragment));
    test_assert(!c.has(flecs::OnInstantiate, flecs::Wildcard));
}
