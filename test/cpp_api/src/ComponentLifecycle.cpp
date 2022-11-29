// Catch static asserts
#define flecs_static_assert(cond, str)\
    ecs_assert(cond, ECS_INVALID_OPERATION, str)

#include <cpp_api.h>

int Pod::ctor_invoked = 0;
int Pod::dtor_invoked = 0;
int Pod::copy_invoked = 0;
int Pod::move_invoked = 0;
int Pod::copy_ctor_invoked = 0;
int Pod::move_ctor_invoked = 0;

int CountNoDefaultCtor::ctor_invoked = 0;
int CountNoDefaultCtor::dtor_invoked = 0;
int CountNoDefaultCtor::copy_invoked = 0;
int CountNoDefaultCtor::move_invoked = 0;
int CountNoDefaultCtor::copy_ctor_invoked = 0;
int CountNoDefaultCtor::move_ctor_invoked = 0;

void ComponentLifecycle_ctor_on_add() {
    flecs::world world;

    world.component<Pod>();

    auto e = world.entity().add<Pod>();
    test_assert(e.id() != 0);
    test_assert(e.has<Pod>());

    const Pod *pod = e.get<Pod>();
    test_assert(pod != NULL);
    test_int(Pod::ctor_invoked, 1);
    test_int(Pod::dtor_invoked, 0);
    test_int(Pod::copy_invoked, 0);
    test_int(Pod::move_invoked, 0);
    
    test_int(pod->value, 10);
}

void ComponentLifecycle_dtor_on_remove() {
    flecs::world world;

    world.component<Pod>();

    auto e = world.entity().add<Pod>();
    test_assert(e.id() != 0);
    test_assert(e.has<Pod>());
    test_int(Pod::ctor_invoked, 1);

    e.remove<Pod>();
    test_assert(!e.has<Pod>());
    test_int(Pod::ctor_invoked, 1);
    test_int(Pod::dtor_invoked, 1);
    test_int(Pod::copy_invoked, 0);
    test_int(Pod::move_invoked, 0);
}

void ComponentLifecycle_move_on_add() {
    flecs::world world;

    world.component<Pod>();

    auto e = world.entity().add<Pod>();
    test_assert(e.id() != 0);
    test_assert(e.has<Pod>());

    const Pod *pod = e.get<Pod>();
    test_assert(pod != NULL);
    test_int(Pod::ctor_invoked, 1);
    test_int(Pod::dtor_invoked, 0);
    test_int(Pod::copy_invoked, 0);
    test_int(Pod::move_invoked, 0);
    test_int(Pod::copy_ctor_invoked, 0);
    test_int(Pod::move_ctor_invoked, 0);

    e.add<Position>();

    test_int(Pod::ctor_invoked, 1);
    test_int(Pod::dtor_invoked, 1);
    test_int(Pod::copy_invoked, 0);
    test_int(Pod::move_invoked, 0);
    test_int(Pod::copy_ctor_invoked, 0);
    test_int(Pod::move_ctor_invoked, 1);

    test_int(pod->value, 10);
}

void ComponentLifecycle_move_on_remove() {
    flecs::world world;

    world.component<Pod>();

    auto e = world.entity().add<Position>().add<Pod>();
    test_assert(e.id() != 0);
    test_assert(e.has<Pod>());

    const Pod *pod = e.get<Pod>();
    test_assert(pod != NULL);
    test_int(Pod::ctor_invoked, 1);
    test_int(Pod::dtor_invoked, 0);
    test_int(Pod::copy_invoked, 0);
    test_int(Pod::move_invoked, 0);
    test_int(Pod::copy_ctor_invoked, 0);
    test_int(Pod::move_ctor_invoked, 0);

    e.remove<Position>();

    test_int(Pod::ctor_invoked, 1);
    test_int(Pod::dtor_invoked, 1);
    test_int(Pod::copy_invoked, 0);
    test_int(Pod::move_invoked, 0);
    test_int(Pod::copy_ctor_invoked, 0);
    test_int(Pod::move_ctor_invoked, 1);

    test_int(pod->value, 10);
}

void ComponentLifecycle_copy_on_set() {
    flecs::world world;

    world.component<Pod>();

    auto e = world.entity();
    test_assert(e.id() != 0);

    e.set<Pod>({20});
    test_assert(e.has<Pod>());
    test_int(Pod::ctor_invoked, 2);
    test_int(Pod::dtor_invoked, 1);
    test_int(Pod::copy_invoked, 0);
    test_int(Pod::move_invoked, 1);
}

void ComponentLifecycle_copy_on_override() {
    flecs::world world;

    world.component<Pod>();

    auto base = world.entity();
    test_assert(base.id() != 0);

    base.set<Pod>({10});
    test_int(Pod::ctor_invoked, 2);
    test_int(Pod::dtor_invoked, 1);
    test_int(Pod::copy_invoked, 0);
    Pod::ctor_invoked = 0;
    Pod::dtor_invoked = 0;
    Pod::copy_invoked = 0;
    Pod::move_invoked = 0;

    auto e = world.entity();
    test_assert(e.id() != 0);
    
    e.add(flecs::IsA, base);
    test_int(Pod::ctor_invoked, 0);

    e.add<Pod>();
    test_int(Pod::ctor_invoked, 1);
    test_int(Pod::dtor_invoked, 0);
    test_int(Pod::copy_invoked, 1);
    test_int(Pod::move_invoked, 0);

    const Pod *pod = e.get<Pod>();
    test_assert(pod != NULL);
    test_int(pod->value, 10);
}

void ComponentLifecycle_struct_w_string_add() {
    flecs::world world;

    auto e = world.entity().add<Struct_w_string>();
    test_assert(e.id() != 0);
    test_assert(e.has<Struct_w_string>());

    const Struct_w_string *str = e.get<Struct_w_string>();
    test_assert(str != NULL);
    test_assert(str->value == "");
}

void ComponentLifecycle_struct_w_string_remove() {
    flecs::world world;

    auto e = world.entity().add<Struct_w_string>();
    test_assert(e.id() != 0);
    test_assert(e.has<Struct_w_string>());

    e.remove<Struct_w_string>();
    test_assert(!e.has<Struct_w_string>());
}

void ComponentLifecycle_struct_w_string_set() {
    flecs::world world;

    auto e = world.entity()
        .set<Struct_w_string>({"Hello World"});
    test_assert(e.id() != 0);
    test_assert(e.has<Struct_w_string>());

    const Struct_w_string *str = e.get<Struct_w_string>();
    test_assert(str != NULL);
    test_assert(str->value == "Hello World");
}

void ComponentLifecycle_struct_w_string_override() {
    flecs::world world;

    auto base = world.entity();
    test_assert(base.id() != 0);

    base.set<Struct_w_string>({"Hello World"});

    auto e = world.entity();
    test_assert(e.id() != 0);
    
    e.add(flecs::IsA, base);

    e.add<Struct_w_string>();

    const Struct_w_string *str = e.get<Struct_w_string>();
    test_assert(str != NULL);
    test_assert(str->value == "Hello World");
}

