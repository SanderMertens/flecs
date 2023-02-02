#include <cpp_api.h>

void Meta_struct() {
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
    const flecs::Member *m = a.get<flecs::Member>();
    test_uint(m->type, flecs::I32);

    auto b = c.lookup("b");
    test_assert(b != 0);
    test_assert( b.has<flecs::Member>() );
    m = b.get<flecs::Member>();
    test_uint(m->type, flecs::F32);
}

void Meta_nested_struct() {
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
    const flecs::Member *m = a.get<flecs::Member>();
    test_uint(m->type, t);
}

void Meta_struct_w_portable_type() {
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
    const flecs::Member *m = a.get<flecs::Member>();
    test_uint(m->type, ecs.component<uintptr_t>());

    auto b = t.lookup("b");
    test_assert(b != 0);
    test_assert( b.has<flecs::Member>() );
    m = b.get<flecs::Member>();
    test_uint(m->type, flecs::Uptr);

    auto c = t.lookup("c");
    test_assert(c != 0);
    test_assert( c.has<flecs::Member>() );
    m = c.get<flecs::Member>();
    test_uint(m->type, flecs::U64);

    auto d = t.lookup("d");
    test_assert(d != 0);
    test_assert( d.has<flecs::Member>() );
    m = d.get<flecs::Member>();
    test_uint(m->type, flecs::Entity);
}

void Meta_units() {
    struct Test {
        int32_t meters;
        int32_t custom_unit;
    };

    flecs::world ecs;

    ecs.import<flecs::units>();

    auto custom_unit = ecs.entity("some_unit").unit("u");
    test_assert(custom_unit != 0);
    test_str(custom_unit.name(), "some_unit");

    const flecs::Unit *unit = custom_unit.get<flecs::Unit>();
    test_assert(unit != nullptr);
    test_str(unit->symbol, "u");

    auto t = ecs.component<Test>()
        .member<int32_t, flecs::units::length::Meters>("meters")
        .member<int32_t>(custom_unit, "custom_unit");
    test_assert(t != 0);

    flecs::entity m_meters = t.lookup("meters");
    test_assert(m_meters != 0);
    const flecs::Member *m = m_meters.get<flecs::Member>();
    test_assert(m != nullptr);
    test_uint(m->type, flecs::I32);
    test_uint(m->unit, ecs.id<flecs::units::length::Meters>());

    flecs::entity m_custom_unit = t.lookup("custom_unit");
    test_assert(m_custom_unit != 0);
    m = m_custom_unit.get<flecs::Member>();
    test_assert(m != nullptr);
    test_uint(m->type, flecs::I32);
    test_uint(m->unit, custom_unit);
}

void Meta_unit_w_quantity() {
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
    const flecs::Member *m = m_1.get<flecs::Member>();
    test_assert(m != nullptr);
    test_uint(m->type, flecs::I32);
    test_uint(m->unit, unit_1);

    flecs::entity m_2 = t.lookup("m_2");
    test_assert(m_2 != 0);
    m = m_2.get<flecs::Member>();
    test_assert(m != nullptr);
    test_uint(m->type, flecs::I32);
    test_uint(m->unit, unit_2);
}

void Meta_unit_w_prefix() {
    struct Test {
        int32_t m_1;
        int32_t m_2;
    };

    flecs::world ecs;

    ecs.import<flecs::units>();

    auto prefix = ecs.entity().unit_prefix("p", 100, 1);
    
    auto unit_1 = ecs.entity().unit("U1");
    const flecs::Unit *unit = unit_1.get<flecs::Unit>();
    test_assert(unit != nullptr);
    test_str(unit->symbol, "U1");

    auto unit_2 = ecs.entity().unit(prefix, unit_1);
    unit = unit_2.get<flecs::Unit>();
    test_assert(unit != nullptr);
    test_str(unit->symbol, "pU1");
}

