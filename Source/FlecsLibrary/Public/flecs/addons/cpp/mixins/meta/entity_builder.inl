/**
 * @file addons/cpp/mixins/meta/entity_builder.inl
 * @brief Meta entity builder mixin.
 */

/**
 * @memberof flecs::entity_view
 * @ingroup cpp_addons_meta
 * 
 * @{
 */

/** Make entity a unit */
const Self& unit(
    const char *symbol, 
    flecs::entity_t prefix = 0,
    flecs::entity_t base = 0,
    flecs::entity_t over = 0,
    int32_t factor = 0,
    int32_t power = 0) const
{
    ecs_unit_desc_t desc = {};
    desc.entity = this->id_;
    desc.symbol = const_cast<char*>(symbol); /* safe, will be copied in */
    desc.base = base;
    desc.over = over;
    desc.prefix = prefix;
    desc.translation.factor = factor;
    desc.translation.power = power;
    ecs_unit_init(this->world(), &desc);

    return to_base();
}

/** Make entity a derived unit */
const Self& unit( 
    flecs::entity_t prefix = 0,
    flecs::entity_t base = 0,
    flecs::entity_t over = 0,
    int32_t factor = 0,
    int32_t power = 0) const
{
    ecs_unit_desc_t desc = {};
    desc.entity = this->id_;
    desc.base = base;
    desc.over = over;
    desc.prefix = prefix;
    desc.translation.factor = factor;
    desc.translation.power = power;
    ecs_unit_init(this->world(), &desc);

    return to_base();
}

/** Make entity a derived unit */
const Self& unit_prefix( 
    const char *symbol,
    int32_t factor = 0,
    int32_t power = 0) const
{
    ecs_unit_prefix_desc_t desc = {};
    desc.entity = this->id_;
    desc.symbol = const_cast<char*>(symbol); /* safe, will be copied in */
    desc.translation.factor = factor;
    desc.translation.power = power;
    ecs_unit_prefix_init(this->world(), &desc);

    return to_base();
}

/** Add quantity to unit */
const Self& quantity(flecs::entity_t quantity) const {
    ecs_add_pair(this->world(), this->id(), flecs::Quantity, quantity);
    return to_base();
}

/** Make entity a unity prefix */
template <typename Quantity>
const Self& quantity() const {
    return this->quantity(_::type<Quantity>::id(this->world()));
}

/** Make entity a quantity */
const Self& quantity() const {
    ecs_add_id(this->world(), this->id(), flecs::Quantity);
    return to_base();
}

/** @} */