void ComponentLifecycle_struct_w_string_add_2_remove() {
    flecs::world world;

    auto e1 = world.entity().add<Struct_w_string>();
    auto e2 = world.entity().add<Struct_w_string>();

    const Struct_w_string *str1 = e1.get<Struct_w_string>();
    test_assert(str1 != NULL);
    test_assert(str1->value == "");
    const Struct_w_string *str2 = e2.get<Struct_w_string>();
    test_assert(str2 != NULL);
    test_assert(str2->value == "");

    e1.remove<Struct_w_string>();
    str1 = e1.get<Struct_w_string>();
    test_assert(str1 == NULL);

    str2 = e2.get<Struct_w_string>();
    test_assert(str2 != NULL);
    test_assert(str2->value == "");

    e2.remove<Struct_w_string>();
    str2 = e2.get<Struct_w_string>();
    test_assert(str2 == NULL);
}

void ComponentLifecycle_struct_w_string_set_2_remove() {
    flecs::world world;

    auto e1 = world.entity().set<Struct_w_string>({"hello"});
    auto e2 = world.entity().set<Struct_w_string>({"world"});

    const Struct_w_string *str1 = e1.get<Struct_w_string>();
    test_assert(str1 != NULL);
    test_assert(str1->value == "hello");
    const Struct_w_string *str2 = e2.get<Struct_w_string>();
    test_assert(str2 != NULL);
    test_assert(str2->value == "world");

    e1.remove<Struct_w_string>();
    str1 = e1.get<Struct_w_string>();
    test_assert(str1 == NULL);

    str2 = e2.get<Struct_w_string>();
    test_assert(str2 != NULL);
    test_assert(str2->value == "world");

    e2.remove<Struct_w_string>();
    str2 = e2.get<Struct_w_string>();
    test_assert(str2 == NULL);
}

void ComponentLifecycle_struct_w_string_add_2_remove_w_tag() {
    flecs::world world;

    auto e1 = world.entity().add<Tag>().add<Struct_w_string>();
    auto e2 = world.entity().add<Tag>().add<Struct_w_string>();

    const Struct_w_string *str1 = e1.get<Struct_w_string>();
    test_assert(str1 != NULL);
    test_assert(str1->value == "");
    const Struct_w_string *str2 = e2.get<Struct_w_string>();
    test_assert(str2 != NULL);
    test_assert(str2->value == "");

    e1.remove<Struct_w_string>();
    str1 = e1.get<Struct_w_string>();
    test_assert(str1 == NULL);

    str2 = e2.get<Struct_w_string>();
    test_assert(str2 != NULL);
    test_assert(str2->value == "");

    e2.remove<Struct_w_string>();
    str2 = e2.get<Struct_w_string>();
    test_assert(str2 == NULL);
}

void ComponentLifecycle_struct_w_string_set_2_remove_w_tag() {
    flecs::world world;

    auto e1 = world.entity().add<Tag>().set<Struct_w_string>({"hello"});
    auto e2 = world.entity().add<Tag>().set<Struct_w_string>({"world"});

    const Struct_w_string *str1 = e1.get<Struct_w_string>();
    test_assert(str1 != NULL);
    test_assert(str1->value == "hello");
    const Struct_w_string *str2 = e2.get<Struct_w_string>();
    test_assert(str2 != NULL);
    test_assert(str2->value == "world");

    e1.remove<Struct_w_string>();
    str1 = e1.get<Struct_w_string>();
    test_assert(str1 == NULL);

    str2 = e2.get<Struct_w_string>();
    test_assert(str2 != NULL);
    test_assert(str2->value == "world");

    e2.remove<Struct_w_string>();
    str2 = e2.get<Struct_w_string>();
    test_assert(str2 == NULL);
}

void ComponentLifecycle_struct_w_vector_add() {
    flecs::world world;

    auto e = world.entity().add<Struct_w_vector>();
    test_assert(e.has<Struct_w_vector>());

    const Struct_w_vector *ptr = e.get<Struct_w_vector>();
    test_assert(ptr != NULL);
    test_int(ptr->value.size(), 0);
}

void ComponentLifecycle_struct_w_vector_remove() {
    flecs::world world;

    auto e = world.entity().add<Struct_w_vector>();
    test_assert(e.has<Struct_w_vector>());

    e.remove<Struct_w_vector>();
    test_assert(!e.has<Struct_w_vector>());
}

void ComponentLifecycle_struct_w_vector_set() {
    flecs::world world;

    auto e = world.entity().set<Struct_w_vector>({std::vector<int>{1, 2}});
    test_assert(e.has<Struct_w_vector>());

    const Struct_w_vector *ptr = e.get<Struct_w_vector>();
    test_assert(ptr != NULL);
    test_int(ptr->value.size(), 2);
    test_int(ptr->value.at(0), 1);
    test_int(ptr->value.at(1), 2);
}

void ComponentLifecycle_struct_w_vector_override() {
    flecs::world world;

    auto base = world.entity().set<Struct_w_vector>({std::vector<int>{1, 2}});
    test_assert(base.has<Struct_w_vector>());

    auto e = world.entity().is_a(base).add<Struct_w_vector>();
    test_assert(e.has<Struct_w_vector>());

    const Struct_w_vector *ptr = base.get<Struct_w_vector>();
    test_assert(ptr != NULL);
    test_int(ptr->value.size(), 2);
    test_int(ptr->value.at(0), 1);
    test_int(ptr->value.at(1), 2);

    ptr = e.get<Struct_w_vector>();
    test_assert(ptr != NULL);
    test_int(ptr->value.size(), 2);
    test_int(ptr->value.at(0), 1);
    test_int(ptr->value.at(1), 2);
}

void ComponentLifecycle_struct_w_vector_add_2_remove() {
    flecs::world world;

    auto e1 = world.entity().add<Struct_w_vector>();
    auto e2 = world.entity().add<Struct_w_vector>();

    const Struct_w_vector *ptr1 = e1.get<Struct_w_vector>();
    test_assert(ptr1 != NULL);
    test_int(ptr1->value.size(), 0);
    const Struct_w_vector *ptr2 = e2.get<Struct_w_vector>();
    test_assert(ptr2 != NULL);
    test_int(ptr2->value.size(), 0);

    e1.remove<Struct_w_vector>();
    ptr1 = e1.get<Struct_w_vector>();
    test_assert(ptr1 == NULL);

    ptr2 = e2.get<Struct_w_vector>();
    test_assert(ptr2 != NULL);
    test_int(ptr2->value.size(), 0);

    e2.remove<Struct_w_vector>();
    ptr2 = e2.get<Struct_w_vector>();
    test_assert(ptr2 == NULL);
}

