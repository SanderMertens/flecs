// Catch static asserts
#define flecs_static_assert(cond, str)\
    ecs_assert(cond, ECS_INVALID_OPERATION, str)

#include <cpp_api.h>

#include <string>

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

class Str {
public:
    std::string value;
};

void ComponentLifecycle_ctor_on_add() {
    flecs::world world;

    world.component<Pod>();

    auto e = flecs::entity(world).add<Pod>();
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

    auto e = flecs::entity(world).add<Pod>();
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

    auto e = flecs::entity(world).add<Pod>();
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

    auto e = flecs::entity(world).add<Position>().add<Pod>();
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

    auto e = flecs::entity(world);
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

    auto base = flecs::entity(world);
    test_assert(base.id() != 0);

    base.set<Pod>({10});
    test_int(Pod::ctor_invoked, 2);
    test_int(Pod::dtor_invoked, 1);
    test_int(Pod::copy_invoked, 0);
    Pod::ctor_invoked = 0;
    Pod::dtor_invoked = 0;
    Pod::copy_invoked = 0;
    Pod::move_invoked = 0;

    auto e = flecs::entity(world);
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
    
    e.add(flecs::IsA, base);

    e.add<Str>();

    const Str *str = e.get<Str>();
    test_assert(str != NULL);
    test_assert(str->value == "Hello World");
}

void ComponentLifecycle_get_mut_new() {
    flecs::world world;

    world.component<Pod>();

    auto e = flecs::entity(world);
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

    auto e = flecs::entity(world);
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

    auto e = flecs::entity(world).add<Pod>();
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

    flecs::entity(world).add<Pod>();

    test_int(Pod::ctor_invoked, 3);
    test_int(Pod::move_ctor_invoked, 2);
    test_int(Pod::move_invoked, 0);
}

void ComponentLifecycle_implicit_after_query() {
    flecs::world world;

    world.query<Pod>();

    auto e = flecs::entity(world).add<Pod>();
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
static void try_add_w_object(flecs::world& ecs) {
    flecs::entity rel = ecs.entity();

    flecs::entity e = ecs.entity().add_w_object<T>(rel);
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

void ComponentLifecycle_no_default_ctor_add_w_object() {
    install_test_abort();

    flecs::world ecs;

    ecs.component<NoDefaultCtor>();

    test_expect_abort();

    try_add_w_object<NoDefaultCtor>(ecs);
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
        test_int(CountNoDefaultCtor::dtor_invoked, 0);
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
    test_int(CountNoDefaultCtor::dtor_invoked, 3);
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

        auto e3 = world.entity().emplace<CountNoDefaultCtor>(3);

        test_int(CountNoDefaultCtor::ctor_invoked, 3);
        test_int(CountNoDefaultCtor::dtor_invoked, 0);
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

        e1.add<Tag>();

        test_int(CountNoDefaultCtor::ctor_invoked, 3);
        test_int(CountNoDefaultCtor::dtor_invoked, 1);
        test_int(CountNoDefaultCtor::copy_invoked, 0);
        test_int(CountNoDefaultCtor::move_invoked, 0);
        test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
        test_int(CountNoDefaultCtor::move_ctor_invoked, 3);

        e2.add<Tag>();

        test_int(CountNoDefaultCtor::ctor_invoked, 3);
        test_int(CountNoDefaultCtor::dtor_invoked, 2);
        test_int(CountNoDefaultCtor::copy_invoked, 0);
        test_int(CountNoDefaultCtor::move_invoked, 0);
        test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
        test_int(CountNoDefaultCtor::move_ctor_invoked, 4);

        e3.add<Tag>();

        test_int(CountNoDefaultCtor::ctor_invoked, 3);
        test_int(CountNoDefaultCtor::dtor_invoked, 3);
        test_int(CountNoDefaultCtor::copy_invoked, 0);
        test_int(CountNoDefaultCtor::move_invoked, 0);
        test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
        test_int(CountNoDefaultCtor::move_ctor_invoked, 7); /* resize */
    }

    test_int(CountNoDefaultCtor::ctor_invoked, 3);
    test_int(CountNoDefaultCtor::dtor_invoked, 6);
    test_int(CountNoDefaultCtor::copy_invoked, 0);
    test_int(CountNoDefaultCtor::move_invoked, 0);
    test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
    test_int(CountNoDefaultCtor::move_ctor_invoked, 7);
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

        auto e3 = world.entity().emplace<CountNoDefaultCtor>(3);

        test_int(CountNoDefaultCtor::ctor_invoked, 3);
        test_int(CountNoDefaultCtor::dtor_invoked, 0);
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

        e1.add<Position>();

        test_int(CountNoDefaultCtor::ctor_invoked, 3);
        test_int(CountNoDefaultCtor::dtor_invoked, 1);
        test_int(CountNoDefaultCtor::copy_invoked, 0);
        test_int(CountNoDefaultCtor::move_invoked, 0);
        test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
        test_int(CountNoDefaultCtor::move_ctor_invoked, 3);

        e2.add<Position>();

        test_int(CountNoDefaultCtor::ctor_invoked, 3);
        test_int(CountNoDefaultCtor::dtor_invoked, 2);
        test_int(CountNoDefaultCtor::copy_invoked, 0);
        test_int(CountNoDefaultCtor::move_invoked, 0);
        test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
        test_int(CountNoDefaultCtor::move_ctor_invoked, 4);

        e3.add<Position>();

        test_int(CountNoDefaultCtor::ctor_invoked, 3);
        test_int(CountNoDefaultCtor::dtor_invoked, 3);
        test_int(CountNoDefaultCtor::copy_invoked, 0);
        test_int(CountNoDefaultCtor::move_invoked, 0);
        test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
        test_int(CountNoDefaultCtor::move_ctor_invoked, 7); /* resize */
    }

    test_int(CountNoDefaultCtor::ctor_invoked, 3);
    test_int(CountNoDefaultCtor::dtor_invoked, 6);
    test_int(CountNoDefaultCtor::copy_invoked, 0);
    test_int(CountNoDefaultCtor::move_invoked, 0);
    test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
    test_int(CountNoDefaultCtor::move_ctor_invoked, 7);
}

void ComponentLifecycle_delete_no_default_ctor() {
	{
        flecs::world world;

        world.component<CountNoDefaultCtor>();

        auto e1 = world.entity().emplace<CountNoDefaultCtor>(1);
        auto e2 = world.entity().emplace<CountNoDefaultCtor>(2);
        auto e3 = world.entity().emplace<CountNoDefaultCtor>(3);
        
        test_int(CountNoDefaultCtor::ctor_invoked, 3);
        test_int(CountNoDefaultCtor::dtor_invoked, 0);
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
        test_int(CountNoDefaultCtor::dtor_invoked, 1);
        test_int(CountNoDefaultCtor::copy_invoked, 0);
        test_int(CountNoDefaultCtor::move_invoked, 1);
        test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
        test_int(CountNoDefaultCtor::move_ctor_invoked, 2);
    }

    test_int(CountNoDefaultCtor::ctor_invoked, 3);
    test_int(CountNoDefaultCtor::dtor_invoked, 3);
    test_int(CountNoDefaultCtor::copy_invoked, 0);
    test_int(CountNoDefaultCtor::move_invoked, 1);
    test_int(CountNoDefaultCtor::copy_ctor_invoked, 0);
    test_int(CountNoDefaultCtor::move_ctor_invoked, 2);
}
