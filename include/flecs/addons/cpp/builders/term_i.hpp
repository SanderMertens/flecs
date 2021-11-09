#pragma once

namespace flecs 
{

template<typename Base>

/** Term identifier builder.
 * A term identifier is either the predicate (pred), subject (subj) or object
 * (obj) of a term. Use the term builder to select the term identifier. */
class term_id_builder_i {
public:
    term_id_builder_i() : m_term_id(nullptr) { }

    virtual ~term_id_builder_i() { }

    /** Assign entity from type to currently selected term identifier. */
    template<typename T>
    Base& entity() {
        ecs_assert(m_term_id != NULL, ECS_INVALID_PARAMETER, NULL);
        m_term_id->entity = _::cpp_type<T>::id(this->world_v());
        return *this;
    }

    /** Assign entity to currently selected term identifier. */
    Base& entity(flecs::id_t id) {
        ecs_assert(m_term_id != NULL, ECS_INVALID_PARAMETER, NULL);
        m_term_id->entity = id;
        return *this;
    }

    /** Assign name to currently selected term identifier. 
     * The name will be resolved to either an entity or a variable name,
     * depending on the following rules (which are the same as the query DSL):
     * 
     * If the variable kind is flecs::VarIsDefault:
     * - If the name is a single uppercase value it is parsed as a variable
     * - If the name starts with an underscore it is parsed as a variable
     * - In any other case the name is parsed as an entity identifier
     * 
     * If the variable kind is flecs::VarIsEntity:
     * - The name is parsed as an entity identifier
     * 
     * If the variable kind is flecs::VarIsVariable:
     * - The name is interpreted as a variable
     */
    Base& name(const char *name) {
        ecs_assert(m_term_id != NULL, ECS_INVALID_PARAMETER, NULL);
        // Const cast is safe, when the value is actually used to construct a
        // query, it will be duplicated.
        m_term_id->name = const_cast<char*>(name);
        return *this;
    }

    /** Set whether the currently selected term id is a variable or entity. */
    Base& var(flecs::var_kind_t var = flecs::VarIsVariable) {
        m_term_id->var = static_cast<ecs_var_kind_t>(var);
        return *this;
    }

    /** Set the current term id to be a variable. */
    Base& var(const char *name) {
        ecs_assert(m_term_id != NULL, ECS_INVALID_PARAMETER, NULL);
        // Const cast is safe, when the value is actually used to construct a
        // query, it will be duplicated.
        m_term_id->name = const_cast<char*>(name);
        return var(); // Default to VarIsVariable
    }

    /** Assign set mask and relation. */
    Base& set(uint8_t mask, const flecs::id_t relation = flecs::IsA)
    {
        ecs_assert(m_term_id != NULL, ECS_INVALID_PARAMETER, NULL);
        m_term_id->set.mask = mask;
        m_term_id->set.relation = relation;
        return *this;
    }

    /** Shorthand for .set(flecs::SuperSet | mask, relation). */
    Base& super(const flecs::id_t relation = flecs::IsA, uint8_t mask = 0)
    {
        ecs_assert(!(mask & flecs::SubSet), ECS_INVALID_PARAMETER, NULL);
        return set(flecs::SuperSet | mask, relation);
    }

    /** Shorthand for .set(flecs::SubSet | mask, relation). */
    Base& sub(const flecs::id_t relation = flecs::IsA, uint8_t mask = 0)
    {
        ecs_assert(!(mask & flecs::SuperSet), ECS_INVALID_PARAMETER, NULL);
        return set(flecs::SubSet | mask, relation);
    }

    /** Set min_depth for set substitution. */
    Base& min_depth(int32_t min_depth) {
        m_term_id->set.min_depth = min_depth;
        return *this;
    }

    /** Set min_depth for set substitution. */
    Base& max_depth(int32_t max_depth) {
        m_term_id->set.max_depth = max_depth;
        return *this;
    }    

    ecs_term_id_t *m_term_id;
    
protected:
    virtual flecs::world_t* world_v() = 0;

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }
};

/** Term builder. A term is a single element of a query expression. */
template<typename Base>
class term_builder_i : public term_id_builder_i<Base> {
public:
    term_builder_i() : m_term(nullptr) { }

    term_builder_i(ecs_term_t *term_ptr) { 
        set_term(term_ptr);
    }

