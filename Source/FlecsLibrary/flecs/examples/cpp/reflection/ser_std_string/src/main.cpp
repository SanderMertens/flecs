#include <ser_std_string.h>
#include <iostream>
#include <string>

// This example shows how to serialize a component with an std::string

struct StringComponent {
    std::string a;
    std::string b;
};

int main(int, char *[]) {
    flecs::world ecs;

    // Register reflection for std::string
    ecs.component<std::string>()
        .opaque(flecs::String) // Opaque type that maps to string
            .serialize([](const flecs::serializer *s, const std::string *data) {
                const char *str = data->c_str();
                return s->value(flecs::String, &str); // Forward to serializer
            })
            .assign_string([](std::string* data, const char *value) {
                *data = value; // Assign new value to std::string
            });

    // Register component with std::string members
    ecs.component<StringComponent>()
        .member<std::string>("a")
        .member<std::string>("b");

    // Create value & serialize it
    StringComponent v = {"foo", "bar"};
    std::cout <<  ecs.to_json(&v) << std::endl;

    // Deserialize new strings into value
    ecs.from_json(&v, "{\"a\": \"hello\", \"b\": \"world\"}");
    std::cout << "{a: " << v.a << ", b: " << v.b << "}" << std::endl;

    // Output:
    //   {"a": "foo", "b": "bar"}
    //   {a: "hello", b: "world"}
}
