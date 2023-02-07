
/** Register custom reflection function for component. */
component& serialize(flecs::id_t as_type, flecs::serialize<T> ser) {
    ecs_opaque_desc_t desc = {};

    /* Safe cast, from a function with a T* arg to a void* arg */
    desc.entity = m_id;
    desc.type.serialize = reinterpret_cast<flecs::serialize_t>(ser);
    desc.type.as_type = as_type;
    ecs_opaque_init(m_world, &desc);
    return *this;
}
