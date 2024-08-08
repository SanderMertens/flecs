#include <cpp.h>

enum StandardEnum {
    Red, Green, Blue
};

enum AnotherEnum {
    Standing, Walking, Running
};

enum SparseEnum {
    Black = 1, White = 3, Grey = 5
};

enum BitMaskEnum {
    ZERO = 0,
    bit_LS_0,
    bit_LS_1 = bit_LS_0<<1,
    bit_LS_2 = bit_LS_0<<2,
    bit_LS_3 = bit_LS_0<<3,
    bit_LS_4 = bit_LS_0<<4,
    bit_LS_5 = bit_LS_0<<5,
    bit_LS_6 = bit_LS_0<<6,
    bit_LS_7 = bit_LS_0<<7,
    bit_LS_8 = bit_LS_0<<8,
    bit_LS_14 = bit_LS_0<<14,
    bit_LS_15 = bit_LS_0<<15,
    bit_LS_28 = bit_LS_0<<28,
    bit_LS_29 = bit_LS_0<<29,
    bit_LS_30 = bit_LS_0<<30,
    // bit_LS_31 = bit_LS_0<<31,
};

enum class TypedBitMaskEnum : uint32_t {
    ZERO = 0,
    bit_LS_0,
    bit_LS_1 = bit_LS_0<<1,
    bit_LS_2 = bit_LS_0<<2,
    bit_LS_3 = bit_LS_0<<3,
    bit_LS_4 = bit_LS_0<<4,
    bit_LS_5 = bit_LS_0<<5,
    bit_LS_6 = bit_LS_0<<6,
    bit_LS_7 = bit_LS_0<<7,
    bit_LS_8 = bit_LS_0<<8,
    bit_LS_9 = bit_LS_0<<9,
    bit_LS_10 = bit_LS_0<<10,
    bit_LS_11 = bit_LS_0<<11,
    bit_LS_12 = bit_LS_0<<12,
    bit_LS_13 = bit_LS_0<<13,
    bit_LS_14 = bit_LS_0<<14,
    bit_LS_15 = bit_LS_0<<15,
    bit_LS_31 = bit_LS_0<<31,
};

enum MixedConstantBitmaskEnum {
    Vim,
    Emacs,
    Nano,
    VsCode,
    Editor = 1 << 7,
    OperatingSystem = 1 << 8,
    Terminal = 1 << 9,
    GUI = 1 << 30,
    BAD_BEEF = -0xBADBEEF
};

enum class EnumClass {
    Grass, Sand, Stone
};

enum PrefixEnum {
    PrefixEnumFoo, PrefixEnumBar
};

enum ConstantsWithNum {
    Num1,
    Num2,
    Num3,
};

enum class EnumIncorrectType : uint8_t {
    A, B
};

enum EnumWithLargeConstant {
    X, Y, Z = 1000
};

enum class EnumClassWithLargeConstant {
    X, Y, Z = 1000
};

/* Optional, but improves compile time */
FLECS_ENUM_LAST(StandardEnum, Blue)
FLECS_ENUM_LAST(SparseEnum, Grey)
FLECS_ENUM_LAST(EnumClass, EnumClass::Stone)

