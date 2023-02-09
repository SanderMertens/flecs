
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
    auto ts = type_support(world);

    ecs_opaque_desc_t desc = {};
    desc.entity = m_id;
    desc.type.as_type = ts.as_type;
    desc.type.serialize = 
        reinterpret_cast<flecs::serialize_t>(ts.serialize);
    desc.type.assign_bool = 
        reinterpret_cast<decltype(desc.type.assign_bool)>(ts.assign_bool);
    desc.type.assign_char = 
        reinterpret_cast<decltype(desc.type.assign_char)>(ts.assign_char);
    desc.type.assign_int = 
        reinterpret_cast<decltype(desc.type.assign_int)>(ts.assign_int);
    desc.type.assign_uint = 
        reinterpret_cast<decltype(desc.type.assign_uint)>(ts.assign_uint);
    desc.type.assign_float = 
        reinterpret_cast<decltype(desc.type.assign_float)>(ts.assign_float);
    desc.type.assign_string = 
        reinterpret_cast<decltype(desc.type.assign_string)>(ts.assign_string);
    desc.type.assign_entity = 
        reinterpret_cast<decltype(desc.type.assign_entity)>(ts.assign_entity);
    desc.type.assign_null = 
        reinterpret_cast<decltype(desc.type.assign_null)>(ts.assign_null);
    desc.type.clear = 
        reinterpret_cast<decltype(desc.type.clear)>(ts.clear);
    desc.type.ensure_element = 
        reinterpret_cast<decltype(desc.type.ensure_element)>(ts.ensure_element);
    desc.type.ensure_member = 
        reinterpret_cast<decltype(desc.type.ensure_member)>(ts.ensure_member);
    desc.type.count = 
        reinterpret_cast<decltype(desc.type.count)>(ts.count);
    desc.type.resize = 
        reinterpret_cast<decltype(desc.type.resize)>(ts.resize);

    ecs_opaque_init(m_world, &desc);
    return *this;
}
