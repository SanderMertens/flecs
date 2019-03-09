#include <include/api.h>

static
void Iter(EcsRows *rows) {
    Position *p = ecs_column(rows, Position, 1);
    Velocity *v = ecs_column(rows, Velocity, 2);
    Mass *m = ecs_column(rows, Mass, 3);

    ProbeSystem(rows);

    int i;
    for (i = rows->offset; i < (rows->offset + rows->limit); i ++) {
        p[i].x = 10;
        p[i].y = 20;

        if (v) {
            v[i].x = 30;
            v[i].y = 40;
        }

        if (m) {
            m[i] = 50;
        }
    }
}

void Run_run() {

}

void Run_run_w_offset() {
    // Implement testcase
}

void Run_run_w_limit() {
    // Implement testcase
}

void Run_run_w_offset_limit() {
    // Implement testcase
}

void Run_run_w_type_filter() {
    // Implement testcase
}

void Run_run_w_type_filter_of_2() {
    // Implement testcase
}

void Run_run_w_param() {
    // Implement testcase
}
