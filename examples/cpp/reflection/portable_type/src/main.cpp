#include <portable_type.h>
#include <iostream>

// The actual types of of int32_t and uintptr_t vary between platforms.
struct PortableType {
    int32_t i32;
    uintptr_t intptr;
};

int main(int, char *[]) {
    flecs::world ecs;

    // Register component. Do not use std::int32_t or std::uintptr_t for the 
    // member type as this will resolve to a different integer type depending on
    // the platform, which can cause unexpected issues when type information is
    // shared between platforms.
    ecs.component<PortableType>()
        .member("i32", flecs::I32) // Use platform-independent type ids
        .member("intptr", flecs::Uptr);

    // Create entity with Position as usual
    auto e = ecs.entity()
        .set<PortableType>({10, 20});

    // Convert position component to flecs expression string
    const PortableType *ptr = e.get<PortableType>();
    std::cout << ecs.to_expr(ptr).c_str() << "\n"; // {i32: 10, inptr: 20}
}