void Meta_unit_w_over() {
    struct Test {
        int32_t m_1;
        int32_t m_2;
    };

    flecs::world ecs;

    ecs.import<flecs::units>();

    auto prefix = ecs.entity().unit_prefix("p", 100, 1);
    
    auto unit_0 = ecs.entity().unit("U0");
    const flecs::Unit *unit = unit_0.get<flecs::Unit>();
    test_assert(unit != nullptr);
    test_str(unit->symbol, "U0");

    auto unit_1 = ecs.entity().unit("U1");
    unit = unit_1.get<flecs::Unit>();
    test_assert(unit != nullptr);
    test_str(unit->symbol, "U1");

    auto unit_2 = ecs.entity().unit(prefix, unit_1, unit_0);
    unit = unit_2.get<flecs::Unit>();
    test_assert(unit != nullptr);
    test_str(unit->symbol, "pU1/U0");
}

void Meta_partial_struct() {
    flecs::world ecs;

    auto c = ecs.component<Position>()
        .member<float>("x");
    test_assert(c != 0);

    const flecs::Component *ptr = c.get<flecs::Component>();
    test_int(ptr->size, 8);
    test_int(ptr->alignment, 4);
    
    auto xe = c.lookup("x");
    test_assert(xe != 0);
    test_assert( xe.has<flecs::Member>() );
    const flecs::Member *x = xe.get<flecs::Member>();
    test_uint(x->type, flecs::F32);
    test_uint(x->offset, 0);
}

void Meta_partial_struct_custom_offset() {
    flecs::world ecs;

    auto c = ecs.component<Position>()
        .member<float>("y", 1, offsetof(Position, y));
    test_assert(c != 0);

    const flecs::Component *ptr = c.get<flecs::Component>();
    test_int(ptr->size, 8);
    test_int(ptr->alignment, 4);
    
    auto xe = c.lookup("y");
    test_assert(xe != 0);
    test_assert( xe.has<flecs::Member>() );
    const flecs::Member *x = xe.get<flecs::Member>();
    test_uint(x->type, flecs::F32);
    test_uint(x->offset, 4);
}

struct Toppings : flecs::bitmask {
    static constexpr uint32_t Bacon = 0x1;
    static constexpr uint32_t Lettuce = 0x2;
    static constexpr uint32_t Tomato = 0x4;
};

struct Sandwich {
    uint32_t toppings;
};

void Meta_bitmask() {
    flecs::world ecs;

    // Register components with reflection data
    ecs.component<Toppings>()
        .bit("Bacon", Toppings::Bacon)
        .bit("Lettuce", Toppings::Lettuce)
        .bit("Tomato", Toppings::Tomato);

    ecs.component<Sandwich>()
        .member<Toppings>("toppings");

    // Create entity with Position as usual
    auto e = ecs.entity()
        .set<Sandwich>({Toppings::Bacon | Toppings::Lettuce});

    // Convert position component to flecs expression string
    const Sandwich *ptr = e.get<Sandwich>();
    test_str(ecs.to_expr(ptr).c_str(), "{toppings: Lettuce|Bacon}");
}

struct Int {
    int32_t v;
};

struct TVector {
    std::vector<int> v;
};

struct TString {
    std::string v;
};

struct TVectorString {
    std::vector<int> v;
    std::string s;
};

struct TStringVector {
    std::string s;
    std::vector<int> v;
};

struct TStringString {
    std::string s1;
    std::string s2;
};

struct TVectorVector {
    std::vector<int> v1;
    std::vector<int> v2;
};

struct TVectorVectorString {
    std::vector<int> v1;
    std::vector<int> v2;
    std::string s;
};

struct TVectorStringVector {
    std::vector<int> v1;
    std::string s;
    std::vector<int> v2;
};

static
int Int_serialize(const flecs::serializer *s, const Int *data) {
    return s->value(data->v);
}

static
int String_serialize(const flecs::serializer *s, const std::string *data) {
    const char *str = data->c_str();
    return s->value(flecs::String, &str);
}

template <typename Elem>
int Vector_serialize(const flecs::serializer *ser, const std::vector<Elem> *data) {
    for (const auto& el : *data) {
        if (ser->value(el)) {
            return -1;
        }
    }

    return 0;
}

void Meta_custom_i32_to_json() {
    flecs::world ecs;

    ecs.component<Int>().serialize(flecs::I32, Int_serialize);

    Int v = {10};
    flecs::string json = ecs.to_json(&v);
    test_str(json, "10");
}