void ComponentLifecycle_struct_w_vector_set_2_remove() {
    flecs::world world;

    auto e1 = world.entity().set<Struct_w_vector>({std::vector<int>{1, 2}});
    auto e2 = world.entity().set<Struct_w_vector>({std::vector<int>{3, 4}});

    const Struct_w_vector *ptr1 = e1.get<Struct_w_vector>();
    test_assert(ptr1 != NULL);
    test_int(ptr1->value.size(), 2);
    test_int(ptr1->value.at(0), 1);
    test_int(ptr1->value.at(1), 2);
    const Struct_w_vector *ptr2 = e2.get<Struct_w_vector>();
    test_assert(ptr2 != NULL);
    test_int(ptr2->value.size(), 2);
    test_int(ptr2->value.at(0), 3);
    test_int(ptr2->value.at(1), 4);

    e1.remove<Struct_w_vector>();
    ptr1 = e1.get<Struct_w_vector>();
    test_assert(ptr1 == NULL);

    ptr2 = e2.get<Struct_w_vector>();
    test_assert(ptr2 != NULL);
    test_int(ptr2->value.size(), 2);
    test_int(ptr2->value.at(0), 3);
    test_int(ptr2->value.at(1), 4);

    e2.remove<Struct_w_vector>();
    ptr2 = e2.get<Struct_w_vector>();
    test_assert(ptr2 == NULL);
}

void ComponentLifecycle_struct_w_vector_add_2_remove_w_tag() {
    flecs::world world;

    auto e1 = world.entity().add<Tag>().add<Struct_w_vector>();
    auto e2 = world.entity().add<Tag>().add<Struct_w_vector>();

    const Struct_w_vector *ptr1 = e1.get<Struct_w_vector>();
    test_assert(ptr1 != NULL);
    test_int(ptr1->value.size(), 0);
    const Struct_w_vector *ptr2 = e2.get<Struct_w_vector>();
    test_assert(ptr2 != NULL);
    test_int(ptr2->value.size(), 0);

    e1.remove<Struct_w_vector>();
    ptr1 = e1.get<Struct_w_vector>();
    test_assert(ptr1 == NULL);

    ptr2 = e2.get<Struct_w_vector>();
    test_assert(ptr2 != NULL);
    test_int(ptr2->value.size(), 0);

    e2.remove<Struct_w_vector>();
    ptr2 = e2.get<Struct_w_vector>();
    test_assert(ptr2 == NULL);
}

void ComponentLifecycle_struct_w_vector_set_2_remove_w_tag() {
    flecs::world world;

    auto e1 = world.entity().add<Tag>().set<Struct_w_vector>({std::vector<int>{1, 2}});
    auto e2 = world.entity().add<Tag>().set<Struct_w_vector>({std::vector<int>{3, 4}});

    const Struct_w_vector *ptr1 = e1.get<Struct_w_vector>();
    test_assert(ptr1 != NULL);
    test_int(ptr1->value.size(), 2);
    test_int(ptr1->value.at(0), 1);
    test_int(ptr1->value.at(1), 2);
    const Struct_w_vector *ptr2 = e2.get<Struct_w_vector>();
    test_assert(ptr2 != NULL);
    test_int(ptr2->value.size(), 2);
    test_int(ptr2->value.at(0), 3);
    test_int(ptr2->value.at(1), 4);

    e1.remove<Struct_w_vector>();
    ptr1 = e1.get<Struct_w_vector>();
    test_assert(ptr1 == NULL);

    ptr2 = e2.get<Struct_w_vector>();
    test_assert(ptr2 != NULL);
    test_int(ptr2->value.size(), 2);
    test_int(ptr2->value.at(0), 3);
    test_int(ptr2->value.at(1), 4);

    e2.remove<Struct_w_vector>();
    ptr2 = e2.get<Struct_w_vector>();
    test_assert(ptr2 == NULL);
}

void ComponentLifecycle_get_mut_new() {
    flecs::world world;

    world.component<Pod>();

    auto e = world.entity();
    test_assert(e.id() != 0);

    Pod* value = e.get_mut<Pod>();
    test_assert(value != NULL);

    Pod::ctor_invoked = 1;
    Pod::dtor_invoked = 0;
    Pod::copy_invoked = 0;
    Pod::move_invoked = 0;

    e.modified<Pod>();

    Pod::ctor_invoked = 1;
    Pod::dtor_invoked = 0;
    Pod::copy_invoked = 0;
    Pod::move_invoked = 0;    
}

void ComponentLifecycle_get_mut_existing() {
    flecs::world world;

    world.component<Pod>();

    auto e = world.entity();
    test_assert(e.id() != 0);

    Pod* value = e.get_mut<Pod>();
    test_assert(value != NULL);

    Pod::ctor_invoked = 1;
    Pod::dtor_invoked = 0;
    Pod::copy_invoked = 0;
    Pod::move_invoked = 0;

    value = e.get_mut<Pod>();
    test_assert(value != NULL);

    /* Repeated calls to get_mut should not invoke constructor */
    Pod::ctor_invoked = 1;
    Pod::dtor_invoked = 0;
    Pod::copy_invoked = 0;
    Pod::move_invoked = 0;
}

void ComponentLifecycle_implicit_component() {
    flecs::world world;

    auto e = world.entity().add<Pod>();
    test_assert(e.id() != 0);
    test_assert(e.has<Pod>());
    test_int(Pod::ctor_invoked, 1);

    const Pod *pod = e.get<Pod>();
    test_assert(pod != NULL);

    test_int(pod->value, 10);

    test_int(Pod::ctor_invoked, 1);
    test_int(Pod::dtor_invoked, 0);
    test_int(Pod::copy_invoked, 0);
    test_int(Pod::move_invoked, 0);

    world.entity().add<Pod>();

    test_int(Pod::ctor_invoked, 2);
    test_int(Pod::move_ctor_invoked, 0);
    test_int(Pod::move_invoked, 0);

    world.entity().add<Pod>();

    test_int(Pod::ctor_invoked, 3);
    test_int(Pod::move_ctor_invoked, 2);
    test_int(Pod::move_invoked, 0);
}

void ComponentLifecycle_implicit_after_query() {
    flecs::world world;

    world.query<Pod>();

    auto e = world.entity().add<Pod>();
    test_assert(e.id() != 0);
    test_assert(e.has<Pod>());
    test_int(Pod::ctor_invoked, 1);

    const Pod *pod = e.get<Pod>();
    test_assert(pod != NULL);

    test_int(pod->value, 10);

    test_int(Pod::ctor_invoked, 1);
    test_int(Pod::dtor_invoked, 0);
    test_int(Pod::copy_invoked, 0);
    test_int(Pod::move_invoked, 0);

    world.entity().add<Pod>();

    test_int(Pod::ctor_invoked, 2);
    test_int(Pod::move_ctor_invoked, 0);
    test_int(Pod::move_invoked, 0); 

    world.entity().add<Pod>();

    test_int(Pod::ctor_invoked, 3);
    test_int(Pod::move_ctor_invoked, 2);
    test_int(Pod::move_invoked, 0); 
}

