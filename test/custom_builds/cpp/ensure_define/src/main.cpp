// Unreal (apparently) defines an ensure macro which conflicts with ensure
// methods in the C++ API. This test makes sure this doesn't break compilation.
#define ensure foobar
#include <ensure_define.h>
#include <iostream>

int main(int argc, char *argv[]) {
    flecs::world world(argc, argv);
}
