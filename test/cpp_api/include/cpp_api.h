#ifndef CPP_API_H
#define CPP_API_H

/* This generated file contains includes for project dependencies */
#include <cpp_api/bake_config.h>

struct Position {
    float x;
    float y;
};

struct Velocity {
    float x;
    float y;
};

struct Mass {
    float value;
};

struct Rotation {
    float value;
};

struct Tag { };

struct Self {
    flecs::entity_view value;
};

class Pod {
public:
    Pod() {
        ctor_invoked ++;
        value = 10;
    }

    Pod(int v) {
        ctor_invoked ++;
        value = v;
    }    

    ~Pod() {
        dtor_invoked ++;
    }

    Pod(const Pod& obj) {
        copy_ctor_invoked ++;
        this->value = obj.value;
    }

    Pod(Pod&& obj) {
        move_ctor_invoked ++;
        this->value = obj.value;
    }

    Pod& operator=(const Pod& obj) {
        copy_invoked ++;
        this->value = obj.value;
        return *this;
    }

    Pod& operator=(Pod&& obj) {
        move_invoked ++;
        this->value = obj.value;
        return *this;
    }   

    int value;

    static int ctor_invoked;
    static int dtor_invoked;
    static int copy_invoked;
    static int move_invoked;
    static int copy_ctor_invoked;
    static int move_ctor_invoked;
};

struct TagA { };
struct TagB { };
struct TagC { };
struct TagD { };
struct TagE { };
struct TagF { };
struct TagG { };
struct TagH { };
struct TagI { };
struct TagJ { };
struct TagK { };
struct TagL { };
struct TagM { };
struct TagN { };
struct TagO { };
struct TagP { };
struct TagQ { };
struct TagR { };
struct TagS { };
struct TagT { };

template <typename T>
struct Template {
    T x;
    T y;
};

struct NoDefaultCtor {
    NoDefaultCtor(int x) : x_(x) { }
    NoDefaultCtor(const NoDefaultCtor& obj) = default;
    NoDefaultCtor(NoDefaultCtor&& obj) = default;

    NoDefaultCtor& operator=(const NoDefaultCtor& obj) = default;
    NoDefaultCtor& operator=(NoDefaultCtor&& obj) = default;

    ~NoDefaultCtor() { }

    int x_;
};

struct DefaultInit {
    DefaultInit() : x_(99) { test_assert(y_ == 99); }
    DefaultInit(int x) : x_(x) { test_assert(y_ == 99); }
    DefaultInit(const DefaultInit& obj) = default;
    DefaultInit(DefaultInit&& obj) = default;

    DefaultInit& operator=(const DefaultInit& obj) = default;
    DefaultInit& operator=(DefaultInit&& obj) = default;

    ~DefaultInit() { }

    int x_;
    int y_ = 99;
};

struct NoCopy {
    NoCopy() : x_(99) { }
    NoCopy(int x) : x_(x) { }
    NoCopy(const NoCopy& obj) = delete;
    NoCopy(NoCopy&& obj) = default;

    NoCopy& operator=(const NoCopy& obj) = delete;
    NoCopy& operator=(NoCopy&& obj) = default;

    ~NoCopy() { }

    int x_;
};

struct NoMove {
    NoMove() : x_(99) { }
    NoMove(int x) : x_(x) { }
    NoMove(const NoMove& obj) = default;
    NoMove(NoMove&& obj) = delete;

    NoMove& operator=(const NoMove& obj) = default;
    NoMove& operator=(NoMove&& obj) = delete;

    ~NoMove() { }

    int x_;
};

struct NoCopyCtor {
    NoCopyCtor() : x_(99) { }
    NoCopyCtor(int x) : x_(x) { }
    NoCopyCtor(const NoCopyCtor& obj) = delete;
    NoCopyCtor(NoCopyCtor&& obj) = default;

    NoCopyCtor& operator=(const NoCopyCtor& obj) = default;
    NoCopyCtor& operator=(NoCopyCtor&& obj) = default;

    ~NoCopyCtor() { }

    int x_;
};

struct NoCopyAssign {
    NoCopyAssign() : x_(99) { }
    NoCopyAssign(int x) : x_(x) { }
    NoCopyAssign(const NoCopyAssign& obj) = default;
    NoCopyAssign(NoCopyAssign&& obj) = default;

    NoCopyAssign& operator=(const NoCopyAssign& obj) = delete;
    NoCopyAssign& operator=(NoCopyAssign&& obj) = default;

    ~NoCopyAssign() { }

    int x_;
};

struct NoMoveCtor {
    NoMoveCtor() : x_(99) { }
    NoMoveCtor(int x) : x_(x) { }
    NoMoveCtor(const NoMoveCtor& obj) = default;
    NoMoveCtor(NoMoveCtor&& obj) = delete;