template <typename T>
static void try_add(flecs::world& ecs) {
    flecs::entity e = ecs.entity().add<T>();
    
    test_assert(e.has<T>());

    const T *ptr = e.get<T>();
    test_int(ptr->x_, 99);

    e.remove<T>();
    test_assert(!e.has<T>());
}

template <typename T>
static void try_add_relation(flecs::world& ecs) {
    flecs::entity obj = ecs.entity();

    flecs::entity e = ecs.entity().add<T>(obj);
    test_assert(e.has<T>());

    const T *ptr = e.get<T>();
    test_int(ptr->x_, 89);

    e.remove<T>();
    test_assert(!e.has<T>());
}

template <typename T>
static void try_add_second(flecs::world& ecs) {
    flecs::entity rel = ecs.entity();

    flecs::entity e = ecs.entity().add_second<T>(rel);
    test_assert(e.has<T>());

    const T *ptr = e.get<T>();
    test_int(ptr->x_, 89);

    e.remove<T>();
    test_assert(!e.has<T>());
}

template <typename T>
static void try_set(flecs::world& ecs) {
    flecs::entity e = ecs.entity().set<T>({10});

    const T *ptr = e.get<T>();
    test_int(ptr->x_, 10);
}

template <typename T>
static void try_emplace(flecs::world& ecs) {
    flecs::entity e = ecs.entity().emplace<T>(10);

    const T *ptr = e.get<T>();
    test_int(ptr->x_, 10);
}

template <typename T>
static void try_set_default(flecs::world& ecs) {
    flecs::entity e = ecs.entity().set(T());

    const T *ptr = e.get<T>();
    test_int(ptr->x_, 99);

    e.remove<T>();
}

void ComponentLifecycle_deleted_copy() {
    flecs::world ecs;

    ecs.component<NoCopy>();

    try_add<NoCopy>(ecs);

    try_set<NoCopy>(ecs);
}

void ComponentLifecycle_no_default_ctor_emplace() {
    flecs::world ecs;

    ecs.component<NoDefaultCtor>();

    try_emplace<NoDefaultCtor>(ecs);
}

void ComponentLifecycle_default_init() {
    flecs::world ecs;

    ecs.component<DefaultInit>();

    try_add<DefaultInit>(ecs);

    try_set<DefaultInit>(ecs);   
}

void ComponentLifecycle_no_default_ctor_add() {
    install_test_abort();

    flecs::world ecs;

    ecs.component<NoDefaultCtor>();

    test_expect_abort();

    try_add<NoDefaultCtor>(ecs);  
}

void ComponentLifecycle_no_default_ctor_add_relation() {
    install_test_abort();

    flecs::world ecs;

    ecs.component<NoDefaultCtor>();

    test_expect_abort();

    try_add_relation<NoDefaultCtor>(ecs); 
}

void ComponentLifecycle_no_default_ctor_add_second() {
    install_test_abort();

    flecs::world ecs;

    ecs.component<NoDefaultCtor>();

    test_expect_abort();

    try_add_second<NoDefaultCtor>(ecs);
}

void ComponentLifecycle_no_default_ctor_set() {
    install_test_abort();

    flecs::world ecs;

    ecs.component<NoDefaultCtor>();

    test_expect_abort();

    try_set<NoDefaultCtor>(ecs);  
}

void ComponentLifecycle_no_copy_ctor() {
    flecs::world ecs;

    ecs.component<NoCopyCtor>();

    try_add<NoCopyCtor>(ecs);

    try_set<NoCopyCtor>(ecs); 
}

void ComponentLifecycle_no_move_ctor() {
    install_test_abort();

    flecs::world ecs;

    test_expect_abort();

    ecs.component<NoMoveCtor>();
}

void ComponentLifecycle_no_copy_assign() {
    flecs::world ecs;

    ecs.component<NoCopyAssign>();

    try_add<NoCopyAssign>(ecs);

    try_set<NoCopyAssign>(ecs);
}

void ComponentLifecycle_no_move_assign() {
    install_test_abort();

    flecs::world ecs;

    test_expect_abort();

    ecs.component<NoMoveAssign>();
}

void ComponentLifecycle_no_copy() {
    flecs::world ecs;

    ecs.component<NoCopy>();

    try_add<NoCopy>(ecs);

    try_set<NoCopy>(ecs);
}

void ComponentLifecycle_no_move() {
    install_test_abort();

    flecs::world ecs;

    test_expect_abort();

    ecs.component<NoMove>();
}

void ComponentLifecycle_no_dtor() {
    install_test_abort();

    flecs::world ecs;

    test_expect_abort();

    ecs.component<NoDtor>();
}

void ComponentLifecycle_default_ctor_w_value_ctor() {
    flecs::world ecs;

    ecs.component<DefaultCtorValueCtor>();

    try_add<DefaultCtorValueCtor>(ecs);

    try_set<DefaultCtorValueCtor>(ecs);

    try_set_default<FlecsCtorDefaultCtor>(ecs);
}

void ComponentLifecycle_no_default_ctor_move_ctor_on_set() {
    flecs::world ecs;

    ecs.component<CountNoDefaultCtor>();

    // Emplace, construct
    auto e = ecs.entity().emplace<CountNoDefaultCtor>(10);
    test_assert(e.has<CountNoDefaultCtor>());

    const CountNoDefaultCtor* ptr = e.get<CountNoDefaultCtor>();
    test_assert(ptr != NULL);
    test_int(ptr->value, 10);

    test_int(CountNoDefaultCtor::ctor_invoked, 1);
    test_int(CountNoDefaultCtor::dtor_invoked, 0);
    test_int(CountNoDefaultCtor::copy_invoked, 0);
    test_int(CountNoDefaultCtor::move_invoked, 0);
    test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
    test_int(CountNoDefaultCtor::move_ctor_invoked, 0);

    // Set, move assign
    e.set<CountNoDefaultCtor>({10});

    test_int(CountNoDefaultCtor::ctor_invoked, 2);
    test_int(CountNoDefaultCtor::dtor_invoked, 1);
    test_int(CountNoDefaultCtor::copy_invoked, 0);
    test_int(CountNoDefaultCtor::move_invoked, 1);
    test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
    test_int(CountNoDefaultCtor::move_ctor_invoked, 0);    
}

void ComponentLifecycle_emplace_w_ctor() {
    flecs::world ecs;

    auto e = ecs.entity()
        .emplace<Pod>(10);

    test_int(Pod::ctor_invoked, 1);
    test_int(Pod::dtor_invoked, 0);

    const Pod *ptr = e.get<Pod>();
    test_assert(ptr != NULL);
    test_int(ptr->value, 10);

    test_int(Pod::ctor_invoked, 1);
    test_int(Pod::dtor_invoked, 0);    
}