void Meta_custom_std_string_to_json() {
    flecs::world ecs;

    ecs.component<std::string>().serialize(flecs::String, String_serialize);

    std::string v = {"Hello World"};
    flecs::string json = ecs.to_json(&v);
    test_str(json, "\"Hello World\"");
}

void Meta_custom_std_vector_i32_to_json() {
    flecs::world ecs;

    ecs.component<std::vector<int>>()
        .serialize(ecs.vector<int>(), Vector_serialize<int>);

    std::vector<int> v = {1, 2, 3};
    flecs::string json = ecs.to_json(&v);
    test_str(json, "[1, 2, 3]");
}

void Meta_custom_std_vector_std_string_to_json() {
    flecs::world ecs;

    ecs.component<std::string>().serialize(flecs::String, String_serialize);

    ecs.component<std::vector<std::string>>()
        .serialize(ecs.vector(flecs::String), Vector_serialize<std::string>);

    std::vector<std::string> v = {"hello", "world", "foo"};
    flecs::string json = ecs.to_json(&v);
    test_str(json, "[\"hello\", \"world\", \"foo\"]");
}

void Meta_type_w_std_vector() {
    flecs::world ecs;

    ecs.component<std::vector<int>>()
        .serialize(ecs.vector<int>(), Vector_serialize<int>);

    ecs.component<TVector>()
        .member<std::vector<int>>("v");

    TVector v = {{1, 2, 3}};
    flecs::string json = ecs.to_json(&v);
    test_str(json, "{\"v\":[1, 2, 3]}");
}

void Meta_type_w_std_string() {
    flecs::world ecs;

    ecs.component<std::string>().serialize(flecs::String, String_serialize);

    ecs.component<TString>()
        .member<std::string>("v");

    TString v = {"hello world"};
    flecs::string json = ecs.to_json(&v);
    test_str(json, "{\"v\":\"hello world\"}");
}

void Meta_type_w_std_vector_std_string() {
    flecs::world ecs;

    ecs.component<std::vector<int>>()
        .serialize(ecs.vector<int>(), Vector_serialize<int>);

    ecs.component<std::string>().serialize(flecs::String, String_serialize);

    ecs.component<TVectorString>()
        .member<std::vector<int>>("v")
        .member<std::string>("s");

    TVectorString v = {{1, 2, 3}, "hello world"};
    flecs::string json = ecs.to_json(&v);
    test_str(json, "{\"v\":[1, 2, 3], \"s\":\"hello world\"}");
}

void Meta_type_w_std_string_std_vector() {
    flecs::world ecs;

    ecs.component<std::vector<int>>()
        .serialize(ecs.vector<int>(), Vector_serialize<int>);

    ecs.component<std::string>().serialize(flecs::String, String_serialize);

    ecs.component<TStringVector>()
        .member<std::string>("s")
        .member<std::vector<int>>("v");

    TStringVector v = {"hello world", {1, 2, 3}};
    flecs::string json = ecs.to_json(&v);
    test_str(json, "{\"s\":\"hello world\", \"v\":[1, 2, 3]}");
}

void Meta_type_w_std_string_std_string() {
    flecs::world ecs;

    ecs.component<std::vector<int>>()
        .serialize(ecs.vector<int>(), Vector_serialize<int>);

    ecs.component<std::string>().serialize(flecs::String, String_serialize);

    ecs.component<TStringString>()
        .member<std::string>("s1")
        .member<std::string>("s2");

    TStringString v = {"hello world", "foo bar"};
    flecs::string json = ecs.to_json(&v);
    test_str(json, "{\"s1\":\"hello world\", \"s2\":\"foo bar\"}");
}

void Meta_type_w_std_vector_std_vector() {
    flecs::world ecs;

    ecs.component<std::vector<int>>()
        .serialize(ecs.vector<int>(), Vector_serialize<int>);

    ecs.component<std::string>().serialize(flecs::String, String_serialize);

    ecs.component<TVectorVector>()
        .member<std::vector<int>>("v1")
        .member<std::vector<int>>("v2");

    TVectorVector v = {{1, 2, 3}, {4, 5, 6}};
    flecs::string json = ecs.to_json(&v);
    test_str(json, "{\"v1\":[1, 2, 3], \"v2\":[4, 5, 6]}");
}

