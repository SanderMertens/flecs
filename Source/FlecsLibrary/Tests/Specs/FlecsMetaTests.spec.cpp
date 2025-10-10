
#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS && defined(FLECS_TESTS)

#include "flecs.h"

#include "Bake/FlecsTestUtils.h"
#include "Bake/FlecsTestTypes.h"

struct FlecsTestToppings : flecs::bitmask {
    static constexpr uint32_t Bacon = 0x1;
    static constexpr uint32_t Lettuce = 0x2;
    static constexpr uint32_t Tomato = 0x4;
};

struct FlecsTestSandwich {
    uint32_t toppings;
};

struct FlecsTestInt {
    int32_t v;
};

struct TFlecsTestVector {
    std::vector<int> v;
};

struct TFlecsTestString {
    std::string v;
};

struct TFlecsTestVectorString {
    std::vector<int> v;
    std::string s;
};

struct TFlecsTestStringVector {
    std::string s;
    std::vector<int> v;
};

struct TFlecsTestStringString {
    std::string s1;
    std::string s2;
};

struct TFlecsTestVectorVector {
    std::vector<int> v1;
    std::vector<int> v2;
};

struct TFlecsTestVectorVectorString {
    std::vector<int> v1;
    std::vector<int> v2;
    std::string s;
};

struct TFlecsTestVectorStringVector {
    std::vector<int> v1;
    std::string s;
    std::vector<int> v2;
};

struct CppTypes {
    std::string s;
    std::vector<std::string> v;
};

enum class EnumWithBits : uint32_t {
    BitA = 0,
    BitB = 1 << 0,
    BitAll = 0xffffffff
};

struct EnumWithBitsStruct {
    EnumWithBits bits = EnumWithBits::BitAll;
};

struct CppEntity {
    flecs::entity entity;
};

