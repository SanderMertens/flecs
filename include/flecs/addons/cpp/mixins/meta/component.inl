
/** Register custom reflection function for opaque type. */
component& serialize(flecs::id_t as_type, flecs::serialize<T> ser) {
    ecs_opaque_desc_t desc = {};

    /* Safe cast, from a function with a T* arg to a void* arg */
    desc.entity = m_id;
    desc.type.serialize = 
        reinterpret_cast<flecs::serialize_t>(ser);desc.type.as_type = as_type;
    ecs_opaque_init(m_world, &desc);
    return *this;
}

/** Register opaque type interface */
template <typename Func>
component& opaque(const Func& type_support) {
    flecs::world world(m_world);
    auto opaque = type_support(world);

    ecs_opaque_desc_t desc = {};
    desc.entity = m_id;
    desc.type.as_type = opaque.as_type;
    desc.type.serialize = 
        reinterpret_cast<flecs::serialize_t>(opaque.serialize);
    desc.type.assign_bool = 
        reinterpret_cast<decltype(desc.type.assign_bool)>(opaque.assign_bool);
    desc.type.assign_char = 
        reinterpret_cast<decltype(desc.type.assign_char)>(opaque.assign_char);
    desc.type.assign_int = 
        reinterpret_cast<decltype(desc.type.assign_int)>(opaque.assign_int);
    desc.type.assign_uint = 
        reinterpret_cast<decltype(desc.type.assign_uint)>(opaque.assign_uint);
    desc.type.assign_float = 
        reinterpret_cast<decltype(desc.type.assign_float)>(opaque.assign_float);
    desc.type.assign_string = 
        reinterpret_cast<decltype(desc.type.assign_string)>(opaque.assign_string);
    desc.type.assign_entity = 
        reinterpret_cast<decltype(desc.type.assign_entity)>(opaque.assign_entity);
    desc.type.assign_null = 
        reinterpret_cast<decltype(desc.type.assign_null)>(opaque.assign_null);
    desc.type.clear = 
        reinterpret_cast<decltype(desc.type.clear)>(opaque.clear);
    desc.type.ensure_element = 
        reinterpret_cast<decltype(desc.type.ensure_element)>(opaque.ensure_element);
    desc.type.ensure_member = 
        reinterpret_cast<decltype(desc.type.ensure_member)>(opaque.ensure_member);
    desc.type.count = 
        reinterpret_cast<decltype(desc.type.count)>(opaque.count);
    desc.type.resize = 
        reinterpret_cast<decltype(desc.type.resize)>(opaque.resize);

    ecs_opaque_init(m_world, &desc);
    return *this;
}
