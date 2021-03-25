
namespace flecs
{

/** Deprecated functions */
template <typename Base>
class entity_builder_deprecated {
    using base_type = const Base;
public:
    template<typename T, typename C>
    ECS_DEPRECATED("use add<Relation, Object>")
    base_type& add_trait() const {
        ecs_add_pair(world(), id(),
            _::cpp_type<T>::id(world()), 
            _::cpp_type<C>::id(world()));
        return *base();
    }

    template<typename T>
    ECS_DEPRECATED("use add<Relation>(const entity&)")
    base_type& add_trait(const Base& c) const {
        ecs_add_pair(world(), id(), _::cpp_type<T>::id(world()), c.id());
        return *base();
    }

    template<typename C>
    ECS_DEPRECATED("use add_object<Object>(const entity&)")
    base_type& add_trait_tag(const Base& t) const {
        ecs_add_pair(world(), id(), t.id(), _::cpp_type<C>::id(world()));
        return *base();
    }

    ECS_DEPRECATED("use add(const entity&, const entity&)")
    base_type& add_trait(const Base& t, const Base& c) const {
        ecs_add_pair(world(), id(), t.id(), c.id());
        return *base();
    }

    template<typename T, typename C>
    ECS_DEPRECATED("use remove<Relation, Object>")
    base_type& remove_trait() const { 
        ecs_remove_pair(world(), id(),
            _::cpp_type<T>::id(world()), 
            _::cpp_type<C>::id(world()));
        return *base();
    }

    template<typename T>
    ECS_DEPRECATED("use remove<Relation>(const entity&)")
    base_type& remove_trait(const Base& c) const {
        ecs_remove_pair(world(), id(), _::cpp_type<T>::id(world()), c.id());
        return *base();        
    }

    template<typename C>
    ECS_DEPRECATED("use remove_object<Object>(const entity&)")
    base_type& remove_trait_tag(const Base& t) const {
        ecs_remove_pair(world(), id(), t.id(), _::cpp_type<C>::id(world()));
        return *base();        
    }

    ECS_DEPRECATED("use remove(const entity&, const entity&)")
    base_type& remove_trait(const Base& t, const Base& c) const {
        ecs_remove_pair(world(), id(), t.id(), c.id());
        return *base();        
    }

    template <typename T, typename C>
    ECS_DEPRECATED("use set<Relation, Object>(const Relation&)")
    const base_type& set_trait(const T& value) const {
        auto comp_id = _::cpp_type<T>::id(world());
        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

        ecs_set_ptr_w_entity(world(), id(), 
            ecs_pair(comp_id, _::cpp_type<C>::id(world())),
            sizeof(T), &value);

        return *base();
    }

    template <typename T>
    ECS_DEPRECATED("use set<Relation>(const entity&, const Relation&)")
    const base_type& set_trait(const T& value, const Base& c) const {
        auto comp_id = _::cpp_type<T>::id(world());
        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

        ecs_set_ptr_w_entity(world(), id(), 
            ecs_pair(comp_id, c.id()),
            sizeof(T), &value);

        return *base();
    }

    template <typename C>
    ECS_DEPRECATED("use set_object<Object>(const entity&, const Object&)")
    const base_type& set_trait_tag(const Base& t, const C& value) const {
        auto comp_id = _::cpp_type<C>::id(world());
        ecs_assert(_::cpp_type<C>::size() != 0, ECS_INVALID_PARAMETER, NULL);

        ecs_set_ptr_w_entity(world(), id(), 
            ecs_pair(t.id(), comp_id),
            sizeof(C), &value);

        return *base();
    }

    ECS_DEPRECATED("use add(flecs::ChildOf, parent)")
    base_type& add_childof(const Base& parent) const {
        ecs_add_entity(world(), id(), ECS_CHILDOF | parent.id());
        return *base();          
    }
    
    ECS_DEPRECATED("use remove(flecs::ChildOf, parent)")
    base_type& remove_childof(const Base& parent) const {
        ecs_remove_entity(world(), id(), ECS_CHILDOF | parent.id());
        return *base();
    }

    ECS_DEPRECATED("use add(flecs::IsA, base)")
    base_type& add_instanceof(const Base& base_entity) const {
        ecs_add_entity(world(), id(), ECS_INSTANCEOF | base_entity.id());
        return *base();        
    }

    ECS_DEPRECATED("use remove(flecs::IsA, base)")
    base_type& remove_instanceof(const Base& base_entity) const {
        ecs_remove_entity(world(), id(), ECS_INSTANCEOF | base_entity.id());
        return *base();
    }

private:
    const Base* base() const { return static_cast<const Base*>(this); }
    flecs::world_t* world() const { return base()->world().c_ptr(); }
    flecs::entity_t id() const { return base()->id(); }    
};

template<typename Base>
class entity_deprecated {
public:
    template<typename T, typename C>
    ECS_DEPRECATED("use get<Relation, Object>")
    const T* get_trait() const {
        auto comp_id = _::cpp_type<T>::id(world());
        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

        return static_cast<const T*>(ecs_get_w_entity(world(), id(), ecs_trait(
            _::cpp_type<C>::id(world()), comp_id)));
    }

