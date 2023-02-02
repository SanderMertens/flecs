#include <ser_std_string.h>
#include <iostream>
#include <string>

// This example shows how to serialize a component with an std::string

struct StringComponent {
    std::string a;
    std::string b;
};

// Serializer function for std::string
int std_string_ser(const flecs::serializer *s, const std::string *data) {
    const char *str = data->c_str();
    s->value(flecs::String, &str); // Serializer-specific string serialization
    return 0;
}

int main(int, char *[]) {
    flecs::world ecs;

    // Register serialization support for std::string
    ecs.component<std::string>()
        .serialize(
            flecs::String,   // Describes what the serialized output looks like
            std_string_ser); // Serializer function

    // Register component with std::string members
    ecs.component<StringComponent>()
        .member<std::string>("a")
        .member<std::string>("b");

    // Create dummy value & serialize it
    StringComponent v = {"foo", "bar"};
    auto json = ecs.to_json(&v);
    std::cout << json << std::endl;

    // Output:
    //   {"a": "foo", "b": "bar"}
}
