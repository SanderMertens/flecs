
/** Register opaque type interface */
template <typename Func>
component& opaque(const Func& type_support) {
    flecs::world world(world_);
    auto ts = type_support(world);
    ts.desc.entity = _::type<T>::id(world_);
    ecs_opaque_init(world_, &ts.desc);
    return *this;
}

flecs::opaque<T> opaque(flecs::entity_t as_type) {
    return flecs::opaque<T>(world_).as_type(as_type);
}

flecs::opaque<T> opaque(flecs::entity as_type) {
    return this->opaque(as_type.id());
}

flecs::opaque<T> opaque(flecs::untyped_component as_type) {
    return this->opaque(as_type.id());
}

/** Return opaque type builder for collection type */
template <typename ElemType>
flecs::opaque<T, ElemType> opaque(flecs::id_t as_type) {
    return flecs::opaque<T, ElemType>(world_).as_type(as_type);
}

/** Add constant. */
component<T>& constant(const char *name, T value) {
    int32_t v = static_cast<int32_t>(value);
    untyped_component::constant(name, v);
    return *this;
}
