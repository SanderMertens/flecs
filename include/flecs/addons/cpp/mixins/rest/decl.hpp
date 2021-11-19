#pragma once

namespace flecs {

/* REST components */
namespace rest {
    using Rest = EcsRest;
}

template <typename T>
struct rest_m : mixin<T> { };

/** Rest mixin for flecs::world */
template <>
struct rest_m<flecs::world> : mixin<flecs::world> {
  void init();
};

using rest_m_world = rest_m<flecs::world>;

}
