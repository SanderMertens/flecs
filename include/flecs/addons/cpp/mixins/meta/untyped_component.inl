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

untyped_component& member(flecs::entity_t type_id, flecs::entity_t unit,
    const char *name, int32_t count = 0, size_t offset = SIZE_MAX)
{
    ecs_member_t m = {};
    m.name = name;
    m.type = type_id;
    m.unit = unit;
    m.count = count;
    m.use_offset = offset != SIZE_MAX;
    m.offset = m.use_offset ? static_cast<int32_t>(offset) : 0;
    ecs_struct_add_member(world_, id_, &m);
    return *this;
}

untyped_component& member(flecs::entity_t type_id, const char *name,
    int32_t count = 0, size_t offset = SIZE_MAX)
{
    return member(type_id, 0, name, count, offset);
}

template <typename MemberType, typename UnitType = void>
untyped_component& member(const char *name, int32_t count = 0, size_t offset = SIZE_MAX) {
    flecs::entity_t unit = 0;
    if constexpr (!std::is_void_v<UnitType>) {
        unit = _::type<UnitType>::id(world_);
    }
    return member(_::type<MemberType>::id(world_), unit, name, count, offset);
}

template <typename MemberType>
untyped_component& member(flecs::entity_t unit, const char *name,
    int32_t count = 0, size_t offset = SIZE_MAX)
{
    return member(_::type<MemberType>::id(world_), unit, name, count, offset);
}

template <typename MemberType, typename ComponentType,
    typename RealType = std::remove_extent_t<MemberType>>
untyped_component& member(const MemberType ComponentType::*ptr,
    const char *name, flecs::entity_t unit = 0)
{
    size_t offset = reinterpret_cast<size_t>(&(static_cast<ComponentType*>(nullptr)->*ptr));
    return member(_::type<RealType>::id(world_), unit, name, std::extent<MemberType>::value, offset);
}

template <typename UnitType, typename MemberType, typename ComponentType,
    typename RealType = std::remove_extent_t<MemberType>>
untyped_component& member(const MemberType ComponentType::*ptr, const char *name) {
    return member<MemberType, ComponentType, RealType>(ptr, name, _::type<UnitType>::id(world_));
}

/** Add a constant. */
template <typename T = int32_t>
untyped_component& constant(
    const char *name,
    T value)
{
    ecs_add_id(world_, id_, _::type<flecs::Enum>::id(world_));

    ecs_entity_desc_t desc = {};
    desc.name = name;
    desc.parent = id_;
    ecs_entity_t eid = ecs_entity_init(world_, &desc);
    ecs_assert(eid != 0, ECS_INTERNAL_ERROR, nullptr);

    ecs_set_id(world_, eid, 
        ecs_pair(flecs::Constant, _::type<T>::id(world_)), sizeof(T),
        &value);

    return *this;
}

/** Add a bitmask constant. */
template <typename T = uint32_t>
untyped_component& bit(
    const char *name, 
    T value)
{
    ecs_add_id(world_, id_, _::type<flecs::Bitmask>::id(world_));

    ecs_entity_desc_t desc = {};
    desc.name = name;
    desc.parent = id_;
    ecs_entity_t eid = ecs_entity_init(world_, &desc);
    ecs_assert(eid != 0, ECS_INTERNAL_ERROR, nullptr);

    ecs_set_id(world_, eid, 
        ecs_pair(flecs::Constant, _::type<T>::id(world_)), sizeof(T),
        &value);

    return *this;
}

/** Register array metadata for a component. */
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

untyped_component& range(double min, double max) {
    return set_member_range<&flecs::member_t::range, &flecs::MemberRanges::value>(min, max);
}

untyped_component& warning_range(double min, double max) {
    return set_member_range<&flecs::member_t::warning_range, &flecs::MemberRanges::warning>(min, max);
}

untyped_component& error_range(double min, double max) {
    return set_member_range<&flecs::member_t::error_range, &flecs::MemberRanges::error>(min, max);
}

private:
    template <auto MemberRange, auto EntityRange>
    untyped_component& set_member_range(double min, double max) {
        auto member = ecs_cpp_last_member(world_, id_);
        if (member) {
            member->*MemberRange = {min, max};
            auto member_entity = member->member;
            if (member_entity) {
                auto id = _::type<flecs::MemberRanges>::id(world_);
                auto ranges = static_cast<flecs::MemberRanges*>(
                    ecs_ensure_id(world_, member_entity, id, sizeof(flecs::MemberRanges)));
                ranges->*EntityRange = {min, max};
                ecs_modified_id(world_, member_entity, id);
            }
        }
        return *this;
    }

public:

/** @} */
