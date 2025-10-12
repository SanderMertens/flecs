#include <ser_std_optional.h>
#include <iostream>
#include <optional>

template <typename T>
flecs::opaque<std::optional<T>, T> std_optional_support(flecs::world &world) {
  return flecs::opaque<std::optional<T>, T>()
      .as_type(world.vector<T>())
      .serialize([](const flecs::serializer *s, const std::optional<T> *data) {
        if (*data) {
          s->value(**data);
        }
        return 0;
      })
      .count(
          [](const std::optional<T> *data) -> size_t { return *data ? 1 : 0; })
      .resize([](std::optional<T> *data, size_t size) {
        switch (size) {
        case 0:
          *data = std::nullopt;
          break;
        case 1:
          if (!data->has_value()) {
            *data = T();
          }
          break;
        default:
          assert(false);
        }
      })
      .ensure_element([](std::optional<T> *data, size_t) {
        if (!data->has_value()) {
          *data = T();
        }
        return &data->value();
      });
}

int main(int, char *[]) {
  flecs::world ecs;

  // Register reflection for std::optional<int>
  ecs.component<std::optional<int>>()
    .opaque(std_optional_support<int>);

  std::optional<int> empty = std::nullopt;
  std::cout << ecs.to_json(&empty) << std::endl;

  std::optional<int> full = 1;
  std::cout << ecs.to_json(&full) << std::endl;

  std::optional<int> o;
  ecs.from_json(&o, "[2]");
  std::cout << ecs.to_json(&o) << std::endl;

  // Output:
  //   []
  //   [1]
  //   [2]
}
