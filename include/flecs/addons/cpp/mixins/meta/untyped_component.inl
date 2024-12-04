/**
 * @file addons/cpp/mixins/meta/untyped_component.inl
 * @brief Meta component mixin.
 */

/**
 * @memberof flecs::component
 * @ingroup cpp_addons_meta
 * 
 * @{
 */

private:

/** Private method that adds member to component. */
untyped_component& internal_member(
    flecs::entity_t type_id, 
    flecs::entity_t unit, 
    const char *name, 
    int32_t count = 0, 
    size_t offset = 0, 
    bool use_offset = false) 
{
    ecs_entity_desc_t desc = {};
    desc.name = name;
    desc.parent = id_;
    ecs_entity_t eid = ecs_entity_init(world_, &desc);
    ecs_assert(eid != 0, ECS_INTERNAL_ERROR, NULL);

    flecs::entity e(world_, eid);

    Member m = {};
    m.type = type_id;
    m.unit = unit;
    m.count = count;
    m.offset = static_cast<int32_t>(offset);
    m.use_offset = use_offset;
    e.set<Member>(m);

    return *this;
}

public: 

/** Add member with unit. */
untyped_component& member(
    flecs::entity_t type_id, 
    flecs::entity_t unit, 
    const char *name, 
    int32_t count = 0) 
{
    return internal_member(type_id, unit, name, count, 0, false);
}

/** Add member with unit, count and offset. */
untyped_component& member(
    flecs::entity_t type_id, 
    flecs::entity_t unit, 
    const char *name, 
    int32_t count, 
    size_t offset) 
{
    return internal_member(type_id, unit, name, count, offset, true);
}

/** Add member. */
untyped_component& member(
    flecs::entity_t type_id, 
    const char* name,
    int32_t count = 0) 
{
    return member(type_id, 0, name, count);
}

/** Add member with count and offset. */
untyped_component& member(
    flecs::entity_t type_id, 
    const char* name, 
    int32_t count, 
    size_t offset) 
{
    return member(type_id, 0, name, count, offset);
}

/** Add member. */
template <typename MemberType>
untyped_component& member(
    const char *name,
    int32_t count = 0) 
{
    flecs::entity_t type_id = _::type<MemberType>::id(world_);
    return member(type_id, name, count);
}

/** Add member. */
template <typename MemberType>
untyped_component& member(
    const char *name,
    int32_t count, 
    size_t offset) 
{
    flecs::entity_t type_id = _::type<MemberType>::id(world_);
    return member(type_id, name, count, offset);
}

/** Add member with unit. */
template <typename MemberType>
untyped_component& member(
    flecs::entity_t unit,
    const char *name, 
    int32_t count = 0) 
{
    flecs::entity_t type_id = _::type<MemberType>::id(world_);
    return member(type_id, unit, name, count);
}

/** Add member with unit. */
template <typename MemberType>
untyped_component& member(
    flecs::entity_t unit,
    const char *name, 
    int32_t count, 
    size_t offset) 
{
    flecs::entity_t type_id = _::type<MemberType>::id(world_);
    return member(type_id, unit, name, count, offset);
}

/** Add member with unit. */
template <typename MemberType, typename UnitType>
untyped_component& member(
    const char *name,
    int32_t count = 0) 
{
    flecs::entity_t type_id = _::type<MemberType>::id(world_);
    flecs::entity_t unit_id = _::type<UnitType>::id(world_);
    return member(type_id, unit_id, name, count);
}

/** Add member with unit. */
template <typename MemberType, typename UnitType>
untyped_component& member(
    const char *name, 
    int32_t count, 
    size_t offset) 
{
    flecs::entity_t type_id = _::type<MemberType>::id(world_);
    flecs::entity_t unit_id = _::type<UnitType>::id(world_);
    return member(type_id, unit_id, name, count, offset);
}

/** Add member using pointer-to-member. */
template <typename MemberType, typename ComponentType, 
    typename RealType = typename std::remove_extent<MemberType>::type>
untyped_component& member(
    const char* name, 
    const MemberType ComponentType::* ptr) 
{
    flecs::entity_t type_id = _::type<RealType>::id(world_);
    size_t offset = reinterpret_cast<size_t>(&(static_cast<ComponentType*>(nullptr)->*ptr));
    return member(type_id, name, std::extent<MemberType>::value, offset);
}

/** Add member with unit using pointer-to-member. */
template <typename MemberType, typename ComponentType, 
    typename RealType = typename std::remove_extent<MemberType>::type>
