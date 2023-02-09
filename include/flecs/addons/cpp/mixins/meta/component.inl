
/** Register opaque type interface */
template <typename Func>
component& opaque(const Func& type_support) {
    flecs::world world(m_world);
    auto ts = type_support(world);
    ts.desc.entity = _::cpp_type<T>::id(m_world);
    ecs_opaque_init(m_world, &ts.desc);
    return *this;
}

/** Return opaque type builder */
flecs::opaque<T> opaque() {
    return flecs::opaque<T>(m_world);
}

/** Return opaque type builder for collection type */
template <typename ElemType>
flecs::opaque<T, ElemType> opaque() {
    return flecs::opaque<T, ElemType>(m_world);
}
