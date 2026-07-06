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

struct TraitSparse {
    static constexpr bool sparse = true;
    float x, y;
};

struct TraitSparseSpecialized {
    float x, y;
};

template <>
struct flecs::sparse<TraitSparseSpecialized> : std::true_type { };

struct TraitSparseInherit {
    static constexpr bool sparse = true;
    static constexpr auto on_instantiate = flecs::on_instantiate::inherit;
    float x, y;
};

void ComponentTraits_sparse_explicit(void) {
    flecs::world world;

    auto c = world.component<TraitSparse>();
    test_assert(c.has(flecs::Sparse));
    test_assert(!c.has(flecs::DontFragment));
    test_assert(!c.has(flecs::OnInstantiate, flecs::Wildcard));
}

void ComponentTraits_sparse_implicit(void) {
    flecs::world world;

    auto e = world.entity().set<TraitSparse>({10, 20});
    test_assert(e.has<TraitSparse>());

    flecs::entity c(world, world.id<TraitSparse>());
    test_assert(c.has(flecs::Sparse));
    test_assert(!c.has(flecs::DontFragment));
}

void ComponentTraits_sparse_specialized(void) {
    flecs::world world;

    auto c = world.component<TraitSparseSpecialized>();
    test_assert(c.has(flecs::Sparse));
    test_assert(!c.has(flecs::DontFragment));
}

void ComponentTraits_sparse_get_get_mut(void) {
    flecs::world world;

    auto e = world.entity().set<TraitSparse>({10, 20});

    const TraitSparse& v = e.get<TraitSparse>();
    test_int(v.x, 10);
    test_int(v.y, 20);

    TraitSparse& mv = e.get_mut<TraitSparse>();
    mv.x = 30;
    mv.y = 40;

    const TraitSparse *p = e.try_get<TraitSparse>();
    test_assert(p != nullptr);
    test_int(p->x, 30);
    test_int(p->y, 40);
}

void ComponentTraits_sparse_try_get_not_found(void) {
    flecs::world world;

    world.component<TraitSparse>();

    auto e = world.entity();
    test_assert(e.try_get<TraitSparse>() == nullptr);
    test_assert(e.try_get_mut<TraitSparse>() == nullptr);
}

void ComponentTraits_dont_fragment_get_get_mut(void) {
    flecs::world world;

    auto e = world.entity().set<TraitDontFragment>({10, 20});

    const TraitDontFragment& v = e.get<TraitDontFragment>();
    test_int(v.x, 10);
    test_int(v.y, 20);

    TraitDontFragment& mv = e.get_mut<TraitDontFragment>();
    mv.x = 30;
    mv.y = 40;

    const TraitDontFragment *p = e.try_get<TraitDontFragment>();
    test_assert(p != nullptr);
    test_int(p->x, 30);
    test_int(p->y, 40);

    test_assert(world.entity().try_get<TraitDontFragment>() == nullptr);
}

void ComponentTraits_on_instantiate_override_get_get_mut(void) {
    flecs::world world;

    auto e = world.entity().set<TraitOnInstantiateOverride>({10, 20});

    const TraitOnInstantiateOverride& v = e.get<TraitOnInstantiateOverride>();
    test_int(v.x, 10);
    test_int(v.y, 20);

    TraitOnInstantiateOverride& mv = e.get_mut<TraitOnInstantiateOverride>();
    mv.x = 30;
    mv.y = 40;

    const TraitOnInstantiateOverride *p = e.try_get<TraitOnInstantiateOverride>();
    test_assert(p != nullptr);
    test_int(p->x, 30);
    test_int(p->y, 40);

    test_assert(world.entity().try_get<TraitOnInstantiateOverride>() == nullptr);
}

void ComponentTraits_on_instantiate_dont_inherit_get_get_mut(void) {
    flecs::world world;

    auto e = world.entity().set<TraitOnInstantiateDontInherit>({10, 20});

    const TraitOnInstantiateDontInherit& v = 
        e.get<TraitOnInstantiateDontInherit>();
    test_int(v.x, 10);
    test_int(v.y, 20);

    TraitOnInstantiateDontInherit& mv = 
        e.get_mut<TraitOnInstantiateDontInherit>();
    mv.x = 30;
    mv.y = 40;

    const TraitOnInstantiateDontInherit *p = 
        e.try_get<TraitOnInstantiateDontInherit>();
    test_assert(p != nullptr);
    test_int(p->x, 30);
    test_int(p->y, 40);
}

