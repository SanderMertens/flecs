
namespace flecs
{

/* Deprecated functions */
template<typename Base>
class type_deprecated {
public:

    template <typename T, typename C>
    ECS_DEPRECATED("use add<Relation, Object>")
    type& add_trait() {
        static_cast<Base*>(this)->add(ecs_pair(
            _::cpp_type<T>::id(world()), 
            _::cpp_type<C>::id(world())));
        return *base();
    }

    template <typename T>
    ECS_DEPRECATED("use add<Relation>(const flecs::entity&)")
    type& add_trait(const flecs::entity& c) {
        static_cast<Base*>(this)->add(ecs_pair(_::cpp_type<T>::id(world()), c.id()));
        return *base();
    }

    ECS_DEPRECATED("use add(const flecs::entity&, const flecs::entity&)")
    type& add_trait(const flecs::entity& t, const flecs::entity& c) {
        static_cast<Base*>(this)->add(ecs_pair(t.id(), c.id()));
        return *base();
    }      

    template <typename C>
    ECS_DEPRECATED("use add_object<Object>(const flecs::entity&)")
    type& add_trait_tag(const flecs::entity& t) {
        static_cast<Base*>(this)->add(ecs_pair(t.id(), _::cpp_type<C>::id(world())));
        return *base();
    }

private:
    Base* base() { return static_cast<Base*>(this); }
    flecs::world_t* world() { return base()->world().c_ptr(); }
};

}