untyped_component& member(
    flecs::entity_t unit, 
    const char* name, 
    const MemberType ComponentType::* ptr) 
{
    flecs::entity_t type_id = _::type<RealType>::id(world_);
    size_t offset = reinterpret_cast<size_t>(&(static_cast<ComponentType*>(nullptr)->*ptr));
    return member(type_id, unit, name, std::extent<MemberType>::value, offset);
}

/** Add member with unit using pointer-to-member. */
template <typename UnitType, typename MemberType, typename ComponentType, 
    typename RealType = typename std::remove_extent<MemberType>::type>
untyped_component& member(
    const char* name, 
    const MemberType ComponentType::* ptr) 
{
    flecs::entity_t type_id = _::type<RealType>::id(world_);
    flecs::entity_t unit_id = _::type<UnitType>::id(world_);
    size_t offset = reinterpret_cast<size_t>(&(static_cast<ComponentType*>(nullptr)->*ptr));
    return member(type_id, unit_id, name, std::extent<MemberType>::value, offset);
}

/** Add constant. */
untyped_component& constant(
    const char *name,
    int32_t value) 
{
    ecs_add_id(world_, id_, _::type<flecs::Enum>::id(world_));

    ecs_entity_desc_t desc = {};
    desc.name = name;
    desc.parent = id_;
    ecs_entity_t eid = ecs_entity_init(world_, &desc);
    ecs_assert(eid != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_set_id(world_, eid, 
        ecs_pair(flecs::Constant, flecs::I32), sizeof(int32_t),
        &value);

    return *this;
}

/** Add bitmask constant. */
untyped_component& bit(
    const char *name, 
    uint32_t value) 
{
    ecs_add_id(world_, id_, _::type<flecs::Bitmask>::id(world_));

    ecs_entity_desc_t desc = {};
    desc.name = name;
    desc.parent = id_;
    ecs_entity_t eid = ecs_entity_init(world_, &desc);
    ecs_assert(eid != 0, ECS_INTERNAL_ERROR, NULL);

    ecs_set_id(world_, eid, 
        ecs_pair(flecs::Constant, flecs::U32), sizeof(uint32_t),
        &value);

    return *this;
}

/** Register array metadata for component */
template <typename Elem>
untyped_component& array(
    int32_t elem_count) 
{
    ecs_array_desc_t desc = {};
    desc.entity = id_;
    desc.type = _::type<Elem>::id(world_);
    desc.count = elem_count;
    ecs_array_init(world_, &desc);
    return *this;
}

/** Add member value range */
untyped_component& range(
    double min,
    double max) 
{
    const flecs::member_t *m = ecs_cpp_last_member(world_, id_);
    if (!m) {
        return *this;
    }

    flecs::world w(world_);
    flecs::entity me = w.entity(m->member);

    // Don't use C++ ensure because Unreal defines a macro called ensure
    flecs::MemberRanges *mr = static_cast<flecs::MemberRanges*>(
        ecs_ensure_id(w, me, w.id<flecs::MemberRanges>()));
    mr->value.min = min;
    mr->value.max = max;
    me.modified<flecs::MemberRanges>();
    return *this;
}

/** Add member warning range */
untyped_component& warning_range(
    double min,
    double max) 
{
    const flecs::member_t *m = ecs_cpp_last_member(world_, id_);
    if (!m) {
        return *this;
    }

    flecs::world w(world_);
    flecs::entity me = w.entity(m->member);

    // Don't use C++ ensure because Unreal defines a macro called ensure
    flecs::MemberRanges *mr = static_cast<flecs::MemberRanges*>(
        ecs_ensure_id(w, me, w.id<flecs::MemberRanges>()));
    mr->warning.min = min;
    mr->warning.max = max;
    me.modified<flecs::MemberRanges>();
    return *this;
}

/** Add member error range */
untyped_component& error_range(
    double min,
    double max) 
{
    const flecs::member_t *m = ecs_cpp_last_member(world_, id_);
    if (!m) {
        return *this;
    }

    flecs::world w(world_);
    flecs::entity me = w.entity(m->member);

    // Don't use C++ ensure because Unreal defines a macro called ensure
    flecs::MemberRanges *mr = static_cast<flecs::MemberRanges*>(ecs_ensure_id(
        w, me, w.id<flecs::MemberRanges>()));
    mr->error.min = min;
    mr->error.max = max;
    me.modified<flecs::MemberRanges>();
    return *this;
}

/** @} */
