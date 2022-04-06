
/** Add member. */
untyped_component& member(flecs::entity_t type_id, const char *name, int32_t count = 0, size_t offset = 0) {
    ecs_entity_desc_t desc = {};
    desc.name = name;
    desc.add[0] = ecs_pair(flecs::ChildOf, m_id);
    ecs_entity_t eid = ecs_entity_init(m_world, &desc);
    ecs_assert(eid != 0, ECS_INTERNAL_ERROR, NULL);

    flecs::entity e(m_world, eid);

    Member m = {};
    m.type = type_id;
    m.count = count;
    m.offset = static_cast<int32_t>(offset);
    e.set<Member>(m);

    return *this;
}

/** Add member with unit. */
untyped_component& member(flecs::entity_t type_id, flecs::entity_t unit, const char *name, int32_t count = 0, size_t offset = 0) {
    ecs_entity_desc_t desc = {};
    desc.name = name;
    desc.add[0] = ecs_pair(flecs::ChildOf, m_id);
    ecs_entity_t eid = ecs_entity_init(m_world, &desc);
    ecs_assert(eid != 0, ECS_INTERNAL_ERROR, NULL);

    flecs::entity e(m_world, eid);

    Member m = {};
    m.type = type_id;
    m.unit = unit;
    m.count = count;
    m.offset = static_cast<int32_t>(offset);
    e.set<Member>(m);

    return *this;
}

/** Add member. */
template <typename MemberType>
untyped_component& member(const char *name, int32_t count = 0, size_t offset = 0) {
    flecs::entity_t type_id = _::cpp_type<MemberType>::id(m_world);
    return member(type_id, name, count, offset);
}

/** Add member with unit. */
template <typename MemberType>
untyped_component& member(flecs::entity_t unit, const char *name, int32_t count = 0, size_t offset = 0) {
    flecs::entity_t type_id = _::cpp_type<MemberType>::id(m_world);
    return member(type_id, unit, name, count, offset);
}

/** Add member with unit. */
template <typename MemberType, typename UnitType>
untyped_component& member(const char *name, int32_t count = 0, size_t offset = 0) {
    flecs::entity_t type_id = _::cpp_type<MemberType>::id(m_world);
    flecs::entity_t unit_id = _::cpp_type<UnitType>::id(m_world);
    return member(type_id, unit_id, name, count, offset);
}

/** Add constant. */
untyped_component& constant(const char *name, int32_t value) {
    ecs_add_id(m_world, m_id, _::cpp_type<flecs::Enum>::id(m_world));

    ecs_entity_desc_t desc = {};
    desc.name = name;
    desc.add[0] = ecs_pair(flecs::ChildOf, m_id);
    ecs_entity_t eid = ecs_entity_init(m_world, &desc);
    ecs_assert(eid != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_set_id(m_world, eid, 
        ecs_pair(flecs::Constant, flecs::I32), sizeof(int32_t),
        &value);

    return *this;
}

/** Add bitmask constant. */
untyped_component& bit(const char *name, uint32_t value) {
    ecs_add_id(m_world, m_id, _::cpp_type<flecs::Bitmask>::id(m_world));

    ecs_entity_desc_t desc = {};
    desc.name = name;
    desc.add[0] = ecs_pair(flecs::ChildOf, m_id);
    ecs_entity_t eid = ecs_entity_init(m_world, &desc);
    ecs_assert(eid != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_set_id(m_world, eid, 
        ecs_pair(flecs::Constant, flecs::U32), sizeof(uint32_t),
        &value);

    return *this;
}
