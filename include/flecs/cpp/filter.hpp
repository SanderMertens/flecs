
namespace flecs 
{

////////////////////////////////////////////////////////////////////////////////
//// A filter is used to match subsets of tables
////////////////////////////////////////////////////////////////////////////////
 
enum match_kind {
    MatchAll = EcsMatchAll,
    MatchAny = EcsMatchAny,
    MatchExact = EcsMatchExact
};

class filter {
public:
    filter() 
        : m_world( nullptr )
        , m_filter{ } {}

    explicit filter(const world& world) 
        : m_world( world.c_ptr() )
        , m_filter{ } { }

    filter& include(type type) {
        m_filter.include = ecs_type_merge(m_world, m_filter.include, type.c_ptr(), nullptr);
        return *this;
    }

    filter& include(entity entity) {
        m_filter.include = ecs_type_add(m_world, m_filter.include, entity.id());
        return *this;
    }

    template <typename T>
    filter& include() {
        m_filter.include = ecs_type_add(m_world, m_filter.include, _::cpp_type<T>::id(m_world));
        return *this;
    }

    filter& include_kind(match_kind kind) {
        m_filter.include_kind = static_cast<ecs_match_kind_t>(kind);
        return *this;
    }

    type include() {
        return type(m_world, m_filter.include);
    }

    filter& exclude(type type) {
        m_filter.exclude = ecs_type_merge(m_world, m_filter.exclude, type.c_ptr(), nullptr);
        return *this;
    }

    filter& exclude(entity entity) {
        m_filter.exclude = ecs_type_add(m_world, m_filter.exclude, entity.id());
        return *this;
    }

    template <typename T>
    filter& exclude() {
        m_filter.exclude = ecs_type_add(m_world, m_filter.exclude, _::cpp_type<T>::id(m_world));
        return *this;
    }
 
    filter& exclude_kind(match_kind kind) {
        m_filter.exclude_kind = static_cast<ecs_match_kind_t>(kind);
        return *this;
    }

    type exclude() {
        return type(m_world, m_filter.exclude);
    }  

    const filter_t* c_ptr() const {
        if (m_world) {
            return &m_filter;
        } else {
            return nullptr;
        }
    }

private:
    world_t *m_world;
    filter_t m_filter;
};

} // namespace flecs
