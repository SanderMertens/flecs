
namespace flecs 
{

template<typename ... Components>
class trigger : public entity
{
public:
    explicit trigger() 
        : entity() { }

    explicit trigger(flecs::world_t *world, flecs::entity_t id)
        : entity(world, id) { }

    void ctx(void *ctx) {
        ecs_trigger_desc_t desc = {};
        desc.entity.entity = m_id;
        desc.ctx = ctx;
        ecs_trigger_init(m_world, &desc);
    }

    void* ctx() const {
        return ecs_get_trigger_ctx(m_world, m_id);
    }
};

} // namespace flecs
