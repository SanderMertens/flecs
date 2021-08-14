
namespace flecs
{

/* Deprecated functions */
template<typename Base>
class iter_deprecated {
public:
    ECS_DEPRECATED("use term_count(int32_t)")
    int32_t column_count() const {
        return base()->term_count();
    }

    ECS_DEPRECATED("use term_size(int32_t)")
    size_t column_size(int32_t col) const {
        return base()->term_size(col);
    }
    
    ECS_DEPRECATED("use is_owned(int32_t)")
    bool is_shared(int32_t col) const {
        return !base()->is_owned(col);
    }

    ECS_DEPRECATED("use term_source(int32_t)")
    flecs::entity column_source(int32_t col) const;

    ECS_DEPRECATED("use term_id(int32_t)")
    flecs::entity column_entity(int32_t col) const;

    ECS_DEPRECATED("no replacement")
    flecs::type column_type(int32_t col) const;

    ECS_DEPRECATED("use type()")
    type table_type() const; 

    template <typename T, if_t< is_const<T>::value > = 0>
    ECS_DEPRECATED("use term<const T>(int32_t)")
    flecs::column<T> column(int32_t col) const {
        return base()->template term<T>(col);
    }

    template <typename T, if_not_t< is_const<T>::value > = 0>
    ECS_DEPRECATED("use term<T>(int32_t)")
    flecs::column<T> column(int32_t col) const {
        ecs_assert(!ecs_term_is_readonly(iter(), col), 
            ECS_COLUMN_ACCESS_VIOLATION, NULL);
        return base()->template term<T>(col);
    }  

    ECS_DEPRECATED("use term(int32_t)")
    flecs::unsafe_column column(int32_t col) const {
        return base()->term(col);
    }

    template <typename T>
    ECS_DEPRECATED("use owned<T>(int32_t)")
    flecs::column<T> owned(int32_t col) const {
        return base()->template owned<T>(col);
    }

    template <typename T>
    ECS_DEPRECATED("use shared<T>(int32_t)")
    const T& shared(int32_t col) const {
        return base()->template shared<T>(col);
    }

    template <typename T, if_t< is_const<T>::value > = 0> 
    ECS_DEPRECATED("no replacement")
    T& element(int32_t col, int32_t row) const {
        return base()->template get_element<T>(col, row);
    }

    template <typename T, if_not_t< is_const<T>::value > = 0>
    ECS_DEPRECATED("no replacement")
    T& element(int32_t col, int32_t row) const {
        ecs_assert(!ecs_term_is_readonly(iter(), col), 
            ECS_COLUMN_ACCESS_VIOLATION, NULL);
        return base()->template get_element<T>(col, row);
    }

private:
    const Base* base() const { return static_cast<const Base*>(this); }
    const flecs::iter_t* iter() const { return base()->c_ptr(); }
};

}