void Enum_standard_enum_reflection(void) {
    flecs::world ecs;

    auto enum_type = flecs::enum_type<StandardEnum>(ecs);
    test_int(enum_type.impl_.constants_size, 3);

    auto e = enum_type.entity();
    test_assert(e != 0);
    test_assert(e == ecs.component<StandardEnum>());
    test_str(e.path().c_str(), "::StandardEnum");
    test_int(enum_type.first(), Red);
    test_int(enum_type.last(), Blue);

    test_int(enum_type.index_by_value(Red), 0);
    test_int(enum_type.index_by_value(Green), 1);
    test_int(enum_type.index_by_value(Blue), 2);
    test_int(enum_type.index_by_value(Blue+1), -1);

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

void Enum_sparse_enum_reflection(void) {
    flecs::world ecs;

    auto enum_type = flecs::enum_type<SparseEnum>(ecs);
    test_int(enum_type.impl_.constants_size, 3);

    auto e = enum_type.entity();
    test_assert(e != 0);
    test_assert(e == ecs.component<SparseEnum>());
    test_str(e.path().c_str(), "::SparseEnum");
    test_int(enum_type.first(), 0);
    test_int(enum_type.last(), 2);

    test_int(enum_type.index_by_value(Black), 0);
    test_int(enum_type.index_by_value(White), 1);
    test_int(enum_type.index_by_value(Grey), 2);
    test_int(enum_type.index_by_value(Grey+1), -1);

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

void Enum_bitmask_enum_reflection(void) {
    flecs::world ecs;

    auto enum_type = flecs::enum_type<BitMaskEnum>(ecs);
    test_int(enum_type.impl_.constants_size, 15);

    auto e = enum_type.entity();
    test_assert(e != 0);
    test_assert(e == ecs.component<BitMaskEnum>());
    test_str(e.path().c_str(), "::BitMaskEnum");
    test_int(enum_type.first(), 0);
    test_int(enum_type.last(), 14);

    test_int(enum_type.index_by_value(ZERO), 0);
    test_int(enum_type.index_by_value(bit_LS_0), 1);
    test_int(enum_type.index_by_value(bit_LS_1), 2);
    test_int(enum_type.index_by_value(2), 2);
    test_int(enum_type.index_by_value(4), 3);
    test_int(enum_type.index_by_value(bit_LS_30), 14);
    test_int(enum_type.index_by_value(7), -1);

    auto e_8 = enum_type.entity(8);
    auto e_16 = enum_type.entity(bit_LS_4);
    auto e_32 = enum_type.entity(0x20);
    auto e_ls_30 = enum_type.entity(bit_LS_30);

    test_assert(e_8 != 0);
    test_str(e_8.path().c_str(), "::BitMaskEnum::bit_LS_3");
    test_bool(enum_type.is_valid(bit_LS_3), true);
    test_assert(e_8.get<BitMaskEnum>() != nullptr);
    test_assert(e_8.get<BitMaskEnum>()[0] == bit_LS_3);

    test_assert(e_16 != 0);
    test_str(e_16.path().c_str(), "::BitMaskEnum::bit_LS_4");
    test_bool(enum_type.is_valid(bit_LS_4), true);
    test_assert(e_16.get<BitMaskEnum>() != nullptr);
    test_assert(e_16.get<BitMaskEnum>()[0] == bit_LS_4);

    test_assert(e_32 != 0);
    test_str(e_32.path().c_str(), "::BitMaskEnum::bit_LS_5");
    test_bool(enum_type.is_valid(bit_LS_5), true);
    test_assert(e_32.get<BitMaskEnum>() != nullptr);
    test_assert(e_32.get<BitMaskEnum>()[0] == bit_LS_5);

    test_assert(e_ls_30 != 0);
    test_str(e_ls_30.path().c_str(), "::BitMaskEnum::bit_LS_30");
    test_bool(enum_type.is_valid(bit_LS_30), true);
    test_assert(e_ls_30.get<BitMaskEnum>() != nullptr);
    test_assert(e_ls_30.get<BitMaskEnum>()[0] == bit_LS_30);

    test_bool(enum_type.is_valid(3), false);
    test_bool(enum_type.is_valid(5), false);
    test_bool(enum_type.is_valid((1 << 31) + 1), false);
}

void Enum_bitmask_enum_with_type_reflection(void) {
    flecs::world ecs;

    auto enum_type = flecs::enum_type<TypedBitMaskEnum>(ecs);
    test_int(enum_type.impl_.constants_size, 18);

    auto e = enum_type.entity();
    test_assert(e != 0);
    test_assert(e == ecs.component<TypedBitMaskEnum>());
    test_str(e.path().c_str(), "::TypedBitMaskEnum");
    test_int(enum_type.first(), 0);
    test_int(enum_type.last(), 17);

    test_int(enum_type.index_by_value(TypedBitMaskEnum::ZERO), 0);
    test_int(enum_type.index_by_value(TypedBitMaskEnum::bit_LS_0), 1);
    test_int(enum_type.index_by_value(TypedBitMaskEnum::bit_LS_1), 2);
    test_int(enum_type.index_by_value(2), 2);
    test_int(enum_type.index_by_value(4), 3);
    test_int(enum_type.index_by_value(7), -1);

    auto e_8 = enum_type.entity(8);
    auto e_16 = enum_type.entity(TypedBitMaskEnum::bit_LS_4);
    auto e_32 = enum_type.entity(0x20);
    auto e_ls_15 = enum_type.entity(TypedBitMaskEnum::bit_LS_15);
    auto e_ls_31 = enum_type.entity(TypedBitMaskEnum::bit_LS_31);

    test_assert(e_8 != 0);
    test_str(e_8.path().c_str(), "::TypedBitMaskEnum::bit_LS_3");
    test_bool(enum_type.is_valid(TypedBitMaskEnum::bit_LS_3), true);
    test_assert(e_8.get<TypedBitMaskEnum>() != nullptr);
    test_assert(e_8.get<TypedBitMaskEnum>()[0] == TypedBitMaskEnum::bit_LS_3);

    test_assert(e_16 != 0);
    test_str(e_16.path().c_str(), "::TypedBitMaskEnum::bit_LS_4");
    test_bool(enum_type.is_valid(TypedBitMaskEnum::bit_LS_4), true);
    test_assert(e_16.get<TypedBitMaskEnum>() != nullptr);
    test_assert(e_16.get<TypedBitMaskEnum>()[0] == TypedBitMaskEnum::bit_LS_4);

    test_assert(e_32 != 0);
    test_str(e_32.path().c_str(), "::TypedBitMaskEnum::bit_LS_5");
    test_bool(enum_type.is_valid(TypedBitMaskEnum::bit_LS_5), true);
    test_assert(e_32.get<TypedBitMaskEnum>() != nullptr);
    test_assert(e_32.get<TypedBitMaskEnum>()[0] == TypedBitMaskEnum::bit_LS_5);

    test_assert(e_ls_15 != 0);
    test_str(e_ls_15.path().c_str(), "::TypedBitMaskEnum::bit_LS_15");
    test_bool(enum_type.is_valid(TypedBitMaskEnum::bit_LS_15), true);
    test_assert(e_ls_15.get<TypedBitMaskEnum>() != nullptr);
    test_assert(e_ls_15.get<TypedBitMaskEnum>()[0] == TypedBitMaskEnum::bit_LS_15);

    test_assert(e_ls_31 != 0);
    test_str(e_ls_31.path().c_str(), "::TypedBitMaskEnum::bit_LS_31");
    test_bool(enum_type.is_valid(TypedBitMaskEnum::bit_LS_31), true);
    test_assert(e_ls_31.get<TypedBitMaskEnum>() != nullptr);
    test_assert(e_ls_31.get<TypedBitMaskEnum>()[0] == TypedBitMaskEnum::bit_LS_31);

    test_bool(enum_type.is_valid(3), false);
    test_bool(enum_type.is_valid(5), false);
    test_bool(enum_type.is_valid(6), false);
}

void Enum_enum_with_mixed_constants_and_bitmask(void) {
    flecs::world ecs;
    auto ec = ecs.component<MixedConstantBitmaskEnum>()
        .constant("BAD_BEEF", BAD_BEEF);

    auto bad_beef_e = ec.lookup("BAD_BEEF");
    test_assert(bad_beef_e != 0);

    flecs::entity vim = ecs.entity().add(Vim);
    test_assert(vim.has(Vim));

    flecs::entity vs_code = ecs.entity().add(MixedConstantBitmaskEnum::VsCode);
    test_assert(vs_code.has(VsCode));

    flecs::entity terminal = ecs.entity().add(MixedConstantBitmaskEnum::Terminal);
    test_assert(terminal.has(Terminal));

    flecs::entity gui = ecs.entity().add(MixedConstantBitmaskEnum::GUI);
    test_assert(gui.has(GUI));

    vim.add(Editor);
    test_assert(vim.has(Editor));
    test_assert(!vim.has(Vim));

    flecs::entity bad_beef = ecs.entity().add(MixedConstantBitmaskEnum::BAD_BEEF);
    test_assert(bad_beef.has(BAD_BEEF));
    bad_beef.add(Nano);
    test_assert(bad_beef.has(Nano));
    test_assert(!bad_beef.has(BAD_BEEF));
}

void Enum_enum_class_reflection(void) {
    flecs::world ecs;

    auto enum_type = flecs::enum_type<EnumClass>(ecs);
    test_int(enum_type.impl_.constants_size, 3);

    auto e = enum_type.entity();
    test_assert(e != 0);
    test_assert(e == ecs.component<EnumClass>());
    test_str(e.path().c_str(), "::EnumClass");
    test_int(enum_type.first(), (int)EnumClass::Grass);
    test_int(enum_type.last(), (int)EnumClass::Stone);

    test_int(enum_type.index_by_value(EnumClass::Grass), 0);
    test_int(enum_type.index_by_value(EnumClass::Sand), 1);
    test_int(enum_type.index_by_value(EnumClass::Stone), 2);
    test_int(enum_type.index_by_value((int)EnumClass::Stone + 1), -1);

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

void Enum_prefixed_enum_reflection(void) {
    flecs::world ecs;

    auto enum_type = flecs::enum_type<PrefixEnum>(ecs);
    test_int(enum_type.impl_.constants_size, 2);

    auto e = enum_type.entity();
    test_assert(e != 0);
    test_assert(e == ecs.component<PrefixEnum>());
    test_str(e.path().c_str(), "::PrefixEnum");
    test_int(enum_type.first(), PrefixEnum::PrefixEnumFoo);
    test_int(enum_type.last(), PrefixEnum::PrefixEnumBar);

    test_int(enum_type.index_by_value(PrefixEnum::PrefixEnumFoo), 0);
    test_int(enum_type.index_by_value(PrefixEnum::PrefixEnumBar), 1);

    auto e_foo = enum_type.entity(PrefixEnum::PrefixEnumFoo);
    auto e_bar = enum_type.entity(PrefixEnum::PrefixEnumBar);

    test_assert(e_foo != 0);
    test_str(e_foo.path().c_str(), "::PrefixEnum::Foo");
    test_bool(enum_type.is_valid(PrefixEnum::PrefixEnumFoo), true);
    test_assert(e_foo.get<PrefixEnum>() != nullptr);
    test_assert(e_foo.get<PrefixEnum>()[0] == PrefixEnum::PrefixEnumFoo);

    test_assert(e_bar != 0);
    test_str(e_bar.path().c_str(), "::PrefixEnum::Bar");
    test_bool(enum_type.is_valid(PrefixEnum::PrefixEnumBar), true);
    test_assert(e_bar.get<PrefixEnum>() != nullptr);
    test_assert(e_bar.get<PrefixEnum>()[0] == PrefixEnum::PrefixEnumBar);

    test_bool(enum_type.is_valid(PrefixEnum::PrefixEnumBar + 1), false);
}

void Enum_constant_with_num_reflection(void) {
    flecs::world ecs;

    auto enum_type = flecs::enum_type<ConstantsWithNum>(ecs);
    test_int(enum_type.impl_.constants_size, 3);

    auto e = enum_type.entity();
    test_assert(e != 0);
    test_assert(e == ecs.component<ConstantsWithNum>());
    test_str(e.path().c_str(), "::ConstantsWithNum");
    test_int(enum_type.first(), ConstantsWithNum::Num1);
    test_int(enum_type.last(), ConstantsWithNum::Num3);

    test_int(enum_type.index_by_value(ConstantsWithNum::Num1), 0);
    test_int(enum_type.index_by_value(ConstantsWithNum::Num2), 1);
    test_int(enum_type.index_by_value(ConstantsWithNum::Num3), 2);

    auto num_1 = enum_type.entity(ConstantsWithNum::Num1);
    auto num_2 = enum_type.entity(ConstantsWithNum::Num2);
    auto num_3 = enum_type.entity(ConstantsWithNum::Num3);

    test_assert(num_1 != 0);
    test_str(num_1.path().c_str(), "::ConstantsWithNum::Num1");
    test_bool(enum_type.is_valid(ConstantsWithNum::Num1), true);
    test_assert(num_1.get<ConstantsWithNum>() != nullptr);
    test_assert(num_1.get<ConstantsWithNum>()[0] == ConstantsWithNum::Num1);

    test_assert(num_2 != 0);
    test_str(num_2.path().c_str(), "::ConstantsWithNum::Num2");
    test_bool(enum_type.is_valid(ConstantsWithNum::Num1), true);
    test_assert(num_2.get<ConstantsWithNum>() != nullptr);
    test_assert(num_2.get<ConstantsWithNum>()[0] == ConstantsWithNum::Num2);

    test_assert(num_3 != 0);
    test_str(num_3.path().c_str(), "::ConstantsWithNum::Num3");
    test_bool(enum_type.is_valid(ConstantsWithNum::Num1), true);
    test_assert(num_3.get<ConstantsWithNum>() != nullptr);
    test_assert(num_3.get<ConstantsWithNum>()[0] == ConstantsWithNum::Num3);

    test_bool(enum_type.is_valid(ConstantsWithNum::Num3 + 1), false);
}

void Enum_get_constant_id(void) {
    flecs::world ecs;

    flecs::entity red = ecs.component<StandardEnum>().lookup("Red");
    const StandardEnum *v = red.get<StandardEnum>();
    test_assert(v != NULL);
    test_assert(v[0] == StandardEnum::Red);
    test_assert(red == ecs.id(StandardEnum::Red));
    test_str("Red", ecs.entity(StandardEnum::Red).name());

    auto e = flecs::enum_type<StandardEnum>(ecs);
    test_assert(e.entity(StandardEnum::Red) == red);
}

void Enum_add_enum_constant(void) {
    flecs::world ecs;

    auto e = ecs.entity().add(StandardEnum::Red);
    test_str(e.type().str().c_str(), "(StandardEnum,StandardEnum.Red)");

    flecs::id id = e.type().get(0);
    test_assert(id.is_pair());

    auto r = id.first();
    test_assert(r == ecs.component<StandardEnum>());

    auto c = r.lookup("Red");
    test_assert(r != 0);

    test_assert(id == ecs.pair(r, c));
}

void Enum_add_enum_class_constant(void) {
    flecs::world ecs;

    auto e = ecs.entity().add(EnumClass::Sand);
    test_str(e.type().str().c_str(), "(EnumClass,EnumClass.Sand)");

    flecs::id id = e.type().get(0);
    test_assert(id.is_pair());

    auto r = id.first();
    test_assert(r == ecs.component<EnumClass>());

    auto c = r.lookup("Sand");
    test_assert(r != 0);

    test_assert(id == ecs.pair(r, c));
}

void Enum_replace_enum_constants(void) {
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

void Enum_has_enum(void) {
    flecs::world ecs;

    auto e = ecs.entity();
    test_assert(!e.has<StandardEnum>());
    
    e.add(StandardEnum::Red);

    test_assert(e.has<StandardEnum>());
    test_assert(e.has(StandardEnum::Red));
    test_assert(!e.has(StandardEnum::Green));
    test_assert(!e.has(StandardEnum::Blue));

    auto r = e.type().get(0).first();
    test_assert(r != 0);
    test_assert(r == ecs.component<StandardEnum>());

    auto c = r.lookup("Red");
    test_assert(c != 0);
    test_assert(e.has<StandardEnum>(c));
}

void Enum_has_enum_wildcard(void) {
    flecs::world ecs;

    auto e = ecs.entity();
    test_assert(!e.has<StandardEnum>(flecs::Wildcard));
    
    e.add(StandardEnum::Green);

    test_assert(e.has<StandardEnum>(flecs::Wildcard));
}

void Enum_get_enum(void) {
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

void Enum_remove_enum(void) {
    flecs::world ecs;

    auto e = ecs.entity().add(StandardEnum::Green);
    test_assert(e.has(StandardEnum::Green));

    e.remove<StandardEnum>();
    test_assert(!e.has(StandardEnum::Green));
}

void Enum_remove_wildcard(void) {
    flecs::world ecs;

    auto e = ecs.entity().add(StandardEnum::Green);
    test_assert(e.has(StandardEnum::Green));

    e.remove<StandardEnum>(flecs::Wildcard);
    test_assert(!e.has(StandardEnum::Green));
}

void Enum_enum_as_component(void) {
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

void Enum_query_enum_wildcard(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().add(StandardEnum::Red);
    auto e2 = ecs.entity().add(StandardEnum::Green);
    auto e3 = ecs.entity().add(StandardEnum::Blue);

    auto q = ecs.query_builder()
        .with<StandardEnum>(flecs::Wildcard)
        .build();

    int32_t count = 0;
    q.each([&](flecs::iter& it, size_t index) {
        if (it.entity(index) == e1) {
            test_assert(it.pair(0).second() == ecs.id(StandardEnum::Red));
            count ++;
        }
        if (it.entity(index) == e2) {
            test_assert(it.pair(0).second() == ecs.id(StandardEnum::Green));
            count ++;
        }
        if (it.entity(index) == e3) {
            test_assert(it.pair(0).second() == ecs.id(StandardEnum::Blue));
            count ++;
        }
    });

    test_int(count, 3);
}

void Enum_query_enum_constant(void) {
    flecs::world ecs;

    ecs.entity().add(StandardEnum::Red);
    ecs.entity().add(StandardEnum::Green);
    auto e1 = ecs.entity().add(StandardEnum::Blue);

    auto q = ecs.query_builder()
        .with<StandardEnum>(StandardEnum::Blue)
        .build();

    int32_t count = 0;
    q.each([&](flecs::iter& it, size_t index) {
        test_assert(it.entity(index) == e1);
        test_assert(it.pair(0).second() == ecs.id(StandardEnum::Blue));
        count ++;
    });

    test_int(count, 1);
}

void Enum_enum_type_from_stage(void) {
    flecs::world ecs;

    auto stage = ecs.get_stage(0);

    ecs.readonly_begin();

    auto enum_type = flecs::enum_type<StandardEnum>(stage);
    test_assert(enum_type.entity() == ecs.component<StandardEnum>());

    ecs.readonly_end();
}

void Enum_add_enum_from_stage(void) {
    flecs::world ecs;

    auto stage = ecs.get_stage(0);

    ecs.readonly_begin();

    auto e = stage.entity().add(StandardEnum::Red);
    test_assert(!e.has(StandardEnum::Red));

    ecs.readonly_end();

    test_assert(e.has(StandardEnum::Red));
}

void Enum_enum_w_2_worlds(void) {
    {
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
    {
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
}

struct MyTag { };

void Enum_add_enum_constant_w_tag(void) {
    flecs::world ecs;

    flecs::entity e1 = ecs.entity()
        .add<MyTag>(Red);
    flecs::entity e2 = ecs.entity()
        .add<MyTag>(Green);
    flecs::entity e3 = ecs.entity()
        .add<MyTag>(Blue);

    test_assert(e1.has<MyTag>(Red));
    test_assert(e2.has<MyTag>(Green));
    test_assert(e3.has<MyTag>(Blue));

    auto enum_type = flecs::enum_type<StandardEnum>(ecs);
    auto e_red = enum_type.entity(Red);
    auto e_green = enum_type.entity(Green);
    auto e_blue = enum_type.entity(Blue);

    auto t1 = e1.target<MyTag>();
    auto t2 = e2.target<MyTag>();
    auto t3 = e3.target<MyTag>();

    test_assert(t1 == e_red);
    test_assert(t2 == e_green);
    test_assert(t3 == e_blue);
}

void Enum_remove_enum_constant_w_tag(void) {
    flecs::world ecs;

    flecs::entity e1 = ecs.entity()
        .add<MyTag>(Red);
    flecs::entity e2 = ecs.entity()
        .add<MyTag>(Green);
    flecs::entity e3 = ecs.entity()
        .add<MyTag>(Blue);

    test_assert(e1.has<MyTag>(Red));
    test_assert(e2.has<MyTag>(Green));
    test_assert(e3.has<MyTag>(Blue));

    e1.remove<MyTag>(Green);
    e1.remove<MyTag>(Blue);
    test_assert(e1.has<MyTag>(Red));
    e1.remove<MyTag>(Red);
    test_assert(!e1.has<MyTag>(Red));

    e2.remove<MyTag>(Red);
    e2.remove<MyTag>(Blue);
    test_assert(e2.has<MyTag>(Green));
    e2.remove<MyTag>(Green);
    test_assert(!e2.has<MyTag>(Green));

    e3.remove<MyTag>(Red);
    e3.remove<MyTag>(Green);
    test_assert(e3.has<MyTag>(Blue));
    e3.remove<MyTag>(Blue);
    test_assert(!e3.has<MyTag>(Blue));
}

void Enum_set_enum_constant_w_tag(void) {
    flecs::world ecs;

    flecs::entity e1 = ecs.entity()
        .set<Position>(Red, {1, 2})
        .set<Position>(Green, {2, 3})
        .set<Position>(Blue, {3, 4});

    test_assert(e1.has<Position>(Red));
    test_assert(e1.has<Position>(Green));
    test_assert(e1.has<Position>(Blue));

    const Position *p = e1.get<Position>(Red);
    test_assert(p != NULL);
    test_int(p->x, 1);
    test_int(p->y, 2);
    
    p = e1.get<Position>(Green);
    test_assert(p != NULL);
    test_int(p->x, 2);
    test_int(p->y, 3);

    p = e1.get<Position>(Blue);
    test_assert(p != NULL);
    test_int(p->x, 3);
    test_int(p->y, 4);
}

void Enum_enum_w_incorrect_size(void) {
    /* Quaratined as test can cause compilation of test suite to fail due to new
     * error messages introduced in clang. */
    test_quarantine("6 Aug 2023");
    // install_test_abort();

    // flecs::world ecs;

    // test_expect_abort();
    // ecs.component<EnumIncorrectType>();
}

void Enum_add_union_enum(void) {
    flecs::world ecs;

    ecs.component<StandardEnum>().add(flecs::Union);

    auto t_color = flecs::enum_type<StandardEnum>(ecs);
    auto red = t_color.entity(StandardEnum::Red);
    auto blue = t_color.entity(StandardEnum::Blue);

    auto e1 = ecs.entity().add(StandardEnum::Red);
    auto e2 = ecs.entity().add(StandardEnum::Blue);

    test_assert(e1.type() == e2.type());
    test_assert(e1.target<StandardEnum>() == red);
    test_assert(e2.target<StandardEnum>() == blue);
    test_assert(e1.has(StandardEnum::Red));
    test_assert(e2.has(StandardEnum::Blue));
}

void Enum_add_2_union_enums(void) {
    flecs::world ecs;

    ecs.component<StandardEnum>().add(flecs::Union);
    ecs.component<AnotherEnum>().add(flecs::Union);

    auto e = ecs.entity();
    e.add(StandardEnum::Red);
    e.add(AnotherEnum::Running);

    test_assert(e.has(StandardEnum::Red));
    test_assert(e.has(AnotherEnum::Running));
    test_assert(e.target<StandardEnum>() != 0);
    test_assert(e.target<AnotherEnum>() != 0);

    auto t_color = flecs::enum_type<StandardEnum>(ecs);
    auto t_AnotherEnum = flecs::enum_type<AnotherEnum>(ecs);
    auto red = t_color.entity(StandardEnum::Red);
    auto running = t_AnotherEnum.entity(AnotherEnum::Running);

    test_assert(e.target<StandardEnum>() == red);
    test_assert(e.target<AnotherEnum>() == running);
}

void Enum_add_2_union_enums_reverse(void) {
    flecs::world ecs;

    ecs.component<StandardEnum>().add(flecs::Union);
    ecs.component<AnotherEnum>().add(flecs::Union);

    auto e = ecs.entity();
    e.add(AnotherEnum::Running);
    e.add(StandardEnum::Red);

    test_assert(e.has(StandardEnum::Red));
    test_assert(e.has(AnotherEnum::Running));
    test_assert(e.target<StandardEnum>() != 0);
    test_assert(e.target<AnotherEnum>() != 0);

    auto t_color = flecs::enum_type<StandardEnum>(ecs);
    auto t_AnotherEnum = flecs::enum_type<AnotherEnum>(ecs);
    auto red = t_color.entity(StandardEnum::Red);
    auto running = t_AnotherEnum.entity(AnotherEnum::Running);

    test_assert(e.target<StandardEnum>() == red);
    test_assert(e.target<AnotherEnum>() == running);
}

void Enum_constant_from_entity(void) {
    flecs::world ecs;

    flecs::entity e_red = ecs.to_entity(StandardEnum::Red);
    test_assert(e_red != 0);

    flecs::entity e_green = ecs.to_entity(StandardEnum::Green);
    test_assert(e_green != 0);

    flecs::entity e_blue = ecs.to_entity(StandardEnum::Blue);
    test_assert(e_blue != 0);

    test_assert(e_red.to_constant<StandardEnum>() == StandardEnum::Red);
    test_assert(e_green.to_constant<StandardEnum>() == StandardEnum::Green);
    test_assert(e_blue.to_constant<StandardEnum>() == StandardEnum::Blue);
}

void Enum_add_if(void) {
    flecs::world ecs;

    auto e = ecs.entity();

    e.add_if(true, StandardEnum::Red);
    test_assert(e.has(StandardEnum::Red));
    test_assert(e.has<StandardEnum>(ecs.to_entity(StandardEnum::Red)));

    e.add_if(false, StandardEnum::Red);
    test_assert(!e.has(StandardEnum::Red));
    test_assert(!e.has<StandardEnum>(ecs.to_entity(StandardEnum::Red)));
}

void Enum_add_if_other(void) {
    flecs::world ecs;

    auto e = ecs.entity();

    e.add(StandardEnum::Red);
    test_assert(e.has(StandardEnum::Red));
    test_assert(e.has<StandardEnum>(ecs.to_entity(StandardEnum::Red)));

    e.add_if(false, StandardEnum::Blue);
    test_assert(!e.has(StandardEnum::Blue));
    test_assert(!e.has(StandardEnum::Red));
    test_assert(!e.has<StandardEnum>(ecs.to_entity(StandardEnum::Red)));
}

void Enum_query_union_enum(void) {
    flecs::world ecs;

    enum Color {
        Red,
        Green,
        Blue
    };

    ecs.component<StandardEnum>().add(flecs::Union);

    flecs::entity e1 = ecs.entity().add(StandardEnum::Red);
    flecs::entity e2 = ecs.entity().add(StandardEnum::Green);
    flecs::entity e3 = ecs.entity().add(StandardEnum::Blue);

    auto q = ecs.query_builder()
        .with<StandardEnum>().second(flecs::Wildcard)
        .build();

    int32_t count = 0;
    q.run([&](flecs::iter& it) {
        while (it.next()) {
            test_int(it.count(), 1);
            if (count == 0) {
                test_int(it.entity(0), e1);
                test_assert(it.id(0) == 
                    ecs.pair<StandardEnum>(ecs.to_entity(StandardEnum::Red)));
            }
            if (count == 2) {
                test_int(it.entity(0), e2);
                test_assert(it.id(0) == 
                    ecs.pair<StandardEnum>(ecs.to_entity(StandardEnum::Green)));
            }
            if (count == 1) {
                test_int(it.entity(0), e3);
                test_assert(it.id(0) == 
                    ecs.pair<StandardEnum>(ecs.to_entity(StandardEnum::Blue)));
            }
            count ++;
        }
    });

    test_int(count, 3);
}

void Enum_query_union_enum_invalid_query_type(void) {
    install_test_abort();

    flecs::world ecs;

    ecs.component<StandardEnum>().add(flecs::Union);

    test_expect_abort();

    ecs.query_builder<StandardEnum>()
        .term_at(0).second(flecs::Wildcard)
        .build();
}

void Enum_component_registered_as_enum(void) {
    flecs::world ecs;

    auto e = ecs.component<StandardEnum>();

    test_assert(e.has<flecs::Enum>());

    const flecs::Type *mt = e.get<flecs::Type>();

    test_assert(mt != nullptr);
    test_assert(mt->kind == flecs::meta::EnumType);

    {
        auto c = e.lookup("Red");
        test_assert(c != 0);

        const StandardEnum *v = c.get<StandardEnum>();
        test_assert(v != nullptr);
        test_assert(*v == StandardEnum::Red);

        const int32_t *vi = c.get_second<int32_t>(flecs::Constant);
        test_assert(vi != nullptr);
        test_int(*vi, StandardEnum::Red);
    }

    {
        auto c = e.lookup("Green");
        test_assert(c != 0);

        const StandardEnum *v = c.get<StandardEnum>();
        test_assert(v != nullptr);
        test_assert(*v == StandardEnum::Green);

        const int32_t *vi = c.get_second<int32_t>(flecs::Constant);
        test_assert(vi != nullptr);
        test_int(*vi, StandardEnum::Green);
    }

    {
        auto c = e.lookup("Blue");
        test_assert(c != 0);

        const StandardEnum *v = c.get<StandardEnum>();
        test_assert(v != nullptr);
        test_assert(*v == StandardEnum::Blue);

        const int32_t *vi = c.get_second<int32_t>(flecs::Constant);
        test_assert(vi != nullptr);
        test_int(*vi, StandardEnum::Blue);
    }
}

void Enum_mixed_auto_manual_constants(void) {
    flecs::world ecs;

    auto e = ecs.component<EnumWithLargeConstant>()
        .constant("Z", EnumWithLargeConstant::Z);

    test_assert(e.has<flecs::Enum>());

    const flecs::Type *mt = e.get<flecs::Type>();
    test_assert(mt != nullptr);
    test_assert(mt->kind == flecs::meta::EnumType);

    {
        auto c = e.lookup("X");
        test_assert(c != 0);

        const EnumWithLargeConstant *v = c.get<EnumWithLargeConstant>();
        test_assert(v != nullptr);
        test_assert(*v == EnumWithLargeConstant::X);

        const int32_t *vi = c.get_second<int32_t>(flecs::Constant);
        test_assert(vi != nullptr);
        test_int(*vi, EnumWithLargeConstant::X);
    }

    {
        auto c = e.lookup("Y");
        test_assert(c != 0);

        const EnumWithLargeConstant *v = c.get<EnumWithLargeConstant>();
        test_assert(v != nullptr);
        test_assert(*v == EnumWithLargeConstant::Y);

        const int32_t *vi = c.get_second<int32_t>(flecs::Constant);
        test_assert(vi != nullptr);
        test_int(*vi, EnumWithLargeConstant::Y);
    }

    {
        auto c = e.lookup("Z");
        test_assert(c != 0);

        const EnumWithLargeConstant *v = c.get<EnumWithLargeConstant>();
        test_assert(v != nullptr);
        test_assert(*v == EnumWithLargeConstant::Z);

        const int32_t *vi = c.get_second<int32_t>(flecs::Constant);
        test_assert(vi != nullptr);
        test_int(*vi, EnumWithLargeConstant::Z);
    }
}

void Enum_enum_class_mixed_auto_manual_constants(void) {
    flecs::world ecs;

    auto e = ecs.component<EnumClassWithLargeConstant>()
        .constant("Z", EnumClassWithLargeConstant::Z);

    test_assert(e.has<flecs::Enum>());

    const flecs::Type *mt = e.get<flecs::Type>();
    test_assert(mt != nullptr);
    test_assert(mt->kind == flecs::meta::EnumType);

    {
        auto c = e.lookup("X");
        test_assert(c != 0);

        const EnumClassWithLargeConstant *v = c.get<EnumClassWithLargeConstant>();
        test_assert(v != nullptr);
        test_assert(*v == EnumClassWithLargeConstant::X);

        const int32_t *vi = c.get_second<int32_t>(flecs::Constant);
        test_assert(vi != nullptr);
        test_assert(*vi == static_cast<int32_t>(EnumClassWithLargeConstant::X));
    }
}

void Enum_enum_child_count(void) {
    flecs::world ecs;

    flecs::entity e = ecs.component<StandardEnum>();

    flecs::query<> f = ecs.query_builder()
        .with(flecs::ChildOf, e)
        .build();

    test_assert(f.count() == 3);
}
