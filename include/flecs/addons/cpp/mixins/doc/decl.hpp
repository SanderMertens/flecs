#pragma once

namespace flecs {

namespace doc {
    using Description = EcsDocDescription;
    static const flecs::entity_t Brief = EcsDocBrief;
    static const flecs::entity_t Detail = EcsDocDetail;
    static const flecs::entity_t Link = EcsDocLink;
}

template <typename T>
struct doc_m : mixin<T> { };

/** Doc mixin for flecs::world */
template <>
struct doc_m<flecs::world> : mixin<flecs::world> {
  void init();
};

using doc_m_world = doc_m<flecs::world>;

}