void ComponentLifecycle_emplace_no_default_ctor() {
    flecs::world ecs;

    auto e = ecs.entity()
        .emplace<CountNoDefaultCtor>(10);

    test_int(CountNoDefaultCtor::ctor_invoked, 1);
    test_int(CountNoDefaultCtor::dtor_invoked, 0);

    const CountNoDefaultCtor *ptr = e.get<CountNoDefaultCtor>();
    test_assert(ptr != NULL);
    test_int(ptr->value, 10);

    test_int(CountNoDefaultCtor::ctor_invoked, 1);
    test_int(CountNoDefaultCtor::dtor_invoked, 0); 
}

void ComponentLifecycle_emplace_defer_use_move_ctor() {
    flecs::world ecs;

    auto e = ecs.entity();

    ecs.defer_begin();
    e.emplace<CountNoDefaultCtor>(10);
    test_assert(!e.has<CountNoDefaultCtor>());
    test_int(CountNoDefaultCtor::ctor_invoked, 1);
    test_int(CountNoDefaultCtor::dtor_invoked, 0);
    test_int(CountNoDefaultCtor::move_invoked, 0);
    test_int(CountNoDefaultCtor::move_ctor_invoked, 0);
    ecs.defer_end();

    test_assert(e.has<CountNoDefaultCtor>());
    test_int(CountNoDefaultCtor::ctor_invoked, 1);
    test_int(CountNoDefaultCtor::dtor_invoked, 0);
    test_int(CountNoDefaultCtor::move_invoked, 0);
    test_int(CountNoDefaultCtor::move_ctor_invoked, 1);

    const CountNoDefaultCtor *ptr = e.get<CountNoDefaultCtor>();
    test_assert(ptr != NULL);
    test_int(ptr->value, 10);

    test_int(CountNoDefaultCtor::ctor_invoked, 1);
    test_int(CountNoDefaultCtor::dtor_invoked, 0);
    test_int(CountNoDefaultCtor::move_invoked, 0);
    test_int(CountNoDefaultCtor::move_ctor_invoked, 1);
}

void ComponentLifecycle_emplace_existing() {
    install_test_abort();

    flecs::world ecs;

    auto e = ecs.entity()
        .emplace<Pod>(10);

    const Pod *ptr = e.get<Pod>();
    test_assert(ptr != NULL);
    test_int(ptr->value, 10);    

    test_int(Pod::ctor_invoked, 1);
    test_int(Pod::dtor_invoked, 0);

    test_expect_abort();
    e.emplace<Pod>(20);
}

void ComponentLifecycle_emplace_singleton() {
    flecs::world ecs;

    ecs.emplace<Pod>(10);

    test_int(Pod::ctor_invoked, 1);
    test_int(Pod::dtor_invoked, 0);

    const Pod *ptr = ecs.get<Pod>();
    test_assert(ptr != NULL);
    test_int(ptr->value, 10);

    test_int(Pod::ctor_invoked, 1);
    test_int(Pod::dtor_invoked, 0); 
}

class CtorDtorNonTrivial {
public:
    CtorDtorNonTrivial(int x) : x_(x) {
        ctor_invoked ++;
    }

    ~CtorDtorNonTrivial() {
        dtor_invoked ++;
        dtor_value = x_;
    }

    int x_;
    std::string str_;

    static int ctor_invoked;
    static int dtor_invoked;
    static int dtor_value;
};

int CtorDtorNonTrivial::ctor_invoked;
int CtorDtorNonTrivial::dtor_invoked;
int CtorDtorNonTrivial::dtor_value;

void ComponentLifecycle_dtor_w_non_trivial_implicit_move() {
    flecs::world ecs;

    test_bool(std::is_trivially_move_assignable<CtorDtorNonTrivial>::value, false);
    test_bool(std::is_move_assignable<CtorDtorNonTrivial>::value, true);

    auto e_1 = ecs.entity().emplace<CtorDtorNonTrivial>(10);
    auto e_2 = ecs.entity().emplace<CtorDtorNonTrivial>(20);

    const CtorDtorNonTrivial *ptr = e_1.get<CtorDtorNonTrivial>();
    test_assert(ptr != nullptr);
    test_int(ptr->x_, 10);

    ptr = e_2.get<CtorDtorNonTrivial>();
    test_assert(ptr != nullptr);
    test_int(ptr->x_, 20);

    test_int(CtorDtorNonTrivial::ctor_invoked, 2);

    // Moves e_2 to e_1
    e_1.destruct();

    test_int(CtorDtorNonTrivial::ctor_invoked, 2);
    test_int(CtorDtorNonTrivial::dtor_invoked, 1);

    // Counter intuitive but correct. The class is not trivially movable, so
    // the move assignment should take care of cleaning up e_1 (10). That still
    // leaves the original e_2 which was moved from, but not destructed.
    //
    // In a real application the class should probably implement its own move
    // assignment to ensure correct destructor behavior.
    test_int(CtorDtorNonTrivial::dtor_value, 20);
}

class CtorDtor_w_MoveAssign {
public:
    CtorDtor_w_MoveAssign(int x) : x_(x) {
        ctor_invoked ++;
    }

    ~CtorDtor_w_MoveAssign() {
        dtor_invoked ++;
        dtor_value = x_;
    }

    CtorDtor_w_MoveAssign(const CtorDtor_w_MoveAssign& obj) = default;
    CtorDtor_w_MoveAssign(CtorDtor_w_MoveAssign&& obj) = default;
    CtorDtor_w_MoveAssign& operator=(const CtorDtor_w_MoveAssign& obj) = default;

    CtorDtor_w_MoveAssign& operator=(CtorDtor_w_MoveAssign&& obj) {
        move_value = this->x_;

        this->x_ = obj.x_;
        obj.x_ = 0;
        return *this;
    }    

    int x_;
    std::string str_;

    static int ctor_invoked;
    static int dtor_invoked;
    static int dtor_value;
    static int move_value;
};

int CtorDtor_w_MoveAssign::ctor_invoked;
int CtorDtor_w_MoveAssign::dtor_invoked;
int CtorDtor_w_MoveAssign::dtor_value;
int CtorDtor_w_MoveAssign::move_value;

