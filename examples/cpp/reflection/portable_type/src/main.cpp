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
        .member(flecs::I32, "i32") // Use platform-independent type ids
        .member(flecs::Uptr, "intptr");

    // Create entity with PortableType
    auto e = ecs.entity()
        .set<PortableType>({10, 20});

    // Convert PortableType component to flecs expression string
    const PortableType *ptr = e.get<PortableType>();
    std::cout << ecs.to_expr(ptr).c_str() << "\n"; // {i32: 10, inptr: 20}
}