    template<typename T>
    ECS_DEPRECATED("use get<Relation>(const entity&)")
    const T* get_trait(const Base& c) const {
        auto comp_id = _::cpp_type<T>::id(world());
        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

        return static_cast<const T*>(ecs_get_w_entity(world(), id(), ecs_trait(
            c.id(), comp_id)));
    }       

    template<typename C>
    ECS_DEPRECATED("use get_object<Object>(const entity&)")
    const C* get_trait_tag(const Base& t) const {
        auto comp_id = _::cpp_type<C>::id(world());
        ecs_assert(_::cpp_type<C>::size() != 0, ECS_INVALID_PARAMETER, NULL);

        return static_cast<const C*>(ecs_get_w_entity(world(), id(), ecs_trait(
            comp_id, t.id())));
    }

    ECS_DEPRECATED("use get(const entity&, const entity&)")
    const void* get_trait(const Base& t, const Base& c) const{
        return ecs_get_w_entity(world(), id(), ecs_trait(c.id(), t.id()));
    }

    template <typename T, typename C>
    ECS_DEPRECATED("use get_mut<Relation, Object>(bool)")
    T* get_trait_mut(bool *is_added = nullptr) const {
        auto t_id = _::cpp_type<T>::id(world());
        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

        return static_cast<T*>(
            ecs_get_mut_w_entity(
                world(), id(), ecs_trait(_::cpp_type<C>::id(world()), 
                    t_id), is_added));
    }

    template <typename T>
    ECS_DEPRECATED("use get_mut<Relation>(const entity&, bool)")
    T* get_trait_mut(const Base& c, bool *is_added = nullptr) const {
        auto comp_id = _::cpp_type<T>::id(world());
        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

        return static_cast<T*>(
            ecs_get_mut_w_entity(
                world(), id(), ecs_trait( comp_id, c.id()), is_added));
    }

    template <typename C>
    ECS_DEPRECATED("use get_mut_object<Object>(const entity&, bool)")
    C* get_trait_tag_mut(const Base& t, bool *is_added = nullptr) const {
        auto comp_id = _::cpp_type<C>::id(world());
        ecs_assert(_::cpp_type<C>::size() != 0, ECS_INVALID_PARAMETER, NULL);

        return static_cast<C*>(
            ecs_get_mut_w_entity(
                world(), id(), ecs_trait(comp_id, t.id()), is_added));
    }

    ECS_DEPRECATED("use has(flecs::ChildOf, parent)")
    bool has_childof(const Base& parent) const {
        return ecs_has_entity(world(), id(), ECS_CHILDOF | parent.id());
    }  

    ECS_DEPRECATED("use has(flecs::IsA, base)")
    bool has_instanceof(const Base& base) const {
        return ecs_has_entity(world(), id(), ECS_INSTANCEOF | base.id());
    }   

    template<typename T, typename C>
    ECS_DEPRECATED("use has<Relation, Object>")
    bool has_trait() const {
        return ecs_has_entity(world(), id(), ecs_trait(
            _::cpp_type<C>::id(world()), 
            _::cpp_type<T>::id(world())));
    }

    template<typename T>
    ECS_DEPRECATED("use has<Relation>(const flecs::entity&)")
    bool has_trait(const Base& component) const {
        return ecs_has_entity(world(), id(), ecs_trait(
            component.id(), _::cpp_type<T>::id(world())));
    }

    template<typename C>
    ECS_DEPRECATED("use has_object<Object>(const flecs::entity&)")
    bool has_trait_tag(const Base& trait) const {
        return ecs_has_entity(world(), id(), ecs_trait(
           _::cpp_type<C>::id(world()), trait.id()));
    }

    ECS_DEPRECATED("use has(const flecs::entity&, const flecs::entity&)")
    bool has_trait(const Base& trait, const Base& e) const {
        return ecs_has_entity(world(), id(), ecs_trait(
            e.id(), trait.id()));
    }

    ECS_DEPRECATED("use object()")
    Base lo() const {
        return Base(world(), ecs_entity_t_lo(id()));
    }

    ECS_DEPRECATED("use relation()")
    Base hi() const {
        return Base(world(), ecs_entity_t_hi(id()));
    }

    ECS_DEPRECATED("use flecs::id(relation, object)")
    static 
    Base comb(Base lo, Base hi) {
        return Base(lo.world(), 
            ecs_entity_t_comb(lo.id(), hi.id()));
    }

private:
    const Base* base() const { return static_cast<const Base*>(this); }
    flecs::world_t* world() const { return base()->world().c_ptr(); }
    flecs::entity_t id() const { return base()->id(); }     
};

}
