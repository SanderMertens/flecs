// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS

#include "flecs.h"

#include "CoreMinimal.h"

#include "FlecsTestUtils.h"

namespace test {
    struct Foo {
        float x;
        float y;
    };
}

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

    enum Number {
        One = 1,
        Two = 2,
        Three = 3
    };

    class Pod {
    public:
        struct Child { };

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

        Pod(Pod&& obj) noexcept {
            move_ctor_invoked ++;
            this->value = obj.value;
        }

        Pod& operator=(const Pod& obj) {
            copy_invoked ++;
            this->value = obj.value;
            return *this;
        }

        Pod& operator=(Pod&& obj) noexcept {
            move_invoked ++;
            this->value = obj.value;
            return *this;
        }   

        int value;

        inline static int ctor_invoked = 0;
        inline static int dtor_invoked = 0;
        inline static int copy_invoked = 0;
        inline static int move_invoked = 0;
        inline static int copy_ctor_invoked = 0;
        inline static int move_ctor_invoked = 0;
    };

    struct Tag0 { };
    struct Tag1 { };
    struct Tag2 { };
    struct Tag3 { };
    struct Tag4 { };
    struct Tag5 { };
    struct Tag6 { };
    struct Tag7 { };
    struct Tag8 { };
    struct Tag9 { };
    struct Tag10 { };
    struct Tag11 { };
    struct Tag12 { };
    struct Tag13 { };
    struct Tag14 { };
    struct Tag15 { };
    struct Tag16 { };
    struct Tag17 { };
    struct Tag18 { };
    struct Tag19 { };
    struct Tag20 { };
    struct Tag21 { };
    struct Tag22 { };
    struct Tag23 { };
    struct Tag24 { };
    struct Tag25 { };
    struct Tag26 { };
    struct Tag27 { };
    struct Tag28 { };
    struct Tag29 { };
    struct Tag30 { };
    struct Tag31 { };
    struct Tag32 { };

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

    struct NonCopyAssignable {
        NonCopyAssignable() { }
        NonCopyAssignable(const NonCopyAssignable& obj) = delete;
        NonCopyAssignable& operator=(const NonCopyAssignable& obj) = delete;

        NonCopyAssignable(NonCopyAssignable&& obj) = default;
        NonCopyAssignable& operator=(NonCopyAssignable&& obj) = default;

        int x;
    };

    struct NonCopyAssignableWMoveAssign {
        NonCopyAssignableWMoveAssign() { }
        NonCopyAssignableWMoveAssign(const NonCopyAssignableWMoveAssign& obj) = delete;
        NonCopyAssignableWMoveAssign& operator=(const NonCopyAssignableWMoveAssign& obj) = delete;

        NonCopyAssignableWMoveAssign(NonCopyAssignableWMoveAssign&& obj) {
            x = obj.x;
            moved = 1;
        }
        NonCopyAssignableWMoveAssign& operator=(NonCopyAssignableWMoveAssign&& obj) {
            x = obj.x;
            moved = 1;
            return *this;
        }

        int x;
        int moved = 0;
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

        CountNoDefaultCtor(CountNoDefaultCtor&& obj) noexcept {
            move_ctor_invoked ++;
            this->value = obj.value;
        }

        CountNoDefaultCtor& operator=(const CountNoDefaultCtor& obj) {
            copy_invoked ++;
            this->value = obj.value;
            return *this;
        }

        CountNoDefaultCtor& operator=(CountNoDefaultCtor&& obj) noexcept {
            move_invoked ++;
            this->value = obj.value;
            return *this;
        }

        static void reset() {
            ctor_invoked = 0;
            dtor_invoked = 0;
            copy_invoked = 0;
            move_invoked = 0;
            copy_ctor_invoked = 0;
            move_ctor_invoked = 0;
        }

        int value;

        inline static int ctor_invoked = 0;
        inline static int dtor_invoked = 0;
        inline static int copy_invoked = 0;
        inline static int move_invoked = 0;
        inline static int copy_ctor_invoked = 0;
        inline static int move_ctor_invoked = 0;
    };

    struct Struct_w_string {
        std::string value;
    };

    struct Struct_w_vector {
        std::vector<int> value;
    };

    struct Tgt { };

    struct R { };
    struct T1 { };
    struct T2 { };
    struct T3 { };

    struct Rel { };

    /**
     * A class that meticulously tracks all operations affecting its lifecycle,
     * including construction, assignment, and destruction.
     *
     * Unlike with Pod, these statistics are tracked per object,
     * which allows for more fine-grained tests.
     */
    struct FlecsTestLifecycleTracker
    {
        enum class Constructor
        {
            default_,
            copy,
            move,
        };

        Constructor constructed_via;
        int         times_destructed{};

        int         times_copy_assigned_into{};
        mutable int times_copy_assigned_from{};
        mutable int times_copy_constructed_from{};

        int times_move_assigned_into{};
        int times_move_assigned_from{};
        int times_move_constructed_from{};

        FlecsTestLifecycleTracker() : constructed_via{Constructor::default_} {}

        FlecsTestLifecycleTracker(FlecsTestLifecycleTracker const& that) : constructed_via{Constructor::copy}
        {
            ++that.times_copy_constructed_from;
        }
        FlecsTestLifecycleTracker& operator=(FlecsTestLifecycleTracker const& that)
        {
            ++times_copy_assigned_into;
            ++that.times_copy_assigned_from;
            return *this;
        }

        FlecsTestLifecycleTracker(FlecsTestLifecycleTracker&& that) noexcept : constructed_via{Constructor::move}
        {
            ++that.times_move_constructed_from;
        }
        FlecsTestLifecycleTracker& operator=(FlecsTestLifecycleTracker&& that) noexcept
        {
            ++times_move_assigned_into;
            ++that.times_move_assigned_from;
            return *this;
        }

        ~FlecsTestLifecycleTracker() { ++times_destructed; }

        bool moved_from() const { return times_move_assigned_from > 0 || times_move_constructed_from > 0; }
        bool moved_into() const { return times_move_assigned_into > 0 || constructed_via == Constructor::move; }
        bool copied_from() const { return times_copy_assigned_from > 0 || times_copy_constructed_from > 0; }
        bool copied_into() const { return times_copy_assigned_into > 0 || constructed_via == Constructor::copy; }
    };

    static inline void RegisterTestTypeComponents(flecs::world& ecs) {
        ecs.component<Position>();
        ecs.component<Velocity>();
        ecs.component<Mass>();
        ecs.component<Rotation>();
        ecs.component<Tag>();
        ecs.component<Self>();
        ecs.component<Number>();
        ecs.component<Pod>();
        ecs.component<NoDefaultCtor>();
        ecs.component<DefaultInit>();
        ecs.component<NoCopy>();
        ecs.component<NoMove>();
        ecs.component<NoCopyCtor>();
        ecs.component<NoCopyAssign>();
        ecs.component<NoMoveCtor>();
        ecs.component<NoMoveAssign>();
        ecs.component<NonCopyAssignable>();
        ecs.component<NonCopyAssignableWMoveAssign>();
        ecs.component<FlecsCtor>();
        ecs.component<FlecsCtorDefaultCtor>();
        ecs.component<FlecsCtorValueCtor>();
        ecs.component<CountNoDefaultCtor>();
        ecs.component<Tgt>();
        ecs.component<R>();
        ecs.component<T1>();
        ecs.component<T2>();
        ecs.component<T3>();
        ecs.component<Rel>();
        ecs.component<test::Foo>();
        ecs.component<Struct_w_string>();
        ecs.component<Struct_w_vector>();
        ecs.component<Pod::Child>();
        ecs.component<Tag0>();
        ecs.component<Tag1>();
        ecs.component<Tag2>();
        ecs.component<Tag3>();
        ecs.component<Tag4>();
        ecs.component<Tag5>();
        ecs.component<Tag6>();
        ecs.component<Tag7>();
        ecs.component<Tag8>();
        ecs.component<Tag9>();
        ecs.component<Tag10>();
        ecs.component<Tag11>();
        ecs.component<Tag12>();
        ecs.component<Tag13>();
        ecs.component<Tag14>();
        ecs.component<Tag15>();
        ecs.component<Tag16>();
        ecs.component<Tag17>();
        ecs.component<Tag18>();
        ecs.component<Tag19>();
        ecs.component<Tag20>();
        ecs.component<Tag21>();
        ecs.component<Tag22>();
        ecs.component<Tag23>();
        ecs.component<Tag24>();
        ecs.component<Tag25>();
        ecs.component<Tag26>();
        ecs.component<Tag27>();
        ecs.component<Tag28>();
        ecs.component<Tag29>();
        ecs.component<Tag30>();
        ecs.component<Tag31>();
        ecs.component<Tag32>();
        ecs.component<Template<int>>();
        ecs.component<Template<float>>();
        ecs.component<Template<double>>();
        ecs.component<Template<Position>>();
        ecs.component<Template<Velocity>>();
        ecs.component<Template<Mass>>();
        ecs.component<Template<Rotation>>();
        ecs.component<Template<Tag>>();
        ecs.component<FlecsTestLifecycleTracker>();
    }

#endif // WITH_AUTOMATION_TESTS
