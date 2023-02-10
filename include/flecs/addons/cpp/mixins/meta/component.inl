
/** Register opaque type interface */
template <typename Func>
component& opaque(const Func& type_support) {
    flecs::world world(m_world);
    auto ts = type_support(world);
    ts.desc.entity = _::cpp_type<T>::id(m_world);
    ecs_opaque_init(m_world, &ts.desc);
    return *this;
}

flecs::opaque<T> opaque(flecs::entity_t as_type) {
    return flecs::opaque<T>(m_world).as_type(as_type);
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
    return flecs::opaque<T, ElemType>(m_world).as_type(as_type);
}
