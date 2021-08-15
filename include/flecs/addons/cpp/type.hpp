
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

template <typename Base>
class type_base : public type_deprecated<type> {
public:
    explicit type_base(
        world_t *world, const char *name = nullptr, const char *expr = nullptr)
    { 
        ecs_type_desc_t desc = {};
        desc.entity.name = name;
        desc.ids_expr = expr;
        m_entity = flecs::entity(world, ecs_type_init(world, &desc));
        sync_from_flecs();
    }

    explicit type_base(world_t *world, type_t t)
        : m_entity( world, static_cast<flecs::id_t>(0) )
        , m_type( t )
        , m_normalized( t ) { }

    type_base(type_t t)
        : m_type( t )
        , m_normalized( t ) { }

    Base& add(const Base& t) {
        m_type = ecs_type_add(world(), m_type, t.id());
        m_normalized = ecs_type_merge(world(), m_normalized, t, nullptr);
        sync_from_me();
        return *this;
    }

    Base& add(id_t e) {
        m_type = ecs_type_add(world(), m_type, e);
        m_normalized = ecs_type_add(world(), m_normalized, e);
        sync_from_me();
        return *this;
    }

    template <typename T>
    Base& add() {
        return this->add(_::cpp_type<T>::id(world()));
    }

    Base& add(entity_t relation, entity_t object) {
        return this->add(ecs_pair(relation, object));
    }

    template <typename Relation, typename Object>
    Base& add() {
        return this->add<Relation>(_::cpp_type<Object>::id(world()));
    }

    Base& is_a(entity_t object) {
        return this->add(flecs::IsA, object);
    }

    Base& child_of(entity_t object) {
        return this->add(flecs::ChildOf, object);
    }    

    template <typename Relation>
    Base& add(entity_t object) {
        return this->add(_::cpp_type<Relation>::id(world()), object);
    }     

    template <typename Object>
    Base& add_w_object(entity_t relation) {
        return this->add(relation, _::cpp_type<Object>::id(world()));
    }

    bool has(id_t id) {
        return ecs_type_has_id(world(), m_normalized, id, false);
    }

    bool has(id_t relation, id_t object) {
        return ecs_type_has_id(world(), m_normalized, 
            ecs_pair(relation, object), false);
    }    

    template <typename T>
    bool has() {
        return this->has(_::cpp_type<T>::id(world()));
    }

    template <typename Relation, typename Object>
    bool has() {
        return this->has(_::cpp_type<flecs::pair<Relation, Object>>::id(world()));
    }

    template <typename T>
    Base& component() {
        component_for_id<T>(world(), m_entity);
        return *this;
    }

    flecs::string str() const {
        char *str = ecs_type_str(world(), m_type);
        return flecs::string(str);
    }

    type_t c_ptr() const {
        return m_type;
    }

    flecs::id_t id() const { 
        return m_entity.id(); 
    }

    flecs::entity entity() const {
        return m_entity;
    }

    flecs::world world() const { 
        return m_entity.world();
    } 

    type_t c_normalized() const {
        return m_normalized;
    }

    void enable() const {
        ecs_enable(world(), id(), true);
    }

    void disable() const {
        ecs_enable(world(), id(), false);
    }

    flecs::vector<flecs::id_t> vector() {
        return flecs::vector<flecs::id_t>( const_cast<ecs_vector_t*>(m_normalized));
    }

    flecs::id get(int32_t index) {
        return flecs::id(world(), vector().get(index));
    }

    /* Implicit conversion to type_t */
    operator type_t() const { return m_normalized; }

    operator Base&() { return *static_cast<Base*>(this); }

private:
    void sync_from_me() {
        if (!id()) {
            return;
        }

        EcsType *tc = ecs_get_mut(world(), id(), EcsType, NULL);
        ecs_assert(tc != NULL, ECS_INTERNAL_ERROR, NULL);
        tc->type = m_type;
        tc->normalized = m_normalized;
        ecs_modified(world(), id(), EcsType);

    }

    void sync_from_flecs() {
        if (!id()) {
            return;
        }

        EcsType *tc = ecs_get_mut(world(), id(), EcsType, NULL);
        ecs_assert(tc != NULL, ECS_INTERNAL_ERROR, NULL);            
        m_type = tc->type;
        m_normalized = tc->normalized;
        ecs_modified(world(), id(), EcsType);
    }   

    flecs::entity m_entity;
    type_t m_type;
    type_t m_normalized;
};

class type : public type_base<type> { 
public:
    explicit type(
        world_t *world, const char *name = nullptr, const char *expr = nullptr)
    : type_base(world, name, expr) { }

    explicit type(world_t *world, type_t t) : type_base(world, t) { }

    type(type_t t) : type_base(t) { }
};

class pipeline : public type_base<pipeline> {
public:
    explicit pipeline(world_t *world, const char *name) : type_base(world, name)
    { 
        this->entity().add(flecs::Pipeline);
    }
};

} // namespace flecs
