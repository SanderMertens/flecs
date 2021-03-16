
namespace flecs 
{

////////////////////////////////////////////////////////////////////////////////
//// A collection of component ids used to describe the contents of a table
////////////////////////////////////////////////////////////////////////////////

class type final : entity {
public:
    explicit type(const flecs::world& world, const char *name = nullptr, const char *expr = nullptr)
        : entity(world, ecs_new_type(world.c_ptr(), 0, name, expr))
    { 
        sync_from_flecs();
    }

    type(const flecs::world& world, type_t t)
        : entity( world.c_ptr(), 0 )
        , m_type( t )
        , m_normalized( t ) { }

    type(world_t *world, type_t t)
        : entity( world, 0 )
        , m_type( t )
        , m_normalized( t ) { }

    type& add(const type& t) {
        m_type = ecs_type_add(m_world, m_type, t.id());
        m_normalized = ecs_type_merge(m_world, m_normalized, t.c_ptr(), nullptr);
        sync_from_me();
        return *this;
    }

    type& add(const entity& e) {
        m_type = ecs_type_add(m_world, m_type, e.id());
        m_normalized = ecs_type_add(m_world, m_normalized, e.id());
        sync_from_me();
        return *this;
    }

    type& add_instanceof(const entity& e) {
        m_type = ecs_type_add(m_world, m_type, e.id() | ECS_INSTANCEOF);
        m_normalized = ecs_type_add(m_world, m_normalized, e.id() | ECS_INSTANCEOF);
        sync_from_me();
        return *this;
    }

    type& add_childof(const entity& e) {
        m_type = ecs_type_add(m_world, m_type, e.id() | ECS_CHILDOF);
        m_normalized = ecs_type_add(m_world, m_normalized, e.id() | ECS_CHILDOF);
        sync_from_me();
        return *this;
    }

    template <typename T, typename C>
    type& add_trait() {
        m_type = ecs_type_add(m_world, m_type, 
            ecs_trait(_::component_info<C>::id(m_world),
                      _::component_info<T>::id(m_world)));

        m_normalized = ecs_type_add(m_world, m_normalized, 
            ecs_trait(_::component_info<C>::id(m_world),
                      _::component_info<T>::id(m_world)));
        
        sync_from_me();
        return *this;
    }

    template <typename T>
    type& add_trait(flecs::entity component) {
        m_type = ecs_type_add(m_world, m_type, 
            ecs_trait(component.id(),
                      _::component_info<T>::id(m_world)));

        m_normalized = ecs_type_add(m_world, m_normalized, 
            ecs_trait(component.id(),
                      _::component_info<T>::id(m_world)));
        
        sync_from_me();
        return *this;
    }

    type& add_trait(flecs::entity trait, flecs::entity component) {
        m_type = ecs_type_add(m_world, m_type, 
            ecs_trait(component.id(), trait.id()));

        m_normalized = ecs_type_add(m_world, m_normalized, 
            ecs_trait(component.id(), trait.id()));
        
        sync_from_me();
        return *this;
    }      

    template <typename C>
    type& add_trait_tag(flecs::entity trait) {
        m_type = ecs_type_add(m_world, m_type, 
            ecs_trait(_::component_info<C>::id(), trait.id()));

        m_normalized = ecs_type_add(m_world, m_normalized, 
            ecs_trait(_::component_info<C>::id(), trait.id()));
        
        sync_from_me();
        return *this;
    }            

    template <typename ... Components>
    type& add() {
        std::stringstream str;
        if (!_::pack_args_to_string<Components...>(m_world, str)) {
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
        }

        std::string expr = str.str();
        ecs_type_t t = ecs_type_from_str(m_world, expr.c_str());
        m_type = ecs_type_merge(m_world, m_type, t, nullptr);
        m_normalized = ecs_type_merge(m_world, m_normalized, t, nullptr);
        sync_from_me();

        return *this;
    }    

    std::string str() const {
        char *str = ecs_type_str(m_world, m_type);
        std::string result(str);
        ecs_os_free(str);
        return result;
    }

    type_t c_ptr() const {
        return m_type;
    }

    // Expose entity id without making the entity class public.
    entity_t id() const {
        return m_id;
    }

    type_t c_normalized() const {
        return m_normalized;
    }

    void enable() const {
        ecs_assert(m_id != 0, ECS_INVALID_OPERATION, NULL);
        ecs_enable(m_world, m_id, true);
    }

    void disable() const {
        ecs_assert(m_id != 0, ECS_INVALID_OPERATION, NULL);
        ecs_enable(m_world, m_id, false);
    }

    flecs::vector<entity_t> vector() {
        return flecs::vector<entity_t>( (ecs_vector_t*)m_normalized );
    }

private:
    void sync_from_me() {
        ecs_assert(m_id != 0, ECS_INVALID_OPERATION, NULL);
        EcsType *tc = ecs_get_mut(m_world, m_id, EcsType, NULL);
        if (tc) {
            tc->type = m_type;
            tc->normalized = m_normalized;
        }
    }

    void sync_from_flecs() {
        ecs_assert(m_id != 0, ECS_INVALID_OPERATION, NULL);
        EcsType *tc = ecs_get_mut(m_world, m_id, EcsType, NULL);
        if (tc) {
            m_type = tc->type;
            m_normalized = tc->normalized;
        }
    }    

    type_t m_type;
    type_t m_normalized;
};

} // namespace flecs