void Meta_type_w_std_vector_std_string_std_vector() {
    flecs::world ecs;

    ecs.component<std::vector<int>>()
        .serialize(ecs.vector<int>(), Vector_serialize<int>);

    ecs.component<std::string>().serialize(flecs::String, String_serialize);

    ecs.component<TVectorStringVector>()
        .member<std::vector<int>>("v1")
        .member<std::string>("s")
        .member<std::vector<int>>("v2");

    TVectorStringVector v = {{1, 2, 3}, "hello world", {4, 5, 6}};
    flecs::string json = ecs.to_json(&v);
    test_str(json, "{\"v1\":[1, 2, 3], \"s\":\"hello world\", \"v2\":[4, 5, 6]}");
}

void Meta_type_w_std_vector_std_vector_std_string() {
    flecs::world ecs;

    ecs.component<std::vector<int>>()
        .serialize(ecs.vector<int>(), Vector_serialize<int>);

    ecs.component<std::string>().serialize(flecs::String, String_serialize);

    ecs.component<TVectorVectorString>()
        .member<std::vector<int>>("v1")
        .member<std::vector<int>>("v2")
        .member<std::string>("s");

    TVectorVectorString v = {{1, 2, 3}, {4, 5, 6}, "hello world"};
    flecs::string json = ecs.to_json(&v);
    test_str(json, "{\"v1\":[1, 2, 3], \"v2\":[4, 5, 6], \"s\":\"hello world\"}");
}

void Meta_primitive_type() {
    flecs::world ecs;

    flecs::entity t = ecs.primitive(flecs::meta::I32);
    test_assert(t != 0);

    test_assert(t.has<flecs::Component>());
    test_assert(t.has<flecs::MetaType>());
    test_assert(t.has<flecs::Primitive>());

    const flecs::Component *c = t.get<flecs::Component>();
    test_assert(c != nullptr);
    test_int(c->size, 4);
    test_int(c->alignment, 4);

    const flecs::MetaType *mt = t.get<flecs::MetaType>();
    test_assert(mt != nullptr);
    test_assert(mt->kind == flecs::meta::PrimitiveType);

    const flecs::Primitive *pt = t.get<flecs::Primitive>();
    test_assert(pt != nullptr);
    test_assert(pt->kind == flecs::meta::I32);
}

void Meta_array_type() {
    flecs::world ecs;

    flecs::entity t = ecs.array(flecs::I32, 3);
    test_assert(t != 0);

    test_assert(t.has<flecs::Component>());
    test_assert(t.has<flecs::MetaType>());
    test_assert(t.has<flecs::Array>());

    const flecs::Component *c = t.get<flecs::Component>();
    test_assert(c != nullptr);
    test_int(c->size, 3 * 4);
    test_int(c->alignment, 4);

    const flecs::MetaType *mt = t.get<flecs::MetaType>();
    test_assert(mt != nullptr);
    test_assert(mt->kind == flecs::meta::ArrayType);

    const flecs::Array *at = t.get<flecs::Array>();
    test_assert(at != nullptr);
    test_assert(at->type == ecs.id<int32_t>());
    test_int(at->count, 3);
}

void Meta_vector_type() {
    flecs::world ecs;

    flecs::entity t = ecs.vector(flecs::I32);
    test_assert(t != 0);

    test_assert(t.has<flecs::Component>());
    test_assert(t.has<flecs::MetaType>());
    test_assert(t.has<flecs::Vector>());

    const flecs::Component *c = t.get<flecs::Component>();
    test_assert(c != nullptr);
    test_int(c->size, 8);
    test_int(c->alignment, 8);

    const flecs::MetaType *mt = t.get<flecs::MetaType>();
    test_assert(mt != nullptr);
    test_assert(mt->kind == flecs::meta::VectorType);

    const flecs::Vector *vt = t.get<flecs::Vector>();
    test_assert(vt != nullptr);
    test_assert(vt->type == ecs.id<int32_t>());
}