void ComponentTraits_on_instantiate_inherit_get_inherited(void) {
    flecs::world world;

    auto base = world.prefab().set<TraitOnInstantiateInherit>({10, 20});
    auto inst = world.entity().is_a(base);

    const TraitOnInstantiateInherit *p = 
        inst.try_get<TraitOnInstantiateInherit>();
    test_assert(p != nullptr);
    test_int(p->x, 10);
    test_int(p->y, 20);

    test_assert(inst.try_get_mut<TraitOnInstantiateInherit>() == nullptr);
}

void ComponentTraits_no_traits_get_inherited(void) {
    flecs::world world;

    world.component<TraitNone>().add(flecs::OnInstantiate, flecs::Inherit);

    auto base = world.prefab().set<TraitNone>({10, 20});
    auto inst = world.entity().is_a(base);

    const TraitNone *p = inst.try_get<TraitNone>();
    test_assert(p != nullptr);
    test_int(p->x, 10);
    test_int(p->y, 20);

    test_assert(inst.try_get_mut<TraitNone>() == nullptr);
}

void ComponentTraits_get_dispatch(void) {
    test_assert((flecs::_::is_sparse_component<TraitSparse>::value));
    test_assert((flecs::_::is_sparse_component<TraitSparseSpecialized>::value));
    test_assert((flecs::_::is_sparse_component<TraitDontFragment>::value));
    test_assert((!flecs::_::is_sparse_component<TraitNone>::value));

    test_assert((flecs::_::is_get_sparse_component<TraitSparse>::value));
    test_assert((flecs::_::is_get_sparse_component<TraitDontFragment>::value));
    test_assert((!flecs::_::is_get_sparse_component<TraitSparseInherit>::value));
    test_assert((!flecs::_::is_get_sparse_component<TraitNone>::value));
    test_assert((!flecs::_::is_get_sparse_component<TraitOnInstantiateOverride>::value));
}

void ComponentTraits_dynamic_inherit_dense_owned(void) {
    flecs::world world;

    world.component<TraitNone>().add(flecs::OnInstantiate, flecs::Inherit);

    auto e = world.entity().set<TraitNone>({10, 20});

    const TraitNone *p = e.try_get<TraitNone>();
    test_assert(p != nullptr);
    test_int(p->x, 10);
    test_int(p->y, 20);

    TraitNone *mp = e.try_get_mut<TraitNone>();
    test_assert(mp != nullptr);
    mp->x = 30;
    test_int(e.try_get<TraitNone>()->x, 30);
}

void ComponentTraits_dynamic_inherit_dense_inherited(void) {
    flecs::world world;

    world.component<TraitNone>().add(flecs::OnInstantiate, flecs::Inherit);

    auto base = world.prefab().set<TraitNone>({10, 20});
    auto inst = world.entity().is_a(base);

    const TraitNone *p = inst.try_get<TraitNone>();
    test_assert(p != nullptr);
    test_int(p->x, 10);
    test_int(p->y, 20);

    test_assert(inst.try_get_mut<TraitNone>() == nullptr);
}

void ComponentTraits_dynamic_inherit_sparse_owned(void) {
    install_test_abort();
    flecs::world world;

    world.component<TraitSparse>().add(flecs::OnInstantiate, flecs::Inherit);

    auto e = world.entity().set<TraitSparse>({10, 20});

    test_expect_abort();
    e.try_get<TraitSparse>();
}

void ComponentTraits_dynamic_inherit_sparse_owned_get_mut(void) {
    install_test_abort();
    flecs::world world;

    world.component<TraitSparse>().add(flecs::OnInstantiate, flecs::Inherit);

    auto e = world.entity().set<TraitSparse>({10, 20});

    test_expect_abort();
    e.try_get_mut<TraitSparse>();
}

void ComponentTraits_dynamic_inherit_sparse_inherited(void) {
    install_test_abort();
    flecs::world world;

    world.component<TraitSparse>().add(flecs::OnInstantiate, flecs::Inherit);

    auto base = world.prefab().set<TraitSparse>({10, 20});
    auto inst = world.entity().is_a(base);

    test_expect_abort();
    inst.try_get<TraitSparse>();
}

void ComponentTraits_dynamic_inherit_dont_fragment_owned(void) {
    install_test_abort();
    flecs::world world;

    world.component<TraitDontFragment>().add(
        flecs::OnInstantiate, flecs::Inherit);

    auto e = world.entity().set<TraitDontFragment>({10, 20});

    test_expect_abort();
    e.try_get<TraitDontFragment>();
}

void ComponentTraits_dynamic_inherit_dont_fragment_owned_get_mut(void) {
    install_test_abort();
    flecs::world world;

    world.component<TraitDontFragment>().add(
        flecs::OnInstantiate, flecs::Inherit);

    auto e = world.entity().set<TraitDontFragment>({10, 20});

    test_expect_abort();
    e.try_get_mut<TraitDontFragment>();
}

