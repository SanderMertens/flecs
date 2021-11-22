#pragma once

namespace flecs {

using snapshot_t = ecs_snapshot_t;

struct snapshot;

template <typename T>
struct snapshot_m : mixin<T> { };

/** Snapshot mixin for flecs::world */
template <>
struct snapshot_m<flecs::world> : mixin<flecs::world> {
  void init() { }

/** Create a snapshot.
 */
template <typename... Args>
flecs::snapshot snapshot(Args &&... args) const;
};

using snapshot_m_world = snapshot_m<flecs::world>;

}
