
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
    using U = typename std::underlying_type<T>::type;

    ecs_add_id(world_, id_, _::type<flecs::Enum>::id(world_));

    ecs_entity_desc_t desc = {};
    desc.name = name;
    desc.parent = id_;
    ecs_entity_t eid = ecs_entity_init(world_, &desc);
    ecs_assert(eid != 0, ECS_INTERNAL_ERROR, NULL);

    flecs::id_t pair = ecs_pair(flecs::Constant, _::type<U>::id(world_));
    U *ptr = static_cast<U*>(ecs_ensure_id(world_, eid, pair, sizeof(U)));
    *ptr = static_cast<U>(value);
    ecs_modified_id(world_, eid, pair);

    // If we're not using automatic enum reflection, manually set static data
    #ifdef FLECS_CPP_NO_ENUM_REFLECTION
    auto et = enum_type<T>(world_);
    et.register_constant(world_, static_cast<U>(value), eid);
    #endif

    return *this;
}
