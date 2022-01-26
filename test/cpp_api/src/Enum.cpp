#include <cpp_api.h>

enum StandardEnum {
    Red, Green, Blue
};

enum SparseEnum {
    Black = 1, White = 3, Grey = 5
};

enum class EnumClass {
    Grass, Sand, Stone
};

/* Optional, but improves compile time */
FLECS_ENUM_LAST(StandardEnum, Blue)
FLECS_ENUM_LAST(SparseEnum, Grey)
FLECS_ENUM_LAST(EnumClass, EnumClass::Stone)

void Enum_standard_enum_reflection() {
    flecs::world ecs;

    auto enum_type = flecs::enum_type<StandardEnum>(ecs);

    auto e = enum_type.entity();
    test_assert(e != 0);
    test_assert(e == ecs.component<StandardEnum>());
    test_str(e.path().c_str(), "::StandardEnum");
    test_int(enum_type.first(), Red);
    test_int(enum_type.last(), Blue);

    auto e_red = enum_type.entity(Red);
    auto e_green = enum_type.entity(Green);
    auto e_blue = enum_type.entity(Blue);

    test_assert(e_red != 0);
    test_str(e_red.path().c_str(), "::StandardEnum::Red");
    test_bool(enum_type.is_valid(Red), true);
    test_assert(e_red.get<StandardEnum>() != nullptr);
    test_assert(e_red.get<StandardEnum>()[0] == Red);

    test_assert(e_green != 0);
    test_str(e_green.path().c_str(), "::StandardEnum::Green");
    test_bool(enum_type.is_valid(Green), true);
    test_assert(e_green.get<StandardEnum>() != nullptr);
    test_assert(e_green.get<StandardEnum>()[0] == Green);

    test_assert(e_blue != 0);
    test_str(e_blue.path().c_str(), "::StandardEnum::Blue");
    test_bool(enum_type.is_valid(Blue), true);
    test_assert(e_blue.get<StandardEnum>() != nullptr);
    test_assert(e_blue.get<StandardEnum>()[0] == Blue);

    test_bool(enum_type.is_valid(Blue + 1), false);
}

void Enum_sparse_enum_reflection() {
    flecs::world ecs;

    auto enum_type = flecs::enum_type<SparseEnum>(ecs);

    auto e = enum_type.entity();
    test_assert(e != 0);
    test_assert(e == ecs.component<SparseEnum>());
    test_str(e.path().c_str(), "::SparseEnum");
    test_int(enum_type.first(), Black);
    test_int(enum_type.last(), Grey);

    auto e_black = enum_type.entity(Black);
    auto e_white = enum_type.entity(White);
    auto e_grey = enum_type.entity(Grey);

    test_assert(e_black != 0);
    test_str(e_black.path().c_str(), "::SparseEnum::Black");
    test_bool(enum_type.is_valid(Black), true);
    test_assert(e_black.get<SparseEnum>() != nullptr);
    test_assert(e_black.get<SparseEnum>()[0] == Black);

    test_assert(e_white != 0);
    test_str(e_white.path().c_str(), "::SparseEnum::White");
    test_bool(enum_type.is_valid(White), true);
    test_assert(e_white.get<SparseEnum>() != nullptr);
    test_assert(e_white.get<SparseEnum>()[0] == White);

    test_assert(e_grey != 0);
    test_str(e_grey.path().c_str(), "::SparseEnum::Grey");
    test_bool(enum_type.is_valid(Grey), true);
    test_assert(e_grey.get<SparseEnum>() != nullptr);
    test_assert(e_grey.get<SparseEnum>()[0] == Grey);

    test_bool(enum_type.is_valid(0), false);
    test_bool(enum_type.is_valid(2), false);
    test_bool(enum_type.is_valid(4), false);
    test_bool(enum_type.is_valid(6), false);
}