void ComponentLifecycle_dtor_w_non_trivial_explicit_move() {
    flecs::world ecs;

    test_bool(std::is_trivially_move_assignable<CtorDtor_w_MoveAssign>::value, false);
    test_bool(std::is_move_assignable<CtorDtor_w_MoveAssign>::value, true);

    auto e_1 = ecs.entity().emplace<CtorDtor_w_MoveAssign>(10);
    auto e_2 = ecs.entity().emplace<CtorDtor_w_MoveAssign>(20);

    const CtorDtor_w_MoveAssign *ptr = e_1.get<CtorDtor_w_MoveAssign>();
    test_assert(ptr != nullptr);
    test_int(ptr->x_, 10);

    ptr = e_2.get<CtorDtor_w_MoveAssign>();
    test_assert(ptr != nullptr);
    test_int(ptr->x_, 20);

    test_int(CtorDtor_w_MoveAssign::ctor_invoked, 2);

    // Moves e_2 to e_1
    e_1.destruct();

    test_int(CtorDtor_w_MoveAssign::ctor_invoked, 2);
    test_int(CtorDtor_w_MoveAssign::dtor_invoked, 1);

    test_int(CtorDtor_w_MoveAssign::move_value, 10);
    test_int(CtorDtor_w_MoveAssign::dtor_value, 0);
}

void ComponentLifecycle_grow_no_default_ctor() {
	{
        flecs::world world;

        world.component<CountNoDefaultCtor>();

        auto e1 = world.entity().emplace<CountNoDefaultCtor>(1);
        auto e2 = world.entity().emplace<CountNoDefaultCtor>(2);

        test_int(CountNoDefaultCtor::ctor_invoked, 2);
        test_int(CountNoDefaultCtor::dtor_invoked, 0);
        test_int(CountNoDefaultCtor::copy_invoked, 0);
        test_int(CountNoDefaultCtor::move_invoked, 0);
        test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
        test_int(CountNoDefaultCtor::move_ctor_invoked, 0);

        auto e3 = world.entity().emplace<CountNoDefaultCtor>(3);
        
        test_int(CountNoDefaultCtor::ctor_invoked, 3);
        test_int(CountNoDefaultCtor::dtor_invoked, 2);
        test_int(CountNoDefaultCtor::copy_invoked, 0);
        test_int(CountNoDefaultCtor::move_invoked, 0);
        test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
        test_int(CountNoDefaultCtor::move_ctor_invoked, 2);

        test_assert(e1.has<CountNoDefaultCtor>());
        test_assert(e2.has<CountNoDefaultCtor>());
        test_assert(e3.has<CountNoDefaultCtor>());

        test_int(e1.get<CountNoDefaultCtor>()->value, 1);
        test_int(e2.get<CountNoDefaultCtor>()->value, 2);
        test_int(e3.get<CountNoDefaultCtor>()->value, 3);
    }

    test_int(CountNoDefaultCtor::ctor_invoked, 3);
    test_int(CountNoDefaultCtor::dtor_invoked, 5);
    test_int(CountNoDefaultCtor::copy_invoked, 0);
    test_int(CountNoDefaultCtor::move_invoked, 0);
    test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
    test_int(CountNoDefaultCtor::move_ctor_invoked, 2);
}

void ComponentLifecycle_grow_no_default_ctor_move() {
    {
        flecs::world world;

        world.component<CountNoDefaultCtor>();
        world.component<Tag>();

        auto e1 = world.entity().emplace<CountNoDefaultCtor>(1);
        auto e2 = world.entity().emplace<CountNoDefaultCtor>(2);

        test_int(CountNoDefaultCtor::ctor_invoked, 2);
        test_int(CountNoDefaultCtor::dtor_invoked, 0);
        test_int(CountNoDefaultCtor::copy_invoked, 0);
        test_int(CountNoDefaultCtor::move_invoked, 0);
        test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
        test_int(CountNoDefaultCtor::move_ctor_invoked, 0);

        CountNoDefaultCtor::reset();
        auto e3 = world.entity().emplace<CountNoDefaultCtor>(3);

        test_int(CountNoDefaultCtor::ctor_invoked, 1);
        test_int(CountNoDefaultCtor::dtor_invoked, 2);
        test_int(CountNoDefaultCtor::copy_invoked, 0);
        test_int(CountNoDefaultCtor::move_invoked, 0);
        test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
        test_int(CountNoDefaultCtor::move_ctor_invoked, 2);

        test_assert(e1.has<CountNoDefaultCtor>());
        test_assert(e2.has<CountNoDefaultCtor>());
        test_assert(e3.has<CountNoDefaultCtor>());

        test_int(e1.get<CountNoDefaultCtor>()->value, 1);
        test_int(e2.get<CountNoDefaultCtor>()->value, 2);
        test_int(e3.get<CountNoDefaultCtor>()->value, 3);

        CountNoDefaultCtor::reset();
        e1.add<Tag>();

        test_int(CountNoDefaultCtor::ctor_invoked, 0);
        test_int(CountNoDefaultCtor::dtor_invoked, 1);
        test_int(CountNoDefaultCtor::copy_invoked, 0);
        test_int(CountNoDefaultCtor::move_invoked, 1);
        test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
        test_int(CountNoDefaultCtor::move_ctor_invoked, 1);

        CountNoDefaultCtor::reset();
        e2.add<Tag>();

        test_int(CountNoDefaultCtor::ctor_invoked, 0);
        test_int(CountNoDefaultCtor::dtor_invoked, 1);
        test_int(CountNoDefaultCtor::copy_invoked, 0);
        test_int(CountNoDefaultCtor::move_invoked, 0);
        test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
        test_int(CountNoDefaultCtor::move_ctor_invoked, 1);

        CountNoDefaultCtor::reset();
        e3.add<Tag>();

        test_int(CountNoDefaultCtor::ctor_invoked, 0);
        test_int(CountNoDefaultCtor::dtor_invoked, 3);
        test_int(CountNoDefaultCtor::copy_invoked, 0);
        test_int(CountNoDefaultCtor::move_invoked, 0);
        test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
        test_int(CountNoDefaultCtor::move_ctor_invoked, 3);

        CountNoDefaultCtor::reset();
    }

    test_int(CountNoDefaultCtor::ctor_invoked, 0);
    test_int(CountNoDefaultCtor::dtor_invoked, 3);
    test_int(CountNoDefaultCtor::copy_invoked, 0);
    test_int(CountNoDefaultCtor::move_invoked, 0);
    test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
    test_int(CountNoDefaultCtor::move_ctor_invoked, 0);
}

