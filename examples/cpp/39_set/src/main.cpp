#include <set.h>
#include <iostream>

// A nice side effect of using the lamvda set API is that we don't need to add
// non-default constructors that accept the members we want to set.

struct Days {
    Days() : value_(0) { }
    int value_;
};

struct Weeks {
    Weeks() : value_(0) { }
    int value_;
};

struct Months {
    Months() : value_(0) { }
    int value_;
};

// Increment days, weeks, months
void inc(flecs::entity e) {
    e.set([](Days& days, Weeks& weeks, Months& months) {
        days.value_ ++;

        if (!(days.value_ % 7)) {
            weeks.value_ ++;
        }

        if (!(days.value_ % 30)) {
            months.value_ ++;
        }
    });
}

int main(int argc, char *argv[]) {
    // Create the world, pass arguments for overriding the number of threads,fps
    // or for starting the admin dashboard (see flecs.h for details).
    flecs::world ecs(argc, argv);

    // Create empty entity
    auto e = ecs.entity();

    // Increment a bunch of times
    for (int i = 0; i < 40; i ++) {
        inc(e);
    }

    // Print output for three components
    e.get([](const Days& d, const Weeks& w, const Months& m) {
        std::cout << "days: " << d.value_
                  << ", weeks: " << w.value_
                  << ", months: " << m.value_ << std::endl;
    });
}