    NoMoveCtor& operator=(const NoMoveCtor& obj) = default;
    NoMoveCtor& operator=(NoMoveCtor&& obj) = default;

    ~NoMoveCtor() { }

    int x_;
};

struct NoMoveAssign {
    NoMoveAssign() : x_(99) { }
    NoMoveAssign(int x) : x_(x) { }
    NoMoveAssign(const NoMoveAssign& obj) = default;
    NoMoveAssign(NoMoveAssign&& obj) = default;

    NoMoveAssign& operator=(const NoMoveAssign& obj) = default;
    NoMoveAssign& operator=(NoMoveAssign&& obj) = delete;

    ~NoMoveAssign() { }

    int x_;
};

struct NoDtor {
    NoDtor() : x_(99) { }
    NoDtor(int x) : x_(x) { }
    NoDtor(const NoDtor& obj) = default;
    NoDtor(NoDtor&& obj) = default;

    NoDtor& operator=(const NoDtor& obj) = delete;
    NoDtor& operator=(NoDtor&& obj) = default;

    ~NoDtor() = delete;

    int x_;
};

struct FlecsCtor {
    FlecsCtor(flecs::world&, flecs::entity e) : x_(89), e_(e) { }

    FlecsCtor(const FlecsCtor& obj) = delete;
    FlecsCtor(FlecsCtor&& obj) = default;

    FlecsCtor& operator=(const FlecsCtor& obj) = default;
    FlecsCtor& operator=(FlecsCtor&& obj) = default;

    ~FlecsCtor() { }

    int x_;
    flecs::entity e_;
};

struct FlecsCtorDefaultCtor {
    FlecsCtorDefaultCtor() : x_(99) { }
    FlecsCtorDefaultCtor(flecs::world&, flecs::entity e) : x_(89), e_(e) { }

    FlecsCtorDefaultCtor(const FlecsCtorDefaultCtor& obj) = delete;
    FlecsCtorDefaultCtor(FlecsCtorDefaultCtor&& obj) = default;

    FlecsCtorDefaultCtor& operator=(const FlecsCtorDefaultCtor& obj) = default;
    FlecsCtorDefaultCtor& operator=(FlecsCtorDefaultCtor&& obj) = default;

    ~FlecsCtorDefaultCtor() { }

    int x_;
    flecs::entity e_;
};

struct DefaultCtorValueCtor {
    DefaultCtorValueCtor() : x_(99) { }
    DefaultCtorValueCtor(int x) : x_(x) { }

    DefaultCtorValueCtor(const DefaultCtorValueCtor& obj) = delete;
    DefaultCtorValueCtor(DefaultCtorValueCtor&& obj) = default;

    DefaultCtorValueCtor& operator=(const DefaultCtorValueCtor& obj) = default;
    DefaultCtorValueCtor& operator=(DefaultCtorValueCtor&& obj) = default;

    ~DefaultCtorValueCtor() { }

    int x_;
};

struct FlecsCtorValueCtor {
    FlecsCtorValueCtor(int x) : x_(x) { }
    FlecsCtorValueCtor(flecs::world&, flecs::entity e) : x_(89), e_(e) { }

    FlecsCtorValueCtor(const FlecsCtorValueCtor& obj) = delete;
    FlecsCtorValueCtor(FlecsCtorValueCtor&& obj) = default;

    FlecsCtorValueCtor& operator=(const FlecsCtorValueCtor& obj) = default;
    FlecsCtorValueCtor& operator=(FlecsCtorValueCtor&& obj) = default;

    ~FlecsCtorValueCtor() { }

    int x_;
    flecs::entity e_;
};

class CountNoDefaultCtor {
public:
    CountNoDefaultCtor(int v) {
        ctor_invoked ++;
        value = v;
    }    

    ~CountNoDefaultCtor() {
        dtor_invoked ++;
    }

    CountNoDefaultCtor(const CountNoDefaultCtor& obj) {
        copy_ctor_invoked ++;
        this->value = obj.value;
    }

    CountNoDefaultCtor(CountNoDefaultCtor&& obj) {
        move_ctor_invoked ++;
        this->value = obj.value;
    }

    CountNoDefaultCtor& operator=(const CountNoDefaultCtor& obj) {
        copy_invoked ++;
        this->value = obj.value;
        return *this;
    }

    CountNoDefaultCtor& operator=(CountNoDefaultCtor&& obj) {
        move_invoked ++;
        this->value = obj.value;
        return *this;
    }   

    int value;

    static int ctor_invoked;
    static int dtor_invoked;
    static int copy_invoked;
    static int move_invoked;
    static int copy_ctor_invoked;
    static int move_ctor_invoked;
};

void install_test_abort();

#endif