void ComponentTraits_dynamic_inherit_dont_fragment_inherited(void) {
    install_test_abort();
    flecs::world world;

    world.component<TraitDontFragment>().add(
        flecs::OnInstantiate, flecs::Inherit);

    auto base = world.prefab().set<TraitDontFragment>({10, 20});
    auto inst = world.entity().is_a(base);

    test_expect_abort();
    inst.try_get<TraitDontFragment>();
}

struct TraitDontFragmentInherit {
    static constexpr bool dont_fragment = true;
    static constexpr auto on_instantiate = flecs::on_instantiate::inherit;
    float x, y;
};

void ComponentTraits_static_inherit_dense_owned(void) {
    flecs::world world;

    auto c = world.component<TraitOnInstantiateInherit>();
    test_assert(c.has(flecs::OnInstantiate, flecs::Inherit));

    auto e = world.entity().set<TraitOnInstantiateInherit>({10, 20});

    const TraitOnInstantiateInherit *p = e.try_get<TraitOnInstantiateInherit>();
    test_assert(p != nullptr);
    test_int(p->x, 10);
    test_int(p->y, 20);

    TraitOnInstantiateInherit *mp = e.try_get_mut<TraitOnInstantiateInherit>();
    test_assert(mp != nullptr);
    mp->x = 30;
    test_int(e.try_get<TraitOnInstantiateInherit>()->x, 30);
}

void ComponentTraits_static_inherit_dense_inherited(void) {
    flecs::world world;

    auto base = world.prefab().set<TraitOnInstantiateInherit>({10, 20});
    auto inst = world.entity().is_a(base);

    const TraitOnInstantiateInherit *p = 
        inst.try_get<TraitOnInstantiateInherit>();
    test_assert(p != nullptr);
    test_int(p->x, 10);
    test_int(p->y, 20);

    test_assert(inst.try_get_mut<TraitOnInstantiateInherit>() == nullptr);
}

void ComponentTraits_static_inherit_sparse_owned(void) {
    flecs::world world;

    auto c = world.component<TraitSparseInherit>();
    test_assert(c.has(flecs::Sparse));
    test_assert(c.has(flecs::OnInstantiate, flecs::Inherit));

    auto e = world.entity().set<TraitSparseInherit>({10, 20});

    const TraitSparseInherit *p = e.try_get<TraitSparseInherit>();
    test_assert(p != nullptr);
    test_int(p->x, 10);
    test_int(p->y, 20);

    TraitSparseInherit *mp = e.try_get_mut<TraitSparseInherit>();
    test_assert(mp != nullptr);
    mp->x = 30;
    test_int(e.try_get<TraitSparseInherit>()->x, 30);
}

void ComponentTraits_static_inherit_sparse_inherited(void) {
    flecs::world world;

    auto base = world.prefab().set<TraitSparseInherit>({10, 20});
    auto inst = world.entity().is_a(base);

    const TraitSparseInherit *p = inst.try_get<TraitSparseInherit>();
    test_assert(p != nullptr);
    test_int(p->x, 10);
    test_int(p->y, 20);

    test_assert(inst.try_get_mut<TraitSparseInherit>() == nullptr);
}

void ComponentTraits_static_inherit_dont_fragment_owned(void) {
    flecs::world world;

    auto c = world.component<TraitDontFragmentInherit>();
    test_assert(c.has(flecs::DontFragment));
    test_assert(c.has(flecs::OnInstantiate, flecs::Inherit));

    auto e = world.entity().set<TraitDontFragmentInherit>({10, 20});

    const TraitDontFragmentInherit *p = e.try_get<TraitDontFragmentInherit>();
    test_assert(p != nullptr);
    test_int(p->x, 10);
    test_int(p->y, 20);

    TraitDontFragmentInherit *mp = e.try_get_mut<TraitDontFragmentInherit>();
    test_assert(mp != nullptr);
    mp->x = 30;
    test_int(e.try_get<TraitDontFragmentInherit>()->x, 30);
}

void ComponentTraits_static_inherit_dont_fragment_inherited(void) {
    flecs::world world;

    auto base = world.prefab().set<TraitDontFragmentInherit>({10, 20});
    auto inst = world.entity().is_a(base);

    const TraitDontFragmentInherit *p = 
        inst.try_get<TraitDontFragmentInherit>();
    test_assert(p != nullptr);
    test_int(p->x, 10);
    test_int(p->y, 20);

    test_assert(inst.try_get_mut<TraitDontFragmentInherit>() == nullptr);
}