    /** Set (component) id to type id. */
    template<typename T>
    Base& id() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->pred.entity = _::cpp_type<T>::id(this->world_v());
        return *this;
    }

    /** Set (component) id to id. */
    Base& id(id_t id) {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->pred.entity = id;
        return *this;
    }

    /** Set (component) id to type.
     * Type must be associated with an entity (e.g. created by world::type) and
     * not an entity type (e.g. returned from entity::type). */
    Base& id(const flecs::type& type);

    Base& id(const flecs::type& type, id_t id);

    /** Set (component) id to pair derived from relation id / object id */
    Base& id(id_t r, id_t o) {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->pred.entity = r;
        m_term->obj.entity = o;
        return *this;
    }

    /** Set (component) id to pair derived from two types. */
    template<typename R, typename O>
    Base& id() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->pred.entity = _::cpp_type<R>::id(this->world_v());
        m_term->obj.entity = _::cpp_type<O>::id(this->world_v());
        return *this;
    }

    /** Set (component) id to pair derived from relation type / object id. */
    template<typename R>
    Base& id(id_t o) {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->pred.entity = _::cpp_type<R>::id(this->world_v());
        m_term->obj.entity = o;
        return *this;
    }    

    /** Set term from expression.
     * The syntax for expr is the same as that of the query DSL. The expression
     * must only contain a single term, for example:
     *   Position // correct
     *   Position, Velocity // incorrect
     */
    Base& expr(const char *expr) {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        const char *ptr;
        if ((ptr = ecs_parse_term(this->world_v(), nullptr, expr, expr, m_term)) == nullptr) {
            ecs_abort(ECS_INVALID_PARAMETER, NULL);
        }

        // Should not have more than one term
        ecs_assert(ptr[0] == 0, ECS_INVALID_PARAMETER, NULL);
        return *this;
    }

    /** Select predicate of term. 
     * Use methods from term_builder to configure properties of predicate. */
    Base& pred() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        this->m_term_id = &m_term->pred;
        return *this;
    }

    /** Select subject of term. 
     * Use methods from term_builder to configure properties of subject. */
    Base& subj() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        this->m_term_id = &m_term->subj;
        return *this;
    }

    /** Select object of term. 
     * Use methods from term_builder to configure properties of object. Setting
     * the object of a term will turn the term into a pair, and requires the
     * predicate to also be set. */
    Base& obj() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        this->m_term_id = &m_term->obj;
        return *this;
    }

    /** Select subject of term, initialize it with specified entity. */
    Base& subj(entity_t entity) {
        this->subj();
        this->m_term_id->entity = entity;
        return *this;
    }

    /** Select object of term, initialize it with specified entity. */
    Base& obj(entity_t entity) {
        this->obj();
        this->m_term_id->entity = entity;
        return *this;
    }
    
    /** Select subject of term, initialize it with id from specified type. */
    template<typename T>
    Base& subj() {
        this->subj();
        this->m_term_id->entity = _::cpp_type<T>::id(this->world_v());
        return *this;
    }

    /** Select object of term, initialize it with id from specified type. */
    template<typename T>
    Base& obj() {
        this->obj();
        this->m_term_id->entity = _::cpp_type<T>::id(this->world_v());
        return *this;
    }

    /** Set role of term. */
    Base& role(id_t role) {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->role = role;
        return *this;
    }

    /** Set read/write access of term. */
    Base& inout(flecs::inout_kind_t inout) {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->inout = static_cast<ecs_inout_kind_t>(inout);
        return *this;
    }

    /** Set operator of term. */
    Base& oper(flecs::oper_kind_t oper) {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->oper = static_cast<ecs_oper_kind_t>(oper);
        return *this;
    }

    /** Make term a singleton. */
    Base& singleton() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        ecs_assert(m_term->id || m_term->pred.entity, ECS_INVALID_PARAMETER, NULL);
        
        flecs::id_t pred = m_term->id;
        if (!pred) {
            pred = m_term->pred.entity;
        }

        ecs_assert(pred != 0, ECS_INVALID_PARAMETER, NULL);

        m_term->subj.entity = pred;

        return *this;
    }

    flecs::id id() {
        return flecs::id(this->world_v(), m_term->id);
    }

    ecs_term_t *m_term;

protected:
    virtual flecs::world_t* world_v() = 0;

    void set_term(ecs_term_t *term) {
        m_term = term;
        if (term) {
            this->m_term_id = &m_term->subj; // default to subject
        } else {
            this->m_term_id = nullptr;
        }
    }

    // A term can contain at most one component, but the parameter pack makes
    // the template parameter optional, which makes it easier to reuse the same
    // code for templated vs. non-templated terms.
    template <typename ... Components>
    void populate_term_from_pack() {
        flecs::array<flecs::id_t, sizeof...(Components)> ids ({
            (_::cpp_type<Components>::id(this->world_v()))...
        });

        flecs::array<flecs::inout_kind_t, sizeof...(Components)> inout_kinds ({
            (type_to_inout<Components>())...
        });

        flecs::array<flecs::oper_kind_t, sizeof...(Components)> oper_kinds ({
            (type_to_oper<Components>())...
        });

        size_t i = 0;
        for (auto the_id : ids) {
            this->id(the_id).inout(inout_kinds[i]).oper(oper_kinds[i]);
            i ++;
        }
    }

    template <typename T, if_t< is_const<T>::value > = 0>
    constexpr flecs::inout_kind_t type_to_inout() const {
        return flecs::In;
    }

    template <typename T, if_t< is_reference<T>::value > = 0>
    constexpr flecs::inout_kind_t type_to_inout() const {
        return flecs::Out;
    }

    template <typename T, if_not_t< 
        is_const<T>::value || is_reference<T>::value > = 0>
    constexpr flecs::inout_kind_t type_to_inout() const {
        return flecs::InOutDefault;
    }

    template <typename T, if_t< is_pointer<T>::value > = 0>
    constexpr flecs::oper_kind_t type_to_oper() const {
        return flecs::Optional;
    }

    template <typename T, if_not_t< is_pointer<T>::value > = 0>
    constexpr flecs::oper_kind_t type_to_oper() const {
        return flecs::And;
    } 

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }   
};

}