void Enum_enum_class_reflection() {
    flecs::world ecs;

    auto enum_type = flecs::enum_type<EnumClass>(ecs);

    auto e = enum_type.entity();
    test_assert(e != 0);
    test_assert(e == ecs.component<EnumClass>());
    test_str(e.path().c_str(), "::EnumClass");
    test_int(enum_type.first(), (int)EnumClass::Grass);
    test_int(enum_type.last(), (int)EnumClass::Stone);

    auto e_grass = enum_type.entity(EnumClass::Grass);
    auto e_sand = enum_type.entity(EnumClass::Sand);
    auto e_stone = enum_type.entity(EnumClass::Stone);

    test_assert(e_grass != 0);
    test_str(e_grass.path().c_str(), "::EnumClass::Grass");
    test_bool(enum_type.is_valid((int)EnumClass::Grass), true);
    test_assert(e_grass.get<EnumClass>() != nullptr);
    test_assert(e_grass.get<EnumClass>()[0] == EnumClass::Grass);

    test_assert(e_sand != 0);
    test_str(e_sand.path().c_str(), "::EnumClass::Sand");
    test_bool(enum_type.is_valid((int)EnumClass::Sand), true);
    test_assert(e_sand.get<EnumClass>() != nullptr);
    test_assert(e_sand.get<EnumClass>()[0] == EnumClass::Sand);

    test_assert(e_stone != 0);
    test_str(e_stone.path().c_str(), "::EnumClass::Stone");
    test_bool(enum_type.is_valid((int)EnumClass::Stone), true);
    test_assert(e_stone.get<EnumClass>() != nullptr);
    test_assert(e_stone.get<EnumClass>()[0] == EnumClass::Stone);

    test_bool(enum_type.is_valid(3), false);
}

void Enum_add_enum_constant() {
    flecs::world ecs;

    auto e = ecs.entity().add(StandardEnum::Red);
    test_str(e.type().str().c_str(), "(StandardEnum,StandardEnum.Red)");

    flecs::id id = e.type().get(0);
    test_assert(id.is_pair());

    auto r = id.relation();
    test_assert(r == ecs.component<StandardEnum>());

    auto c = r.lookup("Red");
    test_assert(r != 0);

    test_assert(id == ecs.pair(r, c));
}

void Enum_replace_enum_constants() {
    flecs::world ecs;

    auto e = ecs.entity().add(StandardEnum::Red);
    test_assert(e.has(StandardEnum::Red));
    test_assert(!e.has(StandardEnum::Green));
    test_assert(!e.has(StandardEnum::Blue));

    e.add(StandardEnum::Green);
    test_assert(!e.has(StandardEnum::Red));
    test_assert(e.has(StandardEnum::Green));
    test_assert(!e.has(StandardEnum::Blue));

    e.add(StandardEnum::Blue);
    test_assert(!e.has(StandardEnum::Red));
    test_assert(!e.has(StandardEnum::Green));
    test_assert(e.has(StandardEnum::Blue));
}

void Enum_has_enum() {
    flecs::world ecs;

    auto e = ecs.entity();
    test_assert(!e.has<StandardEnum>());
    
    e.add(StandardEnum::Red);

    test_assert(e.has<StandardEnum>());
    test_assert(e.has(StandardEnum::Red));
    test_assert(!e.has(StandardEnum::Green));
    test_assert(!e.has(StandardEnum::Blue));

    auto r = e.type().get(0).relation();
    test_assert(r != 0);
    test_assert(r == ecs.component<StandardEnum>());

    auto c = r.lookup("Red");
    test_assert(c != 0);
    test_assert(e.has<StandardEnum>(c));
}

void Enum_has_enum_wildcard() {
    flecs::world ecs;

    auto e = ecs.entity();
    test_assert(!e.has<StandardEnum>(flecs::Wildcard));
    
    e.add(StandardEnum::Green);

    test_assert(e.has<StandardEnum>(flecs::Wildcard));
}

void Enum_get_enum() {
    flecs::world ecs;

    auto e = ecs.entity().add(StandardEnum::Red);
    test_assert(e.has(StandardEnum::Red));

    const StandardEnum *v = e.get<StandardEnum>();
    test_assert(v != NULL);
    test_assert(*v == StandardEnum::Red);

    e.add(StandardEnum::Green);
    test_assert(e.has(StandardEnum::Green));

    v = e.get<StandardEnum>();
    test_assert(v != NULL);
    test_assert(*v == StandardEnum::Green);
}

void Enum_remove_enum() {
    flecs::world ecs;

    auto e = ecs.entity().add(StandardEnum::Green);
    test_assert(e.has(StandardEnum::Green));

    e.remove<StandardEnum>();
    test_assert(!e.has(StandardEnum::Green));
}

void Enum_remove_wildcard() {
    flecs::world ecs;

    auto e = ecs.entity().add(StandardEnum::Green);
    test_assert(e.has(StandardEnum::Green));

    e.remove<StandardEnum>(flecs::Wildcard);
    test_assert(!e.has(StandardEnum::Green));
}

void Enum_enum_as_component() {
    flecs::world ecs;

    auto e = ecs.entity();

    e.set<StandardEnum>({StandardEnum::Green});
    test_assert(e.has<StandardEnum>());

    const StandardEnum *v = e.get<StandardEnum>();
    test_assert(v != NULL);
    test_assert(v[0] == StandardEnum::Green);

    flecs::id id = e.type().get(0);
    test_bool(id.is_pair(), false);
    test_assert(id == ecs.component<StandardEnum>());
}
