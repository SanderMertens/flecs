#include <cpp.h>

void Iterable_page_each(void) {
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

void Iterable_page_iter(void) {
    flecs::world ecs;

    auto e1 = ecs.entity(); e1.set<Self>({e1});
    auto e2 = ecs.entity(); e2.set<Self>({e2});
    auto e3 = ecs.entity(); e3.set<Self>({e3});
    auto e4 = ecs.entity(); e4.set<Self>({e4});
    auto e5 = ecs.entity(); e5.set<Self>({e5});

    auto q = ecs.query<Self>();

    int32_t count = 0;
    q.page(1, 3).run([&](flecs::iter it) {
        while (it.next()) {
            auto self = it.field<Self>(0);
            test_int(it.count(), 3);
            test_assert(it.entity(0) == e2);
            test_assert(it.entity(1) == e3);
            test_assert(it.entity(2) == e4);
            test_assert(it.entity(0) == self[0].value);
            test_assert(it.entity(1) == self[1].value);
            test_assert(it.entity(2) == self[2].value);
            count += it.count();
        }
    });

    test_int(count, 3);
}

void Iterable_worker_each(void) {
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

void Iterable_worker_iter(void) {
    flecs::world ecs;

    auto e1 = ecs.entity(); e1.set<Self>({e1});
    auto e2 = ecs.entity(); e2.set<Self>({e2});
    auto e3 = ecs.entity(); e3.set<Self>({e3});
    auto e4 = ecs.entity(); e4.set<Self>({e4});
    auto e5 = ecs.entity(); e5.set<Self>({e5});

    auto q = ecs.query<Self>();

    int32_t count = 0;
    q.worker(0, 2).run([&](flecs::iter it) {
        while (it.next()) {
            auto self = it.field<Self>(0);
            test_int(it.count(), 3);
            test_assert(it.entity(0) == e1);
            test_assert(it.entity(1) == e2);
            test_assert(it.entity(2) == e3);
            test_assert(it.entity(0) == self[0].value);
            test_assert(it.entity(1) == self[1].value);
            test_assert(it.entity(2) == self[2].value);
            count += it.count();
        }
    });

    test_int(count, 3);

    count = 0;
    q.worker(1, 2).run([&](flecs::iter it) {
        while (it.next()) {
            auto self = it.field<Self>(0);
            test_int(it.count(), 2);
            test_assert(it.entity(0) == e4);
            test_assert(it.entity(1) == e5);
            test_assert(it.entity(0) == self[0].value);
            test_assert(it.entity(1) == self[1].value);
            count += it.count();
        }
    });

    test_int(count, 2);
}

void Iterable_frame_offset(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().set<Position>({1, 0});
    auto e2 = ecs.entity().set<Position>({2, 0});
    auto e3 = ecs.entity().set<Position>({3, 0}).set<Velocity>({0, 0});
    auto e4 = ecs.entity().set<Position>({4, 0}).set<Velocity>({0, 0});
    auto e5 = ecs.entity().set<Position>({5, 0}).set<Mass>({0});

    auto q = ecs.query<Position>();

    flecs::entity_t entities[5] = {0};
    int32_t results = 0;
    size_t total = 0;
    q.run([&](flecs::iter it) {
        while (it.next()) {
            test_int(it.frame_offset(), total);
            for (auto i : it) {
                entities[it.frame_offset() + i] = it.entity(i);
            }
            total += it.count();
            results ++;
        }
    });

    test_int(results, 3);
    test_int(total, 5);
    test_uint(entities[0], e1);
    test_uint(entities[1], e2);
    test_uint(entities[2], e3);
    test_uint(entities[3], e4);
    test_uint(entities[4], e5);
}

void Iterable_worker_frame_offset(void) {
    flecs::world ecs;

    auto e1 = ecs.entity().set<Position>({1, 0});
    auto e2 = ecs.entity().set<Position>({2, 0});
    auto e3 = ecs.entity().set<Position>({3, 0}).set<Velocity>({0, 0});
    auto e4 = ecs.entity().set<Position>({4, 0}).set<Velocity>({0, 0});
    auto e5 = ecs.entity().set<Position>({5, 0}).set<Mass>({0});

    auto q = ecs.query<Position>();

    flecs::entity_t entities[5] = {0};
    int32_t results = 0;
    q.worker(0, 2).run([&](flecs::iter it) {
        while (it.next()) {
            test_int(it.count(), 1);
            if (results == 0) {
                test_int(it.frame_offset(), 0);
            } else if (results == 1) {
                test_int(it.frame_offset(), 2);
            } else if (results == 2) {
                test_int(it.frame_offset(), 4);
            }
            entities[it.frame_offset()] = it.entity(0);
            results ++;
        }
    });

    test_int(results, 3);

    results = 0;
    q.worker(1, 2).run([&](flecs::iter it) {
        while (it.next()) {
            test_int(it.count(), 1);
            if (results == 0) {
                test_int(it.frame_offset(), 1);
            } else if (results == 1) {
                test_int(it.frame_offset(), 3);
            }
            entities[it.frame_offset()] = it.entity(0);
            results ++;
        }
    });

    test_int(results, 2);
    test_uint(entities[0], e1);
    test_uint(entities[1], e2);
    test_uint(entities[2], e3);
    test_uint(entities[3], e4);
    test_uint(entities[4], e5);
}
