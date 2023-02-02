#include <ser_std_vector.h>
#include <iostream>
#include <string>
#include <vector>

// This example shows how to serialize a component with std::vectors

struct VectorComponent {
    std::vector<int32_t> ints;
    std::vector<std::string> strings;
};

// Serializer function for std::string
int std_string_ser(const flecs::serializer *s, const std::string *data) {
    const char *str = data->c_str();
    s->value(flecs::String, &str); // Serializer-specific string serialization
    return 0;
}

// Serializer function for std::vector
template <typename Elem>
int std_vector_ser(const flecs::serializer *s, const std::vector<Elem> *data) {
    for (const auto& el : *data) {
        s->value(el);
    }
    return 0;
}

int main(int, char *[]) {
    flecs::world ecs;

    // Register serialization support for std::string
    ecs.component<std::string>()
        .serialize(
            flecs::String,   // Describes what the serialized output looks like
            std_string_ser); // Serializer function

    // Register serialization support for std::vector<int>
    ecs.component<std::vector<int>>()
        .serialize(
            ecs.vector<int32_t>(),
            std_vector_ser<int32_t>);

    // Register serialization support for std::vector<std::string>
    ecs.component<std::vector<std::string>>()
        .serialize(
            ecs.vector<std::string>(),
            std_vector_ser<std::string>);

    // Register component with std::string members
    ecs.component<VectorComponent>()
        .member<std::vector<int>>("ints")
        .member<std::vector<std::string>>("strings");

    // Create dummy value & serialize it
    VectorComponent v = {{1, 2, 3}, {"foo", "bar"}};
    auto json = ecs.to_json(&v);
    std::cout << json << std::endl;

    // Output:
    //   {"ints":[1, 2, 3], "strings":["foo", "bar"]}
}
