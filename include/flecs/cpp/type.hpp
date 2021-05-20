
#ifdef FLECS_DEPRECATED
#include "../addons/deprecated/type.hpp"
#else
template <typename Base>
class type_deprecated { };
#endif

namespace flecs 
{

////////////////////////////////////////////////////////////////////////////////
//// A collection of component ids used to describe the contents of a table
////////////////////////////////////////////////////////////////////////////////

class type final : public type_deprecated<type> {
public:
    explicit type(world_t *world, const char *name = nullptr, const char *expr = nullptr)
    { 
        ecs_type_desc_t desc = {};
        desc.entity.name = name;
        desc.ids_expr = expr;
        m_entity = flecs::entity_view(world, ecs_type_init(world, &desc));
        sync_from_flecs();
    }

    explicit type(world_t *world, type_t t)
        : m_entity( world, 0 )
        , m_type( t )
        , m_normalized( t ) { }

    type(type_t t)
        : m_entity( 0 )
        , m_type( t )
        , m_normalized( t ) { }

    type& add(const type& t) {
        m_type = ecs_type_add(world().c_ptr(), m_type, t.id());
        m_normalized = ecs_type_merge(world().c_ptr(), m_normalized, t.c_ptr(), nullptr);
        sync_from_me();
        return *this;
    }

    type& add(entity_t e) {
        m_type = ecs_type_add(world().c_ptr(), m_type, e);
        m_normalized = ecs_type_add(world().c_ptr(), m_normalized, e);
        sync_from_me();
        return *this;
    }

    template <typename T>
    type& add() {
        return this->add(_::cpp_type<T>::id(world().c_ptr()));
    }

    type& add(entity_t relation, entity_t object) {
        return this->add(ecs_pair(relation, object));
    }

    template <typename Relation, typename Object>
    type& add() {
        return this->add<Relation>(_::cpp_type<Object>::id(world().c_ptr()));
    }

    type& is_a(entity_t object) {
        return this->add(flecs::IsA, object);
    }

    type& child_of(entity_t object) {
        return this->add(flecs::ChildOf, object);
    }    

    template <typename Relation>
    type& add(entity_t object) {
        return this->add(_::cpp_type<Relation>::id(world().c_ptr()), object);
    }     

    template <typename Object>
    type& add_object(entity_t relation) {
        return this->add(relation, _::cpp_type<Object>::id(world().c_ptr()));
    }

    flecs::string str() const {
        char *str = ecs_type_str(world().c_ptr(), m_type);
        return flecs::string(str);
    }

    type_t c_ptr() const {
        return m_type;
    }

    flecs::entity_t id() const { 
        return m_entity.id(); 
    }

    flecs::world world() const { 
        return m_entity.world();
    } 

    type_t c_normalized() const {
        return m_normalized;
    }

    void enable() const {
        ecs_enable(world().c_ptr(), id(), true);
    }

    void disable() const {
        ecs_enable(world().c_ptr(), id(), false);
    }

    flecs::vector<entity_t> vector() {
        return flecs::vector<entity_t>( const_cast<ecs_vector_t*>(m_normalized));
    }

private:
    void sync_from_me() {
        EcsType *tc = static_cast<EcsType*>(
            ecs_get_mut_w_id(world().c_ptr(), id(), ecs_id(EcsType), NULL));
        if (tc) {
            tc->type = m_type;
            tc->normalized = m_normalized;
        }
    }

    void sync_from_flecs() {
        EcsType *tc = static_cast<EcsType*>(
            ecs_get_mut_w_id(world().c_ptr(), id(), ecs_id(EcsType), NULL));
        if (tc) {
            m_type = tc->type;
            m_normalized = tc->normalized;
        }
    }   

    flecs::entity_view m_entity;
    type_t m_type;
    type_t m_normalized;
};

} // namespace flecs