void ComponentLifecycle_grow_no_default_ctor_move_w_component() {
    {
        flecs::world world;

        world.component<CountNoDefaultCtor>();
        world.component<Position>();

        auto e1 = world.entity().emplace<CountNoDefaultCtor>(1);
        auto e2 = world.entity().emplace<CountNoDefaultCtor>(2);

        test_int(CountNoDefaultCtor::ctor_invoked, 2);
        test_int(CountNoDefaultCtor::dtor_invoked, 0);
        test_int(CountNoDefaultCtor::copy_invoked, 0);
        test_int(CountNoDefaultCtor::move_invoked, 0);
        test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
        test_int(CountNoDefaultCtor::move_ctor_invoked, 0);

        CountNoDefaultCtor::reset();
        auto e3 = world.entity().emplace<CountNoDefaultCtor>(3);

        test_int(CountNoDefaultCtor::ctor_invoked, 1);
        test_int(CountNoDefaultCtor::dtor_invoked, 2);
        test_int(CountNoDefaultCtor::copy_invoked, 0);
        test_int(CountNoDefaultCtor::move_invoked, 0);
        test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
        test_int(CountNoDefaultCtor::move_ctor_invoked, 2);

        test_assert(e1.has<CountNoDefaultCtor>());
        test_assert(e2.has<CountNoDefaultCtor>());
        test_assert(e3.has<CountNoDefaultCtor>());

        test_int(e1.get<CountNoDefaultCtor>()->value, 1);
        test_int(e2.get<CountNoDefaultCtor>()->value, 2);
        test_int(e3.get<CountNoDefaultCtor>()->value, 3);

        CountNoDefaultCtor::reset();
        e1.add<Position>();

        test_int(CountNoDefaultCtor::ctor_invoked, 0);
        test_int(CountNoDefaultCtor::dtor_invoked, 1);
        test_int(CountNoDefaultCtor::copy_invoked, 0);
        test_int(CountNoDefaultCtor::move_invoked, 1);
        test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
        test_int(CountNoDefaultCtor::move_ctor_invoked, 1);

        CountNoDefaultCtor::reset();
        e2.add<Position>(); // e2 is last element, e3 got moved to e1's location

        test_int(CountNoDefaultCtor::ctor_invoked, 0);
        test_int(CountNoDefaultCtor::dtor_invoked, 1);
        test_int(CountNoDefaultCtor::copy_invoked, 0);
        test_int(CountNoDefaultCtor::move_invoked, 0);
        test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
        test_int(CountNoDefaultCtor::move_ctor_invoked, 1);

        CountNoDefaultCtor::reset();
        e3.add<Position>();

        test_int(CountNoDefaultCtor::ctor_invoked, 0);
        test_int(CountNoDefaultCtor::dtor_invoked, 3); /* 2 (e1,e2) for resize, 1 (e3) for moved away storage */
        test_int(CountNoDefaultCtor::copy_invoked, 0);
        test_int(CountNoDefaultCtor::move_invoked, 0);
        test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
        test_int(CountNoDefaultCtor::move_ctor_invoked, 3); /* resize */

        CountNoDefaultCtor::reset();
    }

    test_int(CountNoDefaultCtor::ctor_invoked, 0);
    test_int(CountNoDefaultCtor::dtor_invoked, 3);
    test_int(CountNoDefaultCtor::copy_invoked, 0);
    test_int(CountNoDefaultCtor::move_invoked, 0);
    test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
    test_int(CountNoDefaultCtor::move_ctor_invoked, 0);
}

void ComponentLifecycle_delete_no_default_ctor() {
	{
        flecs::world world;

        world.component<CountNoDefaultCtor>();

        auto e1 = world.entity().emplace<CountNoDefaultCtor>(1);
        auto e2 = world.entity().emplace<CountNoDefaultCtor>(2);
        auto e3 = world.entity().emplace<CountNoDefaultCtor>(3);
        
        test_int(CountNoDefaultCtor::ctor_invoked, 3);
        test_int(CountNoDefaultCtor::dtor_invoked, 2);
        test_int(CountNoDefaultCtor::copy_invoked, 0);
        test_int(CountNoDefaultCtor::move_invoked, 0);
        test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
        test_int(CountNoDefaultCtor::move_ctor_invoked, 2);

        test_assert(e1.has<CountNoDefaultCtor>());
        test_assert(e2.has<CountNoDefaultCtor>());
        test_assert(e3.has<CountNoDefaultCtor>());

        test_int(e1.get<CountNoDefaultCtor>()->value, 1);
        test_int(e2.get<CountNoDefaultCtor>()->value, 2);
        test_int(e3.get<CountNoDefaultCtor>()->value, 3);

        e2.destruct();

        test_int(CountNoDefaultCtor::ctor_invoked, 3);
        test_int(CountNoDefaultCtor::dtor_invoked, 3);
        test_int(CountNoDefaultCtor::copy_invoked, 0);
        test_int(CountNoDefaultCtor::move_invoked, 1);
        test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
        test_int(CountNoDefaultCtor::move_ctor_invoked, 2);
    }

    test_int(CountNoDefaultCtor::ctor_invoked, 3);
    test_int(CountNoDefaultCtor::dtor_invoked, 5);
    test_int(CountNoDefaultCtor::copy_invoked, 0);
    test_int(CountNoDefaultCtor::move_invoked, 1);
    test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
    test_int(CountNoDefaultCtor::move_ctor_invoked, 2);
}

void ComponentLifecycle_on_add_hook() {
    int count = 0;

    {
        flecs::world ecs;

        ecs.component<Position>().on_add([&](Position& p) {
            count ++;
        });

        test_int(0, count);

        auto e = ecs.entity().add<Position>();
        test_int(1, count);

        e.add<Position>();
        test_int(1, count);
    }

    test_int(1, count);
}

void ComponentLifecycle_on_remove_hook() {
    int count = 0;
    
    {
        flecs::world ecs;

        ecs.component<Position>().on_remove([&](Position& p) {
            count ++;
        });

        test_int(0, count);

        auto e1 = ecs.entity().add<Position>();
        ecs.entity().add<Position>();
        test_int(0, count);

        e1.remove<Position>();
        test_int(1, count);
    }

    test_int(2, count);
}

void ComponentLifecycle_on_set_hook() {
    int count = 0;
    Position v = {0};

    {
        flecs::world ecs;

        ecs.component<Position>().on_set([&](Position& p) {
            count ++;
            v = p;
        });

        test_int(0, count);

        auto e1 = ecs.entity().add<Position>();
        test_int(0, count);

        e1.set<Position>({10, 20});
        test_int(1, count);
        test_int(10, v.x);
        test_int(20, v.y);

        ecs.entity().set<Position>({30, 40});
        test_int(2, count);
        test_int(30, v.x);
        test_int(40, v.y);
    }

    test_int(2, count);
}

void ComponentLifecycle_on_add_hook_w_entity() {
    int count = 0;
    flecs::entity e_arg;

    {
        flecs::world ecs;

        ecs.component<Position>().on_add([&](flecs::entity arg, Position& p) {
            e_arg = arg;
            count ++;
        });

        test_int(0, count);
        test_assert(e_arg == 0);

        auto e = ecs.entity().add<Position>();
        test_int(1, count);
        test_assert(e_arg == e);

        e.add<Position>();
        test_int(1, count);
    }

    test_int(1, count);
}

void ComponentLifecycle_on_remove_hook_w_entity() {
    int count = 0;
    flecs::entity e_arg;
    flecs::entity e2;
    
    {
        flecs::world ecs;

        ecs.component<Position>().on_remove([&](flecs::entity arg, Position& p){
            e_arg = arg;
            count ++;
        });

        test_int(0, count);
        test_assert(e_arg == 0);

        auto e1 = ecs.entity().add<Position>();
        e2 = ecs.entity().add<Position>();
        test_int(0, count);

        e1.remove<Position>();
        test_int(1, count);
        test_assert(e_arg == e1);
    }

    test_int(2, count);
    test_assert(e_arg == e2);
}

