#include <ser_std_vector.h>
#include <iostream>
#include <string>
#include <vector>

// This example shows how to serialize a component with std::vectors

struct VectorComponent {
    std::vector<int32_t> ints;
    std::vector<std::string> strings;
};

// Reusable reflection support for std::vector
template <typename Elem, typename Vector = std::vector<Elem>> 
flecs::opaque<Vector, Elem> std_vector_support(flecs::world& world) {
    return flecs::opaque<Vector, Elem>()
        .as_type(world.vector<Elem>())

        // Forward elements of std::vector value to serializer
        .serialize([](const flecs::serializer *s, const Vector *data) {
            for (const auto& el : *data) {
                s->value(el);
            }
            return 0;
        })

        // Return vector count
        .count([](const Vector *data) {
            return data->size();
        })

        // Resize contents of vector
        .resize([](Vector *data, size_t size) {
            data->resize(size);
        })

        // Ensure element exists, return pointer
        .ensure_element([](Vector *data, size_t elem) {
            if (data->size() <= elem) {
                data->resize(elem + 1);
            }

            return &data->data()[elem];
        });
}

int main(int, char *[]) {
    flecs::world ecs;

    // Register reflection for std::string
    ecs.component<std::string>()
        .opaque(flecs::String) // Maps to string
            .serialize([](const flecs::serializer *s, const std::string *data) {
                const char *str = data->c_str();
                return s->value(flecs::String, &str); // Forward to serializer
            })
            .assign_string([](std::string* data, const char *value) {
                *data = value; // Assign new value to std::string
            });

    // Register reflection for std::vector<int>
    ecs.component<std::vector<int>>()
        .opaque(std_vector_support<int>);

    // Register reflection for std::vector<std::string>
    ecs.component<std::vector<std::string>>()
        .opaque(std_vector_support<std::string>);

    // Register component with std::vector members
    ecs.component<VectorComponent>()
        .member<std::vector<int>>("ints")
        .member<std::vector<std::string>>("strings");

    // Create value & serialize it to JSON
    VectorComponent v = {{1, 2, 3}, {"foo", "bar"}};
    std::cout << ecs.to_json(&v) << std::endl;

    // Deserialize new values from JSON into value
    ecs.from_json(&v, 
        "{\"ints\": [4, 5], \"strings\":[\"hello\", \"flecs\", \"reflection\"]}");

    // Serialize again
    std::cout << ecs.to_json(&v) << std::endl;

    // Output:
    //   {"ints":[1, 2, 3], "strings":["foo", "bar"]}
    //   {"ints":[4, 5], "strings":["hello", "flecs", "reflection"]}
}
