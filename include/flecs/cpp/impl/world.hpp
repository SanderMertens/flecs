
namespace flecs 
{

inline void world::delete_entities(flecs::filter filter) const {
    ecs_bulk_delete(m_world, filter.c_ptr());
}

template <typename T>
inline void world::add(flecs::filter filter) const {
    ecs_bulk_add_remove_type(
        m_world, _::cpp_type<T>::type(m_world), nullptr, filter.c_ptr());
}

inline void world::add(flecs::type t) const {
    ecs_bulk_add_remove_type(m_world, t.c_ptr(), nullptr, nullptr);
}

inline void world::add(flecs::type t, flecs::filter filter) const {
    ecs_bulk_add_remove_type(m_world, t.c_ptr(), nullptr, filter.c_ptr());
}

inline void world::add(class flecs::entity e) const {
    ecs_bulk_add_remove_type(m_world, e.to_type().c_ptr(), nullptr, nullptr);
}

inline void world::add(class flecs::entity e, flecs::filter filter) const {
    ecs_bulk_add_remove_type(m_world, e.to_type().c_ptr(), nullptr, filter.c_ptr());
}

template <typename T>
inline void world::remove(flecs::filter filter) const {
    ecs_bulk_add_remove_type(
        m_world, nullptr, _::cpp_type<T>::type(m_world), filter.c_ptr());
}

inline void world::remove(flecs::type t) const {
    ecs_bulk_add_remove_type(m_world, nullptr, t.c_ptr(), nullptr);
}

inline void world::remove(flecs::type t, flecs::filter filter) const {
    ecs_bulk_add_remove_type(m_world, nullptr, t.c_ptr(), filter.c_ptr());
}

inline void world::remove(class entity e) const {
    ecs_bulk_add_remove_type(m_world, nullptr, e.to_type().c_ptr(), nullptr);
}

inline void world::remove(class entity e, flecs::filter filter) const {
    ecs_bulk_add_remove_type(m_world, nullptr, e.to_type().c_ptr(), filter.c_ptr());
}

inline flecs::world_filter world::filter(const flecs::filter& filter) const {
    return flecs::world_filter(*this, filter);
}

inline filter_iterator world::begin() const {
    return filter_iterator(*this, flecs::filter(*this), ecs_filter_next);
}

inline filter_iterator world::end() const {
    return filter_iterator(ecs_filter_next);
}

inline int world::count(flecs::filter filter) const {
    return ecs_count_w_filter(m_world, filter.c_ptr());
}

inline void world::init_builtin_components() {
    pod_component<Component>("flecs::core::Component");
    pod_component<Type>("flecs::core::Type");
    pod_component<Name>("flecs::core::Name");
    component<flecs::_::SystemCppContext>();
}

template <typename T>
inline flecs::entity world::use(const char *alias) {
    entity_t id = _::cpp_type<T>::id(m_world);
    const char *name = alias;
    if (!name) {
        // If no name is defined, use the entity name without the scope
        name = ecs_get_name(m_world, id);
    }
    ecs_use(m_world, id, name);
    return flecs::entity(m_world, id);
}

inline flecs::entity world::use(const char *name, const char *alias) {
    entity_t id = ecs_lookup_path_w_sep(m_world, 0, name, "::", "::");
    ecs_assert(id != 0, ECS_INVALID_PARAMETER, NULL);

    ecs_use(m_world, id, alias);
    return flecs::entity(m_world, id);
}

inline void world::use(flecs::entity e, const char *alias) {
    entity_t id = e.id();
    const char *name = alias;
    if (!name) {
        // If no name is defined, use the entity name without the scope
        ecs_get_name(m_world, id);
    }
    ecs_use(m_world, id, alias);
}

inline entity world::lookup(const char *name) const {
    auto id = ecs_lookup_path_w_sep(m_world, 0, name, "::", "::");
    return flecs::entity(*this, id);
}

template <typename T>
void world::set(T value) const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    e.set<T>(value);
}

template <typename T>
T* world::get_mut() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    return e.get_mut<T>();
}

template <typename T>
void world::modified() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    return e.modified<T>();
}

template <typename T, typename Func>
void world::patch(const Func& func) const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    e.patch<T>(func);
} 

template <typename T>
const T* world::get() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    return e.get<T>();
}

template <typename T>
bool world::has() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    return e.has<T>();
}

template <typename T>
void world::remove() const {
    flecs::entity e(m_world, _::cpp_type<T>::id(m_world));
    e.remove<T>();
}

template <typename T>
flecs::entity world::singleton() {
    return flecs::entity(m_world, _::cpp_type<T>::id(m_world));
}

template <typename... Args>
inline flecs::entity world::entity(Args &&... args) const {
    return flecs::entity(*this, std::forward<Args>(args)...);
}

template <typename... Args>
inline flecs::entity world::prefab(Args &&... args) const {
    return flecs::prefab(*this, std::forward<Args>(args)...);
}

template <typename... Args>
inline flecs::type world::type(Args &&... args) const {
    return flecs::type(*this, std::forward<Args>(args)...);
}

template <typename... Comps, typename... Args>
inline flecs::system<Comps...> world::system(Args &&... args) const {
    return flecs::system<Comps...>(*this, std::forward<Args>(args)...);
}

template <typename... Comps, typename... Args>
inline flecs::query<Comps...> world::query(Args &&... args) const {
    return flecs::query<Comps...>(*this, std::forward<Args>(args)...);
}

template <typename Module, typename... Args>
inline flecs::entity world::module(Args &&... args) const {
    return flecs::module<Module>(*this, std::forward<Args>(args)...);
}

template <typename Module>
inline flecs::entity world::import() {
    return flecs::import<Module>(*this);
}

template <typename T, typename... Args>
inline flecs::entity world::component(Args &&... args) const {
    return flecs::component<T>(*this, std::forward<Args>(args)...);
}

template <typename T, typename... Args>
inline flecs::entity world::pod_component(Args &&... args) const {
    return flecs::pod_component<T>(*this, std::forward<Args>(args)...);
}

template <typename T, typename... Args>
inline flecs::entity world::relocatable_component(Args &&... args) const {
    return flecs::relocatable_component<T>(*this, std::forward<Args>(args)...);
}

template <typename... Args>
inline flecs::snapshot world::snapshot(Args &&... args) const {
    return flecs::snapshot(*this, std::forward<Args>(args)...);
}

} // namespace flecs
