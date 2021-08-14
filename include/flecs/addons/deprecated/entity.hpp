
namespace flecs
{

struct entity_builder_deprecated_tag { };

/** Deprecated functions */
template <typename Base>
class entity_builder_deprecated : public entity_builder_base<entity_builder_deprecated_tag, Base> {
public:
    template<typename T, typename C>
    ECS_DEPRECATED("use add<Relation, Object>")
    const Base& add_trait() const {
        ecs_add_pair(this->base_world(), this->base_id(),
            _::cpp_type<T>::id(this->base_world()), 
            _::cpp_type<C>::id(this->base_world()));
        return *this;
    }

    template<typename T>
    ECS_DEPRECATED("use add<Relation>(const entity&)")
    const Base& add_trait(const Base& c) const {
        ecs_add_pair(this->base_world(), this->base_id(), _::cpp_type<T>::id(this->base_world()), c.id());
        return *this;
    }

    template<typename C>
    ECS_DEPRECATED("use add_object<Object>(const entity&)")
    const Base& add_trait_tag(const Base& t) const {
        ecs_add_pair(this->base_world(), this->base_id(), t.id(), _::cpp_type<C>::id(this->base_world()));
        return *this;
    }

    ECS_DEPRECATED("use add(const entity&, const entity&)")
    const Base& add_trait(const Base& t, const Base& c) const {
        ecs_add_pair(this->base_world(), this->base_id(), t.id(), c.id());
        return *this;
    }

    template<typename T, typename C>
    ECS_DEPRECATED("use remove<Relation, Object>")
    const Base& remove_trait() const { 
        ecs_remove_pair(this->base_world(), this->base_id(),
            _::cpp_type<T>::id(this->base_world()), 
            _::cpp_type<C>::id(this->base_world()));
        return *this;
    }

    template<typename T>
    ECS_DEPRECATED("use remove<Relation>(const entity&)")
    const Base& remove_trait(const Base& c) const {
        ecs_remove_pair(this->base_world(), this->base_id(), _::cpp_type<T>::id(this->base_world()), c.id());
        return *this;        
    }

    template<typename C>
    ECS_DEPRECATED("use remove_object<Object>(const entity&)")
    const Base& remove_trait_tag(const Base& t) const {
        ecs_remove_pair(this->base_world(), this->base_id(), t.id(), _::cpp_type<C>::id(this->base_world()));
        return *this;        
    }

    ECS_DEPRECATED("use remove(const entity&, const entity&)")
    const Base& remove_trait(const Base& t, const Base& c) const {
        ecs_remove_pair(this->base_world(), this->base_id(), t.id(), c.id());
        return *this;        
    }

    template <typename T, typename C>
    ECS_DEPRECATED("use set<Relation, Object>(const Relation&)")
    const Base& set_trait(const T& value) const {
        auto comp_id = _::cpp_type<T>::id(this->base_world());
        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

        ecs_set_ptr_w_entity(this->base_world(), this->base_id(), 
            ecs_pair(comp_id, _::cpp_type<C>::id(this->base_world())),
            sizeof(T), &value);

        return *this;
    }

    template <typename T>
    ECS_DEPRECATED("use set<Relation>(const entity&, const Relation&)")
    const Base& set_trait(const T& value, const Base& c) const {
        auto comp_id = _::cpp_type<T>::id(this->base_world());
        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

        ecs_set_ptr_w_entity(this->base_world(), this->base_id(), 
            ecs_pair(comp_id, c.id()),
            sizeof(T), &value);

        return *this;
    }

    template <typename C>
    ECS_DEPRECATED("use set_object<Object>(const entity&, const Object&)")
    const Base& set_trait_tag(const Base& t, const C& value) const {
        auto comp_id = _::cpp_type<C>::id(this->base_world());
        ecs_assert(_::cpp_type<C>::size() != 0, ECS_INVALID_PARAMETER, NULL);

        ecs_set_ptr_w_entity(this->base_world(), this->base_id(), 
            ecs_pair(t.id(), comp_id),
            sizeof(C), &value);

        return *this;
    }

