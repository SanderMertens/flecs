#include <ser_opaque_type.h>
#include <iostream>
#include <string>

// This example shows how to serialize a type with private members and public
// getter methods.

struct OpaqueType {
    OpaqueType(double x, double y) : x_(x), y_(y) { }

    double x() const { return x_; }
    double y() const { return y_; }

private:
    double x_;
    double y_;
};

// Serializer function for std::string
int opaque_ser(const flecs::serializer *s, const OpaqueType *data) {
    s->member("x"); // Serializer-specific function to insert member
    s->value(data->x()); // Serializer specific function to insert double value
    s->member("y");
    s->value(data->y());
    return 0;
}

int main(int, char *[]) {
    flecs::world ecs;

    // Create struct type that represents the opaque type
    flecs::entity as_struct = ecs.component()
        .member<double>("x")
        .member<double>("y");

    // Register serialization support for opaque type
    ecs.component<OpaqueType>()
        .serialize(
            as_struct,    // Describes what the serialized output looks like
            opaque_ser);  // Serializer function

    // Create dummy value & serialize it
    OpaqueType v = {10, 20};
    std::cout << ecs.to_json(&v) << std::endl;

    // Output:
    //   {"x":10, "y":20}
}
