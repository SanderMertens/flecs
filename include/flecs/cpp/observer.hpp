
namespace flecs 
{

template<typename ... Components>
class observer : public entity
{
public:
    explicit observer() 
        : entity() { }

    explicit observer(flecs::world_t *world, flecs::entity_t id)
        : entity(world, id) { }

    void ctx(void *ctx) {
        ecs_observer_desc_t desc = {};
        desc.entity.entity = m_id;
        desc.ctx = ctx;
        ecs_observer_init(m_world, &desc);
    }

    void* ctx() const {
        return ecs_get_observer_ctx(m_world, m_id);
    }
};

} // namespace flecs
