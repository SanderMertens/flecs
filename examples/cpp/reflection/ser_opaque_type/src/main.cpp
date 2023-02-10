#include <ser_opaque_type.h>
#include <iostream>
#include <string>

// Use opaque reflection support to add a computed 'result' member to type
struct Sum {
    int32_t a;
    int32_t b;
};

int main(int, char *[]) {
    flecs::world ecs;

    // Register serialization support for opaque type
    ecs.component<Sum>()
        // Serialize as struct
        .opaque(ecs.component()
            .member<int32_t>("a")
            .member<int32_t>("b")
            .member<int32_t>("result"))

        // Forward struct members to serializer
        .serialize([](const flecs::serializer *s, const Sum *data) {
            s->member("x");
            s->value(data->a);
            s->member("y");
            s->value(data->b);

            s->member("result");
            s->value(data->a + data->b); // Serialize fake member
            return 0;
        })

        // Return address for requested member
        .ensure_member([](Sum *dst, const char *member) -> void* {
            if (!strcmp(member, "a")) {
                return &dst->a;
            } else if (!strcmp(member, "b")) {
                return &dst->b;
            } else {
                return nullptr; // We can't serialize into fake result member
            }
        });

    // Serialize value of Sum to JSON
    Sum v = {10, 20};
    std::cout << ecs.to_json(&v) << std::endl;

    // Deserialize new value into Sum
    ecs.from_json(&v, "{\"a\": 20, \"b\": 22}");

    // Serialize value again
    std::cout << ecs.to_json(&v) << std::endl;

    // Output
    //  {"a":10, "b":20, "result":30}
    //  {"a":22, "b":20, "result":42}
}