    ECS_DEPRECATED("use set(Func func)")
    template <typename T, typename Func>
    const Base& patch(const Func& func) const {
        auto comp_id = _::cpp_type<T>::id(this->base_world());

        ecs_assert(_::cpp_type<T>::size() != 0, 
            ECS_INVALID_PARAMETER, NULL);

        bool is_added;
        T *ptr = static_cast<T*>(ecs_get_mut_id(
            this->base_world(), this->base_id(), comp_id, &is_added));
        ecs_assert(ptr != NULL, ECS_INTERNAL_ERROR, NULL);

        func(*ptr);
        ecs_modified_w_entity(this->base_world(), this->base_id(), comp_id);

        return *this;
    }    
};

struct entity_deprecated_tag { };

template<typename Base>
class entity_deprecated : entity_builder_base<entity_deprecated_tag, Base> {
public:
    template<typename T, typename C>
    ECS_DEPRECATED("use get<Relation, Object>")
    const T* get_trait() const {
        auto comp_id = _::cpp_type<T>::id(this->base_world());
        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

        return static_cast<const T*>(ecs_get_id(this->base_world(), this->base_id(), ecs_trait(
            _::cpp_type<C>::id(this->base_world()), comp_id)));
    }

    template<typename T>
    ECS_DEPRECATED("use get<Relation>(const entity&)")
    const T* get_trait(const Base& c) const {
        auto comp_id = _::cpp_type<T>::id(this->base_world());
        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

        return static_cast<const T*>(ecs_get_id(this->base_world(), this->base_id(), ecs_trait(
            c.id(), comp_id)));
    }       

    template<typename C>
    ECS_DEPRECATED("use get_object<Object>(const entity&)")
    const C* get_trait_tag(const Base& t) const {
        auto comp_id = _::cpp_type<C>::id(this->base_world());
        ecs_assert(_::cpp_type<C>::size() != 0, ECS_INVALID_PARAMETER, NULL);

        return static_cast<const C*>(ecs_get_id(this->base_world(), this->base_id(), ecs_trait(
            comp_id, t.id())));
    }

    ECS_DEPRECATED("use get(const entity&, const entity&)")
    const void* get_trait(const Base& t, const Base& c) const{
        return ecs_get_id(this->base_world(), this->base_id(), ecs_trait(c.id(), t.id()));
    }

    template <typename T, typename C>
    ECS_DEPRECATED("use get_mut<Relation, Object>(bool)")
    T* get_trait_mut(bool *is_added = nullptr) const {
        auto t_id = _::cpp_type<T>::id(this->base_world());
        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

        return static_cast<T*>(
            ecs_get_mut_id(
                this->base_world(), this->base_id(), ecs_trait(_::cpp_type<C>::id(this->base_world()), 
                    t_id), is_added));
    }

    template <typename T>
    ECS_DEPRECATED("use get_mut<Relation>(const entity&, bool)")
    T* get_trait_mut(const Base& c, bool *is_added = nullptr) const {
        auto comp_id = _::cpp_type<T>::id(this->base_world());
        ecs_assert(_::cpp_type<T>::size() != 0, ECS_INVALID_PARAMETER, NULL);

        return static_cast<T*>(
            ecs_get_mut_id(
                this->base_world(), this->base_id(), ecs_trait( comp_id, c.id()), is_added));
    }

    template <typename C>
    ECS_DEPRECATED("use get_mut_object<Object>(const entity&, bool)")
    C* get_trait_tag_mut(const Base& t, bool *is_added = nullptr) const {
        auto comp_id = _::cpp_type<C>::id(this->base_world());
        ecs_assert(_::cpp_type<C>::size() != 0, ECS_INVALID_PARAMETER, NULL);

        return static_cast<C*>(
            ecs_get_mut_id(
                this->base_world(), this->base_id(), ecs_trait(comp_id, t.id()), is_added));
    }

    template<typename T, typename C>
    ECS_DEPRECATED("use has<Relation, Object>")
    bool has_trait() const {
        return ecs_has_id(this->base_world(), this->base_id(), ecs_trait(
            _::cpp_type<C>::id(this->base_world()), 
            _::cpp_type<T>::id(this->base_world())));
    }

    template<typename T>
    ECS_DEPRECATED("use has<Relation>(const flecs::entity&)")
    bool has_trait(const Base& component) const {
        return ecs_has_id(this->base_world(), this->base_id(), ecs_trait(
            component.id(), _::cpp_type<T>::id(this->base_world())));
    }

    template<typename C>
    ECS_DEPRECATED("use has_object<Object>(const flecs::entity&)")
    bool has_trait_tag(const Base& trait) const {
        return ecs_has_id(this->base_world(), this->base_id(), ecs_trait(
           _::cpp_type<C>::id(this->base_world()), trait.id()));
    }

    ECS_DEPRECATED("use has(const flecs::entity&, const flecs::entity&)")
    bool has_trait(const Base& trait, const Base& e) const {
        return ecs_has_id(this->base_world(), this->base_id(), ecs_trait(
            e.id(), trait.id()));
    }   
};

}
