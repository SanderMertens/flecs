#include <cpp_api.h>

void Iterable_page_each() {
    flecs::world ecs;

    auto e1 = ecs.entity(); e1.set<Self>({e1});
    auto e2 = ecs.entity(); e2.set<Self>({e2});
    auto e3 = ecs.entity(); e3.set<Self>({e3});
    auto e4 = ecs.entity(); e4.set<Self>({e4});
    auto e5 = ecs.entity(); e5.set<Self>({e5});

    auto q = ecs.query<Self>();

    int32_t count = 0;
    q.page(1, 3).each([&](flecs::entity e, Self& self) {
        count ++;
        test_assert(e != e1);
        test_assert(e != e5);
        test_assert(e == self.value);
    });

    test_int(count, 3);
}

void Iterable_page_iter() {
    flecs::world ecs;

    auto e1 = ecs.entity(); e1.set<Self>({e1});
    auto e2 = ecs.entity(); e2.set<Self>({e2});
    auto e3 = ecs.entity(); e3.set<Self>({e3});
    auto e4 = ecs.entity(); e4.set<Self>({e4});
    auto e5 = ecs.entity(); e5.set<Self>({e5});

    auto q = ecs.query<Self>();

    int32_t count = 0;
    q.page(1, 3).iter([&](flecs::iter it, Self* self) {
        test_int(it.count(), 3);
        test_assert(it.entity(0) == e2);
        test_assert(it.entity(1) == e3);
        test_assert(it.entity(2) == e4);
        test_assert(it.entity(0) == self[0].value);
        test_assert(it.entity(1) == self[1].value);
        test_assert(it.entity(2) == self[2].value);
        count += it.count();
    });

    test_int(count, 3);
}

void Iterable_worker_each() {
    flecs::world ecs;

    auto e1 = ecs.entity(); e1.set<Self>({e1});
    auto e2 = ecs.entity(); e2.set<Self>({e2});
    auto e3 = ecs.entity(); e3.set<Self>({e3});
    auto e4 = ecs.entity(); e4.set<Self>({e4});
    auto e5 = ecs.entity(); e5.set<Self>({e5});

    auto q = ecs.query<Self>();

    int32_t count = 0;
    q.worker(0, 2).each([&](flecs::entity e, Self& self) {
        count ++;
        test_assert(e != e4);
        test_assert(e != e5);
        test_assert(e == self.value);
    });

    test_int(count, 3);

    count = 0;
    q.worker(1, 2).each([&](flecs::entity e, Self& self) {
        count ++;
        test_assert(e != e1);
        test_assert(e != e2);
        test_assert(e != e3);
        test_assert(e == self.value);
    });

    test_int(count, 2);
}

void Iterable_worker_iter() {
    flecs::world ecs;

    auto e1 = ecs.entity(); e1.set<Self>({e1});
    auto e2 = ecs.entity(); e2.set<Self>({e2});
    auto e3 = ecs.entity(); e3.set<Self>({e3});
    auto e4 = ecs.entity(); e4.set<Self>({e4});
    auto e5 = ecs.entity(); e5.set<Self>({e5});

    auto q = ecs.query<Self>();

    int32_t count = 0;
    q.worker(0, 2).iter([&](flecs::iter it, Self* self) {
        test_int(it.count(), 3);
        test_assert(it.entity(0) == e1);
        test_assert(it.entity(1) == e2);
        test_assert(it.entity(2) == e3);
        test_assert(it.entity(0) == self[0].value);
        test_assert(it.entity(1) == self[1].value);
        test_assert(it.entity(2) == self[2].value);
        count += it.count();
    });

    test_int(count, 3);

    count = 0;
    q.worker(1, 2).iter([&](flecs::iter it, Self* self) {
        test_int(it.count(), 2);
        test_assert(it.entity(0) == e4);
        test_assert(it.entity(1) == e5);
        test_assert(it.entity(0) == self[0].value);
        test_assert(it.entity(1) == self[1].value);
        count += it.count();
    });

    test_int(count, 2);
}