BEGIN_DEFINE_SPEC(FFlecsMetaTestsSpec,
                  "FlecsLibrary.Meta",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

static flecs::opaque<std::string> std_string_support(flecs::world&) {
    flecs::opaque<std::string> ts;

    // Let reflection framework know what kind of type this is
    ts.as_type(flecs::String);

    // Forward std::string value to (JSON/...) serializer
    ts.serialize([](const flecs::serializer *s, const std::string *data) {
        const char *value = data->c_str();
        return s->value(flecs::String, &value);
    });

    // Serialize string into std::string
    ts.assign_string([](std::string *data, const char *value) {
        *data = value;
    });

    return ts;
}

template <typename T> 
static flecs::opaque<std::vector<T>, T> std_vector_support(flecs::world& world) {
    flecs::opaque<std::vector<T>, T> ts;

    // Let reflection framework know what kind of type this is
    ts.as_type(world.vector<T>());

    // Forward elements of std::vector value to (JSON/...) serializer
    ts.serialize([](const flecs::serializer *s, const std::vector<T> *data) {
        for (const auto& el : *data) {
            s->value(el);
        }
        return 0;
    });

    // Return vector count
    ts.count([](const std::vector<T> *data) {
        return data->size();
    });

    // Ensure element exists, return
    ts.ensure_element([](std::vector<T> *data, size_t elem) {
        if (data->size() <= elem) {
            data->resize(elem + 1);
        }

        return &data->data()[elem];
    });

    // Resize contents of vector
    ts.resize([](std::vector<T> *data, size_t size) {
        data->resize(size);
    });

    return ts;
}

void Meta_struct(void) {
    flecs::world ecs;

    struct Test {
        int32_t a;
        float b;
    };

    auto c = ecs.component<Test>()
        .member<int32_t>("a")
        .member<float>("b");    
    test_assert(c != 0);
    
    auto a = c.lookup("a");
    test_assert(a != 0);
    test_assert( a.has<flecs::Member>() );
    const flecs::Member *m = a.try_get<flecs::Member>();
    test_uint(m->type, flecs::I32);

    auto b = c.lookup("b");
    test_assert(b != 0);
    test_assert( b.has<flecs::Member>() );
    m = b.try_get<flecs::Member>();
    test_uint(m->type, flecs::F32);
}

void Meta_nested_struct(void) {
    flecs::world ecs;

    struct Test {
        int32_t x;
    };

    struct Nested {
        Test a;
    };

    auto t = ecs.component<Test>()
        .member<int32_t>("x");

    auto n = ecs.component<Nested>()
        .member(t, "a");
    test_assert(n != 0);
    
    auto a = n.lookup("a");
    test_assert(a != 0);
    test_assert( a.has<flecs::Member>() );
    const flecs::Member *m = a.try_get<flecs::Member>();
    test_uint(m->type, t);
}

void Meta_struct_w_portable_type(void) {
    flecs::world ecs;

    struct Test {
        uintptr_t a;
        uintptr_t b;
        flecs::entity_t c;
        flecs::entity_t d;
    };

    auto t = ecs.component<Test>()
        .member<uintptr_t>("a")
        .member(flecs::Uptr, "b")
        .member<flecs::entity_t>("c")
        .member(flecs::Entity, "d");
    test_assert(t != 0);

    auto a = t.lookup("a");
    test_assert(a != 0);
    test_assert( a.has<flecs::Member>() );
    const flecs::Member *m = a.try_get<flecs::Member>();
    test_uint(m->type, ecs.component<uintptr_t>());

    auto b = t.lookup("b");
    test_assert(b != 0);
    test_assert( b.has<flecs::Member>() );
    m = b.try_get<flecs::Member>();
    test_uint(m->type, flecs::Uptr);

    auto c = t.lookup("c");
    test_assert(c != 0);
    test_assert( c.has<flecs::Member>() );
    m = c.try_get<flecs::Member>();
    test_uint(m->type, flecs::U64);

    auto d = t.lookup("d");
    test_assert(d != 0);
    test_assert( d.has<flecs::Member>() );
    m = d.try_get<flecs::Member>();
    test_uint(m->type, flecs::Entity);
}

void Meta_units(void) {
    struct Test {
        int32_t meters;
        int32_t custom_unit;
    };

    flecs::world ecs;

    ecs.import<flecs::units>();

    auto custom_unit = ecs.entity("some_unit").unit("u");
    test_assert(custom_unit != 0);
    test_str(custom_unit.name(), "some_unit");

    const flecs::Unit *unit = custom_unit.try_get<flecs::Unit>();
    test_assert(unit != nullptr);
    test_str(unit->symbol, "u");

    auto t = ecs.component<Test>()
        .member<int32_t, flecs::units::length::Meters>("meters")
        .member<int32_t>(custom_unit, "custom_unit");
    test_assert(t != 0);

    flecs::entity m_meters = t.lookup("meters");
    test_assert(m_meters != 0);
    const flecs::Member *m = m_meters.try_get<flecs::Member>();
    test_assert(m != nullptr);
    test_uint(m->type, flecs::I32);
    test_uint(m->unit, ecs.id<flecs::units::length::Meters>());

    flecs::entity m_custom_unit = t.lookup("custom_unit");
    test_assert(m_custom_unit != 0);
    m = m_custom_unit.try_get<flecs::Member>();
    test_assert(m != nullptr);
    test_uint(m->type, flecs::I32);
    test_uint(m->unit, custom_unit);
}

void Meta_unit_w_quantity(void) {
    struct Test {
        int32_t m_1;
        int32_t m_2;
    };

    flecs::world ecs;

    ecs.import<flecs::units>();

    auto custom_quantity = ecs.entity().quantity();
    auto unit_1 = ecs.entity().unit("u1").quantity(custom_quantity);
    auto unit_2 = ecs.entity().unit("u2").quantity<flecs::units::Length>();

    test_assert(unit_1.has(flecs::Quantity, custom_quantity));
    test_assert(unit_2.has(flecs::Quantity, ecs.id<flecs::units::Length>()));

    auto t = ecs.component<Test>()
        .member<int32_t>(unit_1, "m_1")
        .member<int32_t>(unit_2, "m_2");
    test_assert(t != 0);

    flecs::entity m_1 = t.lookup("m_1");
    test_assert(m_1 != 0);
    const flecs::Member *m = m_1.try_get<flecs::Member>();
    test_assert(m != nullptr);
    test_uint(m->type, flecs::I32);
    test_uint(m->unit, unit_1);

    flecs::entity m_2 = t.lookup("m_2");
    test_assert(m_2 != 0);
    m = m_2.try_get<flecs::Member>();
    test_assert(m != nullptr);
    test_uint(m->type, flecs::I32);
    test_uint(m->unit, unit_2);
}

void Meta_unit_w_prefix(void) {
    struct Test {
        int32_t m_1;
        int32_t m_2;
    };

    flecs::world ecs;

    ecs.import<flecs::units>();

    auto prefix = ecs.entity().unit_prefix("p", 100, 1);
    
    auto unit_1 = ecs.entity().unit("U1");
    const flecs::Unit *unit = unit_1.try_get<flecs::Unit>();
    test_assert(unit != nullptr);
    test_str(unit->symbol, "U1");

    auto unit_2 = ecs.entity().unit(prefix, unit_1);
    unit = unit_2.try_get<flecs::Unit>();
    test_assert(unit != nullptr);
    test_str(unit->symbol, "pU1");
}

void Meta_unit_w_over(void) {
    struct Test {
        int32_t m_1;
        int32_t m_2;
    };

    flecs::world ecs;

    ecs.import<flecs::units>();

    auto prefix = ecs.entity().unit_prefix("p", 100, 1);
    
    auto unit_0 = ecs.entity().unit("U0");
    const flecs::Unit *unit = unit_0.try_get<flecs::Unit>();
    test_assert(unit != nullptr);
    test_str(unit->symbol, "U0");

    auto unit_1 = ecs.entity().unit("U1");
    unit = unit_1.try_get<flecs::Unit>();
    test_assert(unit != nullptr);
    test_str(unit->symbol, "U1");

    auto unit_2 = ecs.entity().unit(prefix, unit_1, unit_0);
    unit = unit_2.try_get<flecs::Unit>();
    test_assert(unit != nullptr);
    test_str(unit->symbol, "pU1/U0");
}

void Meta_partial_struct(void) {
    flecs::world ecs;

    auto c = ecs.component<Position>()
        .member<float>("x");
    test_assert(c != 0);

    const flecs::Component *ptr = c.try_get<flecs::Component>();
    test_int(ptr->size, 8);
    test_int(ptr->alignment, 4);
    
    auto xe = c.lookup("x");
    test_assert(xe != 0);
    test_assert( xe.has<flecs::Member>() );
    const flecs::Member *x = xe.try_get<flecs::Member>();
    test_uint(x->type, flecs::F32);
    test_uint(x->offset, 0);
}

void Meta_partial_struct_custom_offset(void) {
    flecs::world ecs;

    auto c = ecs.component<Position>()
        .member<float>("y", 1, offsetof(Position, y));
    test_assert(c != 0);

    const flecs::Component *ptr = c.try_get<flecs::Component>();
    test_int(ptr->size, 8);
    test_int(ptr->alignment, 4);
    
    auto xe = c.lookup("y");
    test_assert(xe != 0);
    test_assert( xe.has<flecs::Member>() );
    const flecs::Member *x = xe.try_get<flecs::Member>();
    test_uint(x->type, flecs::F32);
    test_uint(x->offset, 4);
}

void Meta_bitmask(void) {
    flecs::world ecs;

    // Register components with reflection data
    ecs.component<FlecsTestToppings>()
        .bit("Bacon", FlecsTestToppings::Bacon)
        .bit("Lettuce", FlecsTestToppings::Lettuce)
        .bit("Tomato", FlecsTestToppings::Tomato);

    ecs.component<FlecsTestSandwich>()
        .member<FlecsTestToppings>("toppings");

    // Create entity with Sandwich as usual
    auto e = ecs.entity()
        .set<FlecsTestSandwich>({FlecsTestToppings::Bacon | FlecsTestToppings::Lettuce});

    // Convert Sandwidth component to flecs expression string
    const FlecsTestSandwich *ptr = e.try_get<FlecsTestSandwich>();
    test_str(ecs.to_expr(ptr).c_str(), "{toppings: Lettuce|Bacon}");
}

static
int Int_serialize(const flecs::serializer *s, const FlecsTestInt *data) {
    return s->value(data->v);
}

static
int String_serialize(const flecs::serializer *s, const std::string *data) {
    const char *str = data->c_str();
    return s->value(flecs::String, &str);
}

template <typename Elem>
static int Vector_serialize(const flecs::serializer *ser, const std::vector<Elem> *data) {
    for (const auto& el : *data) {
        if (ser->value(el)) {
            return -1;
        }
    }

    return 0;
}

void Meta_custom_i32_to_json(void) {
    flecs::world ecs;

    ecs.component<FlecsTestInt>().opaque(flecs::I32)
        .serialize(Int_serialize);

    FlecsTestInt v = {10};
    flecs::string json = ecs.to_json(&v);
    test_str(json, "10");
}

void Meta_custom_std_string_to_json(void) {
    flecs::world ecs;

    ecs.component<std::string>().opaque(flecs::String)
        .serialize(String_serialize);

    std::string v = {"Hello World"};
    flecs::string json = ecs.to_json(&v);
    test_str(json, "\"Hello World\"");
}

void Meta_custom_std_vector_i32_to_json(void) {
    flecs::world ecs;

    ecs.component<std::vector<int>>().opaque(ecs.vector<int>())
        .serialize(Vector_serialize<int>);

    std::vector<int> v = {1, 2, 3};
    flecs::string json = ecs.to_json(&v);
    test_str(json, "[1, 2, 3]");
}

void Meta_custom_std_vector_std_string_to_json(void) {
    flecs::world ecs;

    ecs.component<std::string>().opaque(flecs::String)
        .serialize(String_serialize);

    ecs.component<std::vector<std::string>>().opaque(ecs.vector(flecs::String))
        .serialize(Vector_serialize<std::string>);

    std::vector<std::string> v = {"hello", "world", "foo"};
    flecs::string json = ecs.to_json(&v);
    test_str(json, "[\"hello\", \"world\", \"foo\"]");
}

void Meta_type_w_std_vector(void) {
    flecs::world ecs;

    ecs.component<std::vector<int>>().opaque(ecs.vector<int>())
        .serialize(Vector_serialize<int>);

    ecs.component<TFlecsTestVector>()
        .member<std::vector<int>>("v");

    TFlecsTestVector v = {{1, 2, 3}};
    flecs::string json = ecs.to_json(&v);
    test_str(json, "{\"v\":[1, 2, 3]}");
}

void Meta_type_w_std_string(void) {
    flecs::world ecs;

    ecs.component<std::string>().opaque(flecs::String)
        .serialize(String_serialize);

    ecs.component<TFlecsTestString>()
        .member<std::string>("v");

    TFlecsTestString v = {"hello world"};
    flecs::string json = ecs.to_json(&v);
    test_str(json, "{\"v\":\"hello world\"}");
}

/*void Meta_type_w_std_vector_std_string(void) {
    flecs::world ecs;

    ecs.component<std::string>().opaque(flecs::String)
        .serialize(String_serialize);

    ecs.component<std::vector<std::string>>().opaque(ecs.vector(flecs::String))
        .serialize(Vector_serialize<std::string>);

    ecs.component<TFlecsTestVectorString>()
        .member<std::vector<std::string>>("v");

    TFlecsTestVectorString v = {{"hello", "world"}};
    flecs::string json = ecs.to_json(&v);
    TestTrue("Json is correct", json == "[\"hello\", \"world\"]");
}*/

void Meta_type_w_std_string_std_vector(void) {
    flecs::world ecs;

    ecs.component<std::vector<int>>().opaque(ecs.vector<int>())
        .serialize(Vector_serialize<int>);

    ecs.component<std::string>().opaque(flecs::String)
        .serialize(String_serialize);

    ecs.component<TFlecsTestStringVector>()
        .member<std::string>("s")
        .member<std::vector<int>>("v");

    TFlecsTestStringVector v = {"hello world", {1, 2, 3}};
    flecs::string json = ecs.to_json(&v);
    test_str(json, "{\"s\":\"hello world\", \"v\":[1, 2, 3]}");
}

void Meta_type_w_std_string_std_string(void) {
    flecs::world ecs;

    ecs.component<std::vector<int>>().opaque(ecs.vector<int>())
        .serialize(Vector_serialize<int>);

    ecs.component<std::string>().opaque(flecs::String)
        .serialize(String_serialize);

    ecs.component<TFlecsTestStringString>()
        .member<std::string>("s1")
        .member<std::string>("s2");

    TFlecsTestStringString v = {"hello world", "foo bar"};
    flecs::string json = ecs.to_json(&v);
    test_str(json, "{\"s1\":\"hello world\", \"s2\":\"foo bar\"}");
}


void Meta_type_w_std_vector_std_vector(void) {
    flecs::world ecs;

    ecs.component<std::vector<int>>().opaque(ecs.vector<int>())
        .serialize(Vector_serialize<int>);

    ecs.component<std::string>().opaque(flecs::String)
        .serialize(String_serialize);

    ecs.component<TFlecsTestVectorVector>()
        .member<std::vector<int>>("v1")
        .member<std::vector<int>>("v2");

    TFlecsTestVectorVector v = {{1, 2, 3}, {4, 5, 6}};
    flecs::string json = ecs.to_json(&v);
    test_str(json, "{\"v1\":[1, 2, 3], \"v2\":[4, 5, 6]}");
}

void Meta_type_w_std_vector_std_string_std_vector(void) {
    flecs::world ecs;

    ecs.component<std::vector<int>>().opaque(ecs.vector<int>())
        .serialize(Vector_serialize<int>);

    ecs.component<std::string>().opaque(flecs::String)
        .serialize(String_serialize);

    ecs.component<TFlecsTestVectorStringVector>()
        .member<std::vector<int>>("v1")
        .member<std::string>("s")
        .member<std::vector<int>>("v2");

    TFlecsTestVectorStringVector v = {{1, 2, 3}, "hello world", {4, 5, 6}};
    flecs::string json = ecs.to_json(&v);
    test_str(json, "{\"v1\":[1, 2, 3], \"s\":\"hello world\", \"v2\":[4, 5, 6]}");
}

void Meta_type_w_std_vector_std_vector_std_string(void) {
    flecs::world ecs;

    ecs.component<std::vector<int>>().opaque(ecs.vector<int>())
        .serialize(Vector_serialize<int>);

    ecs.component<std::string>().opaque(flecs::String)
        .serialize(String_serialize);

    ecs.component<TFlecsTestVectorVectorString>()
        .member<std::vector<int>>("v1")
        .member<std::vector<int>>("v2")
        .member<std::string>("s");

    TFlecsTestVectorVectorString v = {{1, 2, 3}, {4, 5, 6}, "hello world"};
    flecs::string json = ecs.to_json(&v);
    test_str(json, "{\"v1\":[1, 2, 3], \"v2\":[4, 5, 6], \"s\":\"hello world\"}");
}

void Meta_primitive_type(void) {
    flecs::world ecs;

    flecs::entity t = ecs.primitive(flecs::meta::I32);
    test_assert(t != 0);

    test_assert(t.has<flecs::Component>());
    test_assert(t.has<flecs::Type>());
    test_assert(t.has<flecs::Primitive>());

    const flecs::Component *c = t.try_get<flecs::Component>();
    test_assert(c != nullptr);
    test_int(c->size, 4);
    test_int(c->alignment, 4);

    const flecs::Type *mt = t.try_get<flecs::Type>();
    test_assert(mt != nullptr);
    test_assert(mt->kind == flecs::meta::PrimitiveType);

    const flecs::Primitive *pt = t.try_get<flecs::Primitive>();
    test_assert(pt != nullptr);
    test_assert(pt->kind == flecs::meta::I32);
}

void Meta_array_type(void) {
    flecs::world ecs;

    flecs::entity t = ecs.array(flecs::I32, 3);
    test_assert(t != 0);

    test_assert(t.has<flecs::Component>());
    test_assert(t.has<flecs::Type>());
    test_assert(t.has<flecs::Array>());

    const flecs::Component *c = t.try_get<flecs::Component>();
    test_assert(c != nullptr);
    test_int(c->size, 3 * 4);
    test_int(c->alignment, 4);

    const flecs::Type *mt = t.try_get<flecs::Type>();
    test_assert(mt != nullptr);
    test_assert(mt->kind == flecs::meta::ArrayType);

    const flecs::Array *at = t.try_get<flecs::Array>();
    test_assert(at != nullptr);
    test_assert(at->type == ecs.id<int32_t>());
    test_int(at->count, 3);
}

void Meta_vector_type(void) {
    flecs::world ecs;

    flecs::entity t = ecs.vector(flecs::I32);
    test_assert(t != 0);

    test_assert(t.has<flecs::Component>());
    test_assert(t.has<flecs::Type>());
    test_assert(t.has<flecs::Vector>());

    const flecs::Component *c = t.try_get<flecs::Component>();
    test_assert(c != nullptr);
    test_int(c->size, ECS_SIZEOF(ecs_vec_t));
    test_int(c->alignment, ECS_SIZEOF(void*));

    const flecs::Type *mt = t.try_get<flecs::Type>();
    test_assert(mt != nullptr);
    test_assert(mt->kind == flecs::meta::VectorType);

    const flecs::Vector *vt = t.try_get<flecs::Vector>();
    test_assert(vt != nullptr);
    test_assert(vt->type == ecs.id<int32_t>());
}

void Meta_i32_from_json(void) {
    flecs::world ecs;

    int32_t v = 0;
    const char *r = ecs.from_json(&v, "10");
    test_str(r, "");
    test_int(v, 10);
}

void Meta_struct_from_json(void) {
    flecs::world ecs;

    ecs.component<Position>()
        .member<float>("x")
        .member<float>("y");

    Position v = {};
    const char *r = ecs.from_json(&v, "{\"x\":10, \"y\":20}");
    test_str(r, "");
    test_int(v.x, 10);
    test_int(v.y, 20);
}

void Meta_void_from_json(void) {
    flecs::world ecs;

    flecs::entity pos = ecs.component<Position>()
        .member<float>("x")
        .member<float>("y");

    Position v = {};
    void *ptr = &v;
    const char *r = ecs.from_json(pos, ptr, "{\"x\":10, \"y\":20}");
    test_str(r, "");
    test_int(v.x, 10);
    test_int(v.y, 20);
}

void Meta_entity_from_json_empty(void) {
    flecs::world ecs;

    ecs.component<Position>()
        .member<float>("x")
        .member<float>("y");

    flecs::entity e = ecs.entity();

    const char *r = e.from_json("{}");
    test_str(r, "");
}

void Meta_entity_from_json_w_path(void) {
    flecs::world ecs;

    ecs.component<Position>()
        .member<float>("x")
        .member<float>("y");

    flecs::entity e = ecs.entity();

    const char *r = e.from_json("{\"name\":\"ent\"}");
    test_str(r, "");

    test_assert(e != 0);
    test_str(e.name(), "ent");
}

void Meta_entity_from_json_w_ids(void) {
    flecs::world ecs;

    ecs.component<Position>()
        .member<float>("x")
        .member<float>("y");

    flecs::entity e = ecs.entity();

    const char *r = e.from_json("{\"name\":\"ent\", \"tags\":[\"Position\"]}");
    test_str(r, "");

    test_assert(e != 0);
    test_str(e.name(), "ent");
    test_assert(e.has<Position>());
}

void Meta_entity_from_json_w_values(void) {
    flecs::world ecs;

    ecs.component<Position>()
        .member<float>("x")
        .member<float>("y");

    flecs::entity e = ecs.entity();

    const char *r = e.from_json("{\"name\":\"ent\", \"components\":{\"Position\": {\"x\":10, \"y\":20}}}");
    test_str(r, "");

    test_assert(e != 0);
    test_str(e.name(), "ent");
    test_assert(e.has<Position>());

    const Position *p = e.try_get<Position>();
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Meta_set_type_json(void) {
    flecs::world ecs;

    ecs.component<Position>()
        .member<float>("x")
        .member<float>("y");

    flecs::entity e = ecs.entity()
        .set_json<Position>("{\"x\":10, \"y\":20}");

    const Position *p = e.try_get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Meta_set_pair_R_T_json(void) {
    flecs::world ecs;
    ecs.component<Tag>();

    ecs.component<Position>()
        .member<float>("x")
        .member<float>("y");

    flecs::entity e = ecs.entity()
        .set_json<Position, Tag>("{\"x\":10, \"y\":20}");

    const Position *p = e.try_get<Position, Tag>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Meta_set_pair_R_t_json(void) {
    flecs::world ecs;

    ecs.component<Position>()
        .member<float>("x")
        .member<float>("y");

    flecs::entity tgt = ecs.entity();

    flecs::entity e = ecs.entity()
        .set_json<Position>(tgt, "{\"x\":10, \"y\":20}");

    const Position *p = e.try_get<Position>(tgt);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}


void Meta_set_pair_r_T_json(void) {
    flecs::world ecs;
    ecs.component<Tag>();

    flecs::entity pos = ecs.component<Position>()
        .member<float>("x")
        .member<float>("y");

    flecs::entity e = ecs.entity()
        .set_json_second<Tag>(pos, "{\"x\":10, \"y\":20}");

    const Position *p = e.try_get<Position, Tag>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Meta_set_pair_r_t_json(void) {
    flecs::world ecs;

    flecs::entity pos = ecs.component<Position>()
        .member<float>("x")
        .member<float>("y");

    flecs::entity tgt = ecs.entity();

    flecs::entity e = ecs.entity()
        .set_json(pos, tgt, "{\"x\":10, \"y\":20}");

    const Position *p = e.try_get<Position>(tgt);
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Meta_set_id_json(void) {
    flecs::world ecs;

    flecs::entity pos = ecs.component<Position>()
        .member<float>("x")
        .member<float>("y");

    flecs::entity e = ecs.entity()
        .set_json(pos, "{\"x\":10, \"y\":20}");

    const Position *p = e.try_get<Position>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void Meta_ser_deser_std_string(void) {
    flecs::world world;

    world.component<std::string>()
        .opaque(&std_string_support);

    std::string v = "hello world";
    test_str(world.to_json(&v).c_str(), "\"hello world\"");

    world.from_json(&v, "\"foo bar\"");
    test_str(world.to_json(&v).c_str(), "\"foo bar\"");
}

void Meta_ser_deser_std_vector_int(void) {
    flecs::world world;

    world.component<std::vector<int>>()
        .opaque(std_vector_support<int>);

    std::vector<int> v = {1, 2, 3};
    test_str(world.to_json(&v).c_str(), "[1, 2, 3]");

    world.from_json(&v, "[4, 5, 6]");
    test_str(world.to_json(&v).c_str(), "[4, 5, 6]");
}

void Meta_ser_deser_std_vector_std_string(void) {
    flecs::world world;

    world.component<std::string>()
        .opaque(std_string_support);

    world.component<std::vector<std::string>>()
        .opaque(std_vector_support<std::string>);

    std::vector<std::string> v = {"foo", "bar"};
    test_str(world.to_json(&v).c_str(), "[\"foo\", \"bar\"]");

    world.from_json(&v, "[\"hello\", \"world\"]");
    test_str(world.to_json(&v).c_str(), "[\"hello\", \"world\"]");
}

void Meta_ser_deser_type_w_std_string_std_vector_std_string(void) {
    flecs::world world;

    world.component<std::string>()
        .opaque(std_string_support);

    world.component<std::vector<std::string>>()
        .opaque(std_vector_support<std::string>);

    world.component<CppTypes>()
        .member<std::string>("s")
        .member<std::vector<std::string>>("v");

    CppTypes v = {"hello", {"world"}};

    test_str(world.to_json(&v).c_str(), "{\"s\":\"hello\", \"v\":[\"world\"]}");

    world.from_json(&v, "{\"s\":\"foo\", \"v\":[\"bar\"]}");

    test_str(world.to_json(&v).c_str(), "{\"s\":\"foo\", \"v\":[\"bar\"]}");
}

void Meta_ser_deser_flecs_entity(void) {
    flecs::world world;

    flecs::entity e1 = world.entity("ent1");
    flecs::entity e2 = world.entity("ent2");

    flecs::entity v = e1;
    test_str(world.to_json(&v).c_str(), "\"ent1\"");

    world.from_json(&v, "\"ent2\"");
    test_str(world.to_json(&v).c_str(), "\"ent2\"");
    test_assert(v == e2);
}

void Meta_world_ser_deser_flecs_entity(void) {
    flecs::world world;

    world.component<CppEntity>()
        .member<flecs::entity>("entity");

    flecs::entity e1 = world.entity("ent1");
    flecs::entity e2 = world.entity("ent2").set<CppEntity>({e1});

    {
        const CppEntity *ptr = e2.try_get<CppEntity>();
        test_assert(ptr != nullptr);
        test_str(world.to_json(ptr).c_str(), "{\"entity\":\"ent1\"}");
    }

    auto json = world.to_json();
    world.from_json(json);

    test_assert(e1.is_alive());
    test_assert(e2.is_alive());

    {
        const CppEntity *ptr = e2.try_get<CppEntity>();
        test_assert(ptr != nullptr);
        test_str(world.to_json(ptr).c_str(), "{\"entity\":\"ent1\"}");
    }
}

void Meta_opaque_vector_w_builder(void) {
    flecs::world world;

    world.component<std::vector<int>>().opaque<int>(world.vector<int>())
        .serialize([](const flecs::serializer *s, const std::vector<int> *data) {
            for (const auto& el : *data) {
                s->value(el);
            }
            return 0;
        })
        .count([](const std::vector<int> *data) {
            return data->size();
        })
        .ensure_element([](std::vector<int> *data, size_t elem) {
            if (data->size() <= elem) {
                data->resize(elem + 1);
            }

            return &data->data()[elem];
        })
        .resize([](std::vector<int> *data, size_t size) {
            data->resize(size);
        });

    std::vector<int> v = {};

    world.from_json(&v, "[10, 20, 30]");
    test_uint(v.size(), 3);
    test_int(v[0], 10);
    test_int(v[1], 20);
    test_int(v[2], 30);

    test_str(world.to_json(&v).c_str(), "[10, 20, 30]");
}

void Meta_deser_entity_w_path(void) {
    flecs::world world;

    flecs::entity ent = world.entity("ent");

    flecs::entity e;
    world.from_json(&e, "\"ent\"");

    test_assert(e == ent);
    test_str(e.path().c_str(), "::ent");
}

void Meta_enum_w_bits(void) {
    flecs::world ecs;

    flecs::log::set_level(-4);

    /* It is illegal to register an enumeration as bitset, this test makes sure
     * the code doesn't crash. */
	ecs.component<EnumWithBits>()
		.bit("BitA", (uint32_t)EnumWithBits::BitA)
		.bit("BitB", (uint32_t)EnumWithBits::BitB)
        .bit("BitAll", (uint32_t)EnumWithBits::BitAll);

	ecs.component<EnumWithBitsStruct>()
		.member<EnumWithBits>("bits");

    for (int i = 0; i < 1; i++)
	{
        ecs.entity("e")
            .child_of(ecs.entity("p"))
			.add<EnumWithBitsStruct>();
	}

    auto q = ecs.query<EnumWithBitsStruct>();
    auto s = q.iter().to_json();
    test_str(s.c_str(), "{\"results\":[{\"parent\":\"p\", \"name\":\"e\", \"fields\":{\"values\":[{\"bits\":\"BitAll\"}]}}]}");
}

void Meta_value_range(void) {
    flecs::world ecs;

    flecs::entity s = ecs.component<Position>()
        .member<float>("x").range(-1, 1)
        .member<float>("y").range(-2, 2);

    flecs::entity x = s.lookup("x");
    test_assert(x != 0);
    test_assert(x.has<flecs::MemberRanges>());
    {
        const flecs::MemberRanges *ranges = x.try_get<flecs::MemberRanges>();
        test_assert(ranges != nullptr);
        test_assert(ranges->value.min == -1);
        test_assert(ranges->value.max == 1);
    }

    flecs::entity y = s.lookup("y");
    test_assert(y != 0);
    test_assert(y.has<flecs::MemberRanges>());
    {
        const flecs::MemberRanges *ranges = y.try_get<flecs::MemberRanges>();
        test_assert(ranges != nullptr);
        test_assert(ranges->value.min == -2);
        test_assert(ranges->value.max == 2);
    }
}

void Meta_warning_range(void) {
    flecs::world ecs;

    flecs::entity s = ecs.component<Position>()
        .member<float>("x").warning_range(-1, 1)
        .member<float>("y").warning_range(-2, 2);

    flecs::entity x = s.lookup("x");
    test_assert(x != 0);
    test_assert(x.has<flecs::MemberRanges>());
    {
        const flecs::MemberRanges *ranges = x.try_get<flecs::MemberRanges>();
        test_assert(ranges != nullptr);
        test_assert(ranges->warning.min == -1);
        test_assert(ranges->warning.max == 1);
    }

    flecs::entity y = s.lookup("y");
    test_assert(y != 0);
    test_assert(y.has<flecs::MemberRanges>());
    {
        const flecs::MemberRanges *ranges = y.try_get<flecs::MemberRanges>();
        test_assert(ranges != nullptr);
        test_assert(ranges->warning.min == -2);
        test_assert(ranges->warning.max == 2);
    }
}

void Meta_error_range(void) {
    flecs::world ecs;

    flecs::entity s = ecs.component<Position>()
        .member<float>("x").error_range(-1, 1)
        .member<float>("y").error_range(-2, 2);

    flecs::entity x = s.lookup("x");
    test_assert(x != 0);
    test_assert(x.has<flecs::MemberRanges>());
    {
        const flecs::MemberRanges *ranges = x.try_get<flecs::MemberRanges>();
        test_assert(ranges != nullptr);
        test_assert(ranges->error.min == -1);
        test_assert(ranges->error.max == 1);
    }

    flecs::entity y = s.lookup("y");
    test_assert(y != 0);
    test_assert(y.has<flecs::MemberRanges>());
    {
        const flecs::MemberRanges *ranges = y.try_get<flecs::MemberRanges>();
        test_assert(ranges != nullptr);
        test_assert(ranges->error.min == -2);
        test_assert(ranges->error.max == 2);
    }
}

void Meta_struct_member_ptr(void) {
    flecs::world ecs;

    struct Test {
        int32_t x;
    };
    struct Test2 {
        double y;
    };
    struct Nested {
        Test a;
        int pad; // This will be ignored
        Test2 b[2];
    };

    auto t = ecs.component<Test>()
        .member("x", &Test::x);

    auto t2 = ecs.component<Test2>()
        .member("y", &Test2::y);

    auto n = ecs.component<Nested>()
        .member("a", &Nested::a)
        .member("b", &Nested::b);

    { // Validate Test
        test_assert(t != 0);

        auto x = t.lookup("x");
        test_assert(x != 0);
        test_assert(x.has<flecs::Member>());
        const flecs::Member *xm = x.try_get<flecs::Member>();
        test_uint(xm->type, flecs::I32);
        test_uint(xm->offset, offsetof(Test, x));
    }
    { // Validate Test2
        test_assert(t2 != 0);

        auto y = t2.lookup("y");
        test_assert(y != 0);
        test_assert(y.has<flecs::Member>());
        const flecs::Member *ym = y.try_get<flecs::Member>();
        test_uint(ym->type, flecs::F64);
        test_uint(ym->offset, offsetof(Test2, y));
    }

    // Validate Nested
    test_assert(n != 0);
    {
        auto a = n.lookup("a");
        test_assert(a != 0);
        test_assert(a.has<flecs::Member>());
        const flecs::Member *am = a.try_get<flecs::Member>();
        test_uint(am->type, t);
        test_uint(am->offset, offsetof(Nested, a));
    }
    {
        auto b = n.lookup("b");
        test_assert(b != 0);
        test_assert(b.has<flecs::Member>());
        const flecs::Member *bm = b.try_get<flecs::Member>();
        test_uint(bm->type, t2);
        test_uint(bm->offset, offsetof(Nested, b));
        test_uint(bm->count, 2);
    }
}

void Meta_struct_member_ptr_packed_struct(void) {
    flecs::world ecs;

    #if defined(__GNUC__) || defined(__clang__)
    struct FlecsTestPackedStruct {
        char a;
        int b;
        char pad[2];
        double c;
    } __attribute__((__packed__));
    #endif

    #if defined(_MSC_VER) && !defined(__clang__)
    #pragma pack(push, 1)
    struct FlecsTestPackedStruct {
        char a;
        int b;
        char pad[2];
        double c;
    };
    #pragma pack(pop)
    #endif

    auto s = ecs.component<FlecsTestPackedStruct>()
        .member("a", &FlecsTestPackedStruct::a)
        .member("b", &FlecsTestPackedStruct::b)
        .member("c", &FlecsTestPackedStruct::c);

    test_assert(s != 0);

    {
        auto a = s.lookup("a");
        test_assert(a != 0);
        test_assert(a.has<flecs::Member>());
        const flecs::Member *am = a.try_get<flecs::Member>();
        test_uint(am->type, flecs::Char);
        test_uint(am->offset, offsetof(FlecsTestPackedStruct, a));
    }
    {
        auto b = s.lookup("b");
        test_assert(b != 0);
        test_assert(b.has<flecs::Member>());
        const flecs::Member *bm = b.try_get<flecs::Member>();
        test_uint(bm->type, flecs::I32);
        test_uint(bm->offset, offsetof(FlecsTestPackedStruct, b));
    }
    {
        auto c = s.lookup("c");
        test_assert(c != 0);
        test_assert(c.has<flecs::Member>());
        const flecs::Member *cm = c.try_get<flecs::Member>();
        test_uint(cm->type, flecs::F64);
        test_uint(cm->offset, offsetof(FlecsTestPackedStruct, c));
    }
}

void Meta_component_as_array(void) {
    flecs::world ecs;

    flecs::entity c = ecs.component<Position>()
        .array<float>(2);

    test_assert(c.has<flecs::Array>());

    const flecs::Array *ptr = c.try_get<flecs::Array>();
    test_assert(ptr != nullptr);
    test_assert(ptr->type == ecs.id<float>());
    test_int(ptr->count, 2);
}

void Meta_out_of_order_member_declaration(void) {
    flecs::world ecs;

    auto c = ecs.component<Position>()
        .member<float>("y", 0, offsetof(Position, y))
        .member<float>("x", 0, offsetof(Position, x));
    test_assert(c != 0);

    const flecs::Component *ptr = c.try_get<flecs::Component>();
    test_int(ptr->size, 8);
    test_int(ptr->alignment, 4);

    auto xe = c.lookup("x");
    test_assert(xe != 0);
    test_assert( xe.has<flecs::Member>() );
    const flecs::Member *x = xe.try_get<flecs::Member>();
    test_uint(x->type, flecs::F32);
    test_uint(x->offset, 0);

    auto ye = c.lookup("y");
    test_assert(ye != 0);
    test_assert( ye.has<flecs::Member>() );
    const flecs::Member *y = ye.try_get<flecs::Member>();
    test_uint(y->type, flecs::F32);
    test_uint(y->offset, 4);

    flecs::entity e2 = ecs.entity("ent2").set<Position>({10, 20});
    const Position *p = e2.try_get<Position>();

    auto json = ecs.to_json(p);
    test_str(json.c_str(), "{\"y\":20, \"x\":10}");

    Position p2 = {};
    ecs.from_json(&p2, json.c_str());
    test_int(p2.x, 10);
    test_int(p2.y, 20);
}

void Meta_entity_to_json(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    flecs::entity e = ecs.entity("foo").set<Position>({10, 20});

    test_str(e.to_json().c_str(), "{\"name\":\"foo\", \"components\":{\"Position\":null}}");
}

void Meta_iter_to_json(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    ecs.entity("foo").set<Position>({10, 20});

    auto q = ecs.query<Position>();

    test_str(q.iter().to_json().c_str(), "{\"results\":[{\"name\":\"foo\", \"fields\":{\"values\":[0]}}]}");
}

void Meta_query_to_json(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    ecs.entity("foo").set<Position>({10, 20});

    auto q = ecs.query<Position>();

    test_str(q.to_json().c_str(), "{\"results\":[{\"name\":\"foo\", \"fields\":{\"values\":[0]}}]}");
}

void Meta_entity_to_json_w_default_desc(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    flecs::entity e = ecs.entity("foo").set<Position>({10, 20});

    flecs::entity_to_json_desc_t desc = ECS_ENTITY_TO_JSON_INIT;

    test_str(e.to_json(&desc).c_str(), "{\"name\":\"foo\", \"components\":{\"Position\":null}}");
}

void Meta_query_to_json_w_default_desc(void) {
    flecs::world ecs;
    RegisterTestTypeComponents(ecs);

    ecs.entity("foo").set<Position>({10, 20});

    auto q = ecs.query<Position>();

    flecs::iter_to_json_desc_t desc = ECS_ITER_TO_JSON_INIT;

    test_str(q.to_json(&desc).c_str(), "{\"results\":[{\"name\":\"foo\", \"fields\":{\"values\":[0]}}]}");
}

void Meta_script_to_std_vector_int(void) {
    flecs::world world;

    world.component<std::vector<int>>("IntVec")
        .opaque(std_vector_support<int>);

    flecs::entity s = world.script()
        .code("e { IntVec: [10, 20, 30] }")
        .run();

    const flecs::Script& sptr = s.get<flecs::Script>();
    test_assert(sptr.error == nullptr);

    flecs::entity e = world.lookup("e");
    test_assert(e != 0);

    const std::vector<int>& v = e.get<std::vector<int>>();
    test_int(v.size(), 3);
    test_int(v.at(0), 10);
    test_int(v.at(1), 20);
    test_int(v.at(2), 30);
}

void Meta_script_to_std_vector_std_string(void) {
    flecs::world world;

    world.component<std::string>()
        .opaque(std_string_support);

    world.component<std::vector<std::string>>("StringVec")
        .opaque(std_vector_support<std::string>);

    flecs::entity s = world.script()
        .code("e { StringVec: [\"Hello\", \"World\"] }")
        .run();

    const flecs::Script& sptr = s.get<flecs::Script>();
    test_assert(sptr.error == nullptr);

    flecs::entity e = world.lookup("e");
    test_assert(e != 0);

    const std::vector<std::string>& v = e.get<std::vector<std::string>>();
    test_int(v.size(), 2);
    test_str(v.at(0).c_str(), "Hello");
    test_str(v.at(1).c_str(), "World");
}

void Meta_anonymous_opaque_as_type_parent(void) {
    // Implement testcase
}

void Meta_named_opaque_as_type_parent(void) {
    // Implement testcase
}

void Meta_parented_opaque_as_type_parent(void) {
    // Implement testcase
}

END_DEFINE_SPEC(FFlecsMetaTestsSpec);

/*"id": "Meta",
            "testcases": [
                "struct",
                "nested_struct",
                "struct_w_portable_type",
                "partial_struct",
                "partial_struct_custom_offset",
                "units",
                "unit_w_quantity",
                "unit_w_prefix",
                "unit_w_over",
                "bitmask",
                "custom_i32_to_json",
                "custom_std_string_to_json",
                "custom_std_vector_i32_to_json",
                "custom_std_vector_std_string_to_json",
                "type_w_std_vector",
                "type_w_std_string",
                "type_w_std_vector_std_string",
                "type_w_std_string_std_vector",
                "type_w_std_string_std_string",
                "type_w_std_vector_std_vector",
                "type_w_std_vector_std_string_std_vector",
                "type_w_std_vector_std_vector_std_string",
                "primitive_type",
                "array_type",
                "vector_type",
                "i32_from_json",
                "struct_from_json",
                "void_from_json",
                "entity_from_json_empty",
                "entity_from_json_w_path",
                "entity_from_json_w_ids",
                "entity_from_json_w_values",
                "set_type_json",
                "set_pair_R_T_json",
                "set_pair_R_t_json",
                "set_pair_r_T_json",
                "set_pair_r_t_json",
                "set_id_json",
                "ser_deser_std_string",
                "ser_deser_std_vector_int",
                "ser_deser_std_vector_std_string",
                "ser_deser_type_w_std_string_std_vector_std_string",
                "ser_deser_flecs_entity",
                "world_ser_deser_flecs_entity",
                "opaque_vector_w_builder",
                "deser_entity_w_path",
                "enum_w_bits",
                "value_range",
                "warning_range",
                "error_range",
                "struct_member_ptr",
                "struct_member_ptr_packed_struct",
                "component_as_array",
                "out_of_order_member_declaration",
                "entity_to_json",
                "iter_to_json",
                "query_to_json",
                "entity_to_json_w_default_desc",
                "query_to_json_w_default_desc",
                "script_to_std_vector_int",
                "script_to_std_vector_std_string",
                "anonymous_opaque_as_type_parent",
                "named_opaque_as_type_parent",
                "parented_opaque_as_type_parent"
            ]*/

void FFlecsMetaTestsSpec::Define()
{
	It("Meta_struct", [&]() { Meta_struct(); });
    It("Meta_nested_struct", [&]() { Meta_nested_struct(); });
    It("Meta_struct_w_portable_type", [&]() { Meta_struct_w_portable_type(); });
    It("Meta_partial_struct", [&]() { Meta_partial_struct(); });
    It("Meta_partial_struct_custom_offset", [&]() { Meta_partial_struct_custom_offset(); });
    It("Meta_units", [&]() { Meta_units(); });
    It("Meta_unit_w_quantity", [&]() { Meta_unit_w_quantity(); });
    It("Meta_unit_w_prefix", [&]() { Meta_unit_w_prefix(); });
    It("Meta_unit_w_over", [&]() { Meta_unit_w_over(); });
    It("Meta_bitmask", [&]() { Meta_bitmask(); });
    It("Meta_custom_i32_to_json", [&]() { Meta_custom_i32_to_json(); });
    It("Meta_custom_std_string_to_json", [&]() { Meta_custom_std_string_to_json(); });
    It("Meta_custom_std_vector_i32_to_json", [&]() { Meta_custom_std_vector_i32_to_json(); });
    It("Meta_custom_std_vector_std_string_to_json", [&]() { Meta_custom_std_vector_std_string_to_json(); });
    It("Meta_type_w_std_vector", [&]() { Meta_type_w_std_vector(); });
    It("Meta_type_w_std_string", [&]() { Meta_type_w_std_string(); });
    //It("Meta_type_w_std_vector_std_string", [&]() { Meta_type_w_std_vector_std_string(); });
    It("Meta_type_w_std_string_std_vector", [&]() { Meta_type_w_std_string_std_vector(); });
    It("Meta_type_w_std_string_std_string", [&]() { Meta_type_w_std_string_std_string(); });
    It("Meta_type_w_std_vector_std_vector", [&]() { Meta_type_w_std_vector_std_vector(); });
    It("Meta_type_w_std_vector_std_string_std_vector", [&]() { Meta_type_w_std_vector_std_string_std_vector(); });
    It("Meta_type_w_std_vector_std_vector_std_string", [&]() { Meta_type_w_std_vector_std_vector_std_string(); });
    It("Meta_primitive_type", [&]() { Meta_primitive_type(); });
    It("Meta_array_type", [&]() { Meta_array_type(); });
    It("Meta_vector_type", [&]() { Meta_vector_type(); });
    It("Meta_i32_from_json", [&]() { Meta_i32_from_json(); });
    It("Meta_struct_from_json", [&]() { Meta_struct_from_json(); });
    It("Meta_void_from_json", [&]() { Meta_void_from_json(); });
    It("Meta_entity_from_json_empty", [&]() { Meta_entity_from_json_empty(); });
    It("Meta_entity_from_json_w_path", [&]() { Meta_entity_from_json_w_path(); });
    It("Meta_entity_from_json_w_ids", [&]() { Meta_entity_from_json_w_ids(); });
    It("Meta_entity_from_json_w_values", [&]() { Meta_entity_from_json_w_values(); });
    It("Meta_set_type_json", [&]() { Meta_set_type_json(); });
    It("Meta_set_pair_R_type_T_type_json", [&]() { Meta_set_pair_R_T_json(); });
    It("Meta_set_pair_R_type_t_entity_json", [&]() { Meta_set_pair_R_t_json(); });
    It("Meta_set_pair_r_entity_T_type_json", [&]() { Meta_set_pair_r_T_json(); });
    It("Meta_set_pair_r_entity_t_entity_json", [&]() { Meta_set_pair_r_t_json(); });
    It("Meta_set_id_json", [&]() { Meta_set_id_json(); });
    It("Meta_ser_deser_std_string", [&]() { Meta_ser_deser_std_string(); });
    It("Meta_ser_deser_std_vector_int", [&]() { Meta_ser_deser_std_vector_int(); });
    It("Meta_ser_deser_std_vector_std_string", [&]() { Meta_ser_deser_std_vector_std_string(); });
    It("Meta_ser_deser_type_w_std_string_std_vector_std_string", [&]() { Meta_ser_deser_type_w_std_string_std_vector_std_string(); });
    It("Meta_ser_deser_flecs_entity", [&]() { Meta_ser_deser_flecs_entity(); });
    //It("Meta_world_ser_deser_flecs_entity", [&]() { Meta_world_ser_deser_flecs_entity(); });
    It("Meta_opaque_vector_w_builder", [&]() { Meta_opaque_vector_w_builder(); });
    It("Meta_deser_entity_w_path", [&]() { Meta_deser_entity_w_path(); });
    It("Meta_enum_w_bits", [&]() { Meta_enum_w_bits(); });
    It("Meta_value_range", [&]() { Meta_value_range(); });
    It("Meta_warning_range", [&]() { Meta_warning_range(); });
    It("Meta_error_range", [&]() { Meta_error_range(); });
    It("Meta_struct_member_ptr", [&]() { Meta_struct_member_ptr(); });
    It("Meta_struct_member_ptr_packed_struct", [&]() { Meta_struct_member_ptr_packed_struct(); });
    It("Meta_component_as_array", [&]() { Meta_component_as_array(); });
    It("Meta_out_of_order_member_declaration", [&]() { Meta_out_of_order_member_declaration(); });
    It("Meta_entity_to_json", [&]() { Meta_entity_to_json(); });
    It("Meta_iter_to_json", [&]() { Meta_iter_to_json(); });
    It("Meta_query_to_json", [&]() { Meta_query_to_json(); });
    It("Meta_entity_to_json_w_default_desc", [&]() { Meta_entity_to_json_w_default_desc(); });
    It("Meta_query_to_json_w_default_desc", [&]() { Meta_query_to_json_w_default_desc(); });
    It("Meta_script_to_std_vector_int", [&]() { Meta_script_to_std_vector_int(); });
    It("Meta_script_to_std_vector_std_string", [&]() { Meta_script_to_std_vector_std_string(); });
    It("Meta_anonymous_opaque_as_type_parent", [&]() { Meta_anonymous_opaque_as_type_parent(); });
    It("Meta_named_opaque_as_type_parent", [&]() { Meta_named_opaque_as_type_parent(); });
    It("Meta_parented_opaque_as_type_parent", [&]() { Meta_parented_opaque_as_type_parent(); });
}

#endif // WITH_AUTOMATION_TESTS