void ComponentLifecycle_on_set_hook_w_entity() {
    int count = 0;
    Position v = {0};
    flecs::entity e_arg;

    {
        flecs::world ecs;

        ecs.component<Position>().on_set([&](flecs::entity arg, Position& p) {
            count ++;
            v = p;
            e_arg = arg;
        });

        test_int(0, count);

        auto e1 = ecs.entity().add<Position>();
        test_int(0, count);

        e1.set<Position>({10, 20});
        test_int(1, count);
        test_assert(e_arg == e1);
        test_int(10, v.x);
        test_int(20, v.y);

        auto e2 = ecs.entity().set<Position>({30, 40});
        test_int(2, count);
        test_assert(e_arg == e2);
        test_int(30, v.x);
        test_int(40, v.y);
    }

    test_int(2, count);
}

void ComponentLifecycle_chained_hooks() {
    flecs::world ecs;

    int32_t add_count = 0;
    int32_t remove_count = 0;
    int32_t set_count = 0;

    ecs.component<Position>()
        .on_add([&](Position& p){
            add_count ++;
        })
        .on_set([&](Position& p){
            set_count ++;
        })
        .on_remove([&](Position& p){
            remove_count ++;
        });

    auto e = ecs.entity();
    test_int(0, add_count);
    test_int(0, set_count);
    test_int(0, remove_count);

    e.add<Position>();
    test_int(1, add_count);
    test_int(0, set_count);
    test_int(0, remove_count);

    e.set<Position>({10, 20});
    test_int(1, add_count);
    test_int(1, set_count);
    test_int(0, remove_count);

    e.remove<Position>();
    test_int(1, add_count);
    test_int(1, set_count);
    test_int(1, remove_count);
}

void ComponentLifecycle_ctor_w_2_worlds() {
    {
        flecs::world ecs;

        test_int(Pod::ctor_invoked, 0);

        ecs.entity().add<Pod>();
        test_int(Pod::ctor_invoked, 1);
    }

    Pod::ctor_invoked = 0;

    {
        flecs::world ecs;

        test_int(Pod::ctor_invoked, 0);

        ecs.entity().add<Pod>();
        test_int(Pod::ctor_invoked, 1);
    }
}

void ComponentLifecycle_ctor_w_2_worlds_explicit_registration() {
    {
        flecs::world ecs;

        ecs.component<Pod>();
        test_int(Pod::ctor_invoked, 0);

        ecs.entity().add<Pod>();
        test_int(Pod::ctor_invoked, 1);
    }

    Pod::ctor_invoked = 0;

    {
        flecs::world ecs;

        ecs.component<Pod>();
        test_int(Pod::ctor_invoked, 0);

        ecs.entity().add<Pod>();
        test_int(Pod::ctor_invoked, 1);
    }
}

struct DeferEmplaceTest {
    double x, y;

    DeferEmplaceTest(double x_, double y_) {
        x = x_;
        y = y_;
    }
};

void ComponentLifecycle_defer_emplace() {
    flecs::world ecs;

    flecs::entity e = ecs.entity();

    ecs.defer_begin();
    e.emplace<DeferEmplaceTest>(10.0, 20.0);
    test_assert(!e.has<DeferEmplaceTest>());
    ecs.defer_end();
    test_assert(e.has<DeferEmplaceTest>());

    const DeferEmplaceTest *p = e.get<DeferEmplaceTest>();
    test_assert(p != NULL);
    test_int(p->x, 10);
    test_int(p->y, 20);
}

void ComponentLifecycle_emplace_w_on_add() {
    flecs::world ecs;

    flecs::entity e1 = ecs.entity();

    int on_add = 0;
    ecs.component<Position>()
        .on_add([&](flecs::entity e, Position&) {
            on_add = true;
            test_assert(e == e1);
        });

    e1.emplace<Position>();
    test_int(on_add, 1);
}

void ComponentLifecycle_emplace_w_on_add_existing() {
    flecs::world ecs;

    flecs::entity e1 = ecs.entity().add<Velocity>();
    
    int on_add = 0;
    ecs.component<Position>()
        .on_add([&](flecs::entity e, Position&) {
            on_add = true;
            test_assert(e == e1);
        });

    e1.emplace<Position>();
    test_int(on_add, 1);
}

void ComponentLifecycle_set_pair_no_copy() {
    flecs::world ecs;

    struct Tag { };

    flecs::entity e = ecs.entity()
        .set<NoCopy, Tag>({ 10 });

    const NoCopy *ptr = e.get<NoCopy, Tag>();
    test_assert(ptr != NULL);
    test_int(ptr->x_, 10);
}

void ComponentLifecycle_set_pair_w_entity_no_copy() {
    flecs::world ecs;

    flecs::entity tag = ecs.entity();

    flecs::entity e = ecs.entity()
        .set<NoCopy>(tag, { 10 });

    const NoCopy *ptr = e.get<NoCopy>(tag);
    test_assert(ptr != NULL);
    test_int(ptr->x_, 10);
}

void ComponentLifecycle_set_pair_second_no_copy() {
    flecs::world ecs;

    flecs::entity tag = ecs.entity();

    flecs::entity e = ecs.entity()
        .set_second<NoCopy>(tag, { 10 });

    const NoCopy *ptr = e.get_second<NoCopy>(tag);
    test_assert(ptr != NULL);
    test_int(ptr->x_, 10);
}

void ComponentLifecycle_set_override_no_copy() {
    flecs::world ecs;

    flecs::entity e = ecs.entity()
        .set_override<NoCopy>({ 10 });

    const NoCopy *ptr = e.get<NoCopy>();
    test_assert(ptr != NULL);
    test_int(ptr->x_, 10);

    test_assert(e.has(flecs::Override | ecs.id<NoCopy>()));
}

void ComponentLifecycle_set_override_pair_no_copy() {
    flecs::world ecs;

    flecs::entity e = ecs.entity()
        .set_override<NoCopy, Tag>({ 10 });

    const NoCopy *ptr = e.get<NoCopy, Tag>();
    test_assert(ptr != NULL);
    test_int(ptr->x_, 10);

    test_assert(e.has(flecs::Override | ecs.pair<NoCopy, Tag>()));
}

void ComponentLifecycle_set_override_pair_w_entity_no_copy() {
    flecs::world ecs;

    flecs::entity tag = ecs.entity();

    flecs::entity e = ecs.entity()
        .set_override<NoCopy>(tag, { 10 });

    const NoCopy *ptr = e.get<NoCopy>(tag);
    test_assert(ptr != NULL);
    test_int(ptr->x_, 10);

    test_assert(e.has(flecs::Override | ecs.pair<NoCopy>(tag)));
}
