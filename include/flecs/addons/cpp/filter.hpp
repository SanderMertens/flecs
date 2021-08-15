namespace flecs 
{

////////////////////////////////////////////////////////////////////////////////
//// Ad hoc queries (filters)
////////////////////////////////////////////////////////////////////////////////

class filter_base {
public:
    filter_base()
        : m_world(nullptr)
        , m_filter({}) { }   
    
    filter_base(world_t *world, ecs_filter_t *filter = NULL)
        : m_world(world) {
            ecs_filter_move(&m_filter, filter);
        }

    /** Get pointer to C filter object.
     */
    const filter_t* c_ptr() const {
        if (m_filter.term_count) {
            return &m_filter;
        } else {
            return NULL;
        }
    }

    filter_base(const filter_base& obj) {
        this->m_world = obj.m_world;
        ecs_filter_copy(&m_filter, &obj.m_filter);
    }

    filter_base& operator=(const filter_base& obj) {
        this->m_world = obj.m_world;
        ecs_filter_copy(&m_filter, &obj.m_filter);
        return *this; 
    }

    filter_base(filter_base&& obj) {
        this->m_world = obj.m_world;
        ecs_filter_move(&m_filter, &obj.m_filter);
    }

    filter_base& operator=(filter_base&& obj) {
        this->m_world = obj.m_world;
        ecs_filter_move(&m_filter, &obj.m_filter);
        return *this; 
    }


    /** Free the filter.
     */
    ~filter_base() {
        ecs_filter_fini(&m_filter);
    }

    template <typename Func>
    void iter(Func&& func) const {
        ecs_iter_t it = ecs_filter_iter(m_world, &m_filter);
        while (ecs_filter_next(&it)) {
            _::iter_invoker<Func>(func).invoke(&it);
        }
    }  

    template <typename Func>
    void each_term(const Func& func) {
        for (int i = 0; i < m_filter.term_count; i ++) {
            flecs::term t(m_world, m_filter.terms[i]);
            func(t);
        }
    }

    flecs::term term(int32_t index) {
        return flecs::term(m_world, m_filter.terms[index]);
    }

    int32_t term_count() {
        return m_filter.term_count;
    }

    flecs::string str() {
        char *result = ecs_filter_str(m_world, &m_filter);
        return flecs::string(result);
    }

protected:
    world_t *m_world;
    filter_t m_filter;
};


template<typename ... Components>
class filter : public filter_base {
    using Terms = typename _::term_ptrs<Components...>::array;

public:
    filter() { }

    filter(world_t *world, filter_t *f)
        : filter_base(world, f) { }

    explicit filter(const world& world, const char *expr = nullptr) 
        : filter_base(world.c_ptr())
    {
        auto qb = world.filter_builder<Components ...>()
            .expr(expr);

        if (!expr) {
            qb.substitute_default();
        }

        flecs::filter_t f = qb;
        ecs_filter_move(&m_filter, &f);
    }

    filter(const filter& obj) : filter_base(obj) { }

    filter& operator=(const filter& obj) {
        *this = obj;
        return *this;
    }

    filter(filter&& obj) : filter_base(std::move(obj)) { }

    filter& operator=(filter&& obj) {
        filter_base(std::move(obj));
        return *this;
    }

    template <typename Func>
    void each(Func&& func) const {
        iterate<_::each_invoker>(std::forward<Func>(func), ecs_filter_next);
    } 

    template <typename Func>
    void iter(Func&& func) const { 
        iterate<_::iter_invoker>(std::forward<Func>(func), ecs_filter_next);
    }

private:
    template < template<typename Func, typename ... Comps> class Invoker, typename Func, typename NextFunc, typename ... Args>
    void iterate(Func&& func, NextFunc next, Args &&... args) const {
        ecs_iter_t it = ecs_filter_iter(m_world, &m_filter);
        while (next(&it, std::forward<Args>(args)...)) {
            Invoker<Func, Components...>(std::move(func)).invoke(&it);
        }
    }
};

}
