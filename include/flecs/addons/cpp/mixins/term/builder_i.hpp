#pragma once

#include "../../utils/signature.hpp"

namespace flecs 
{

/** Term identifier builder.
 * A term identifier is either the predicate (pred), subject (subj) or object
 * (obj) of a term. Use the term builder to select the term identifier. */
template<typename Base>
struct term_id_builder_i {
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

    Base& owned(bool value = true) {
        if (value) {
            m_term_id->set.mask = flecs::Self;
        } else {
            m_term_id->set.mask = flecs::SuperSet;
            m_term_id->set.relation = flecs::IsA;
        }
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
struct term_builder_i : term_id_builder_i<Base> {
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

    /** Alias for id, for compatibility with populate_filter_from_pack */
    Base& term(id_t id) {
        return this->id(id);
    }

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

    /** Select predicate of term, initialize it with specified name. */
    Base& pred(const char *n) {
        this->pred();
        this->m_term_id->name = const_cast<char*>(n);
        return *this;
    }

    /** Select subject of term, initialize it with specified name. */
    Base& subj(const char *n) {
        this->subj();
        this->m_term_id->name = const_cast<char*>(n);
        return *this;
    }

    /** Select object of term, initialize it with specified name. */
    Base& obj(const char *n) {
        this->obj();
        this->m_term_id->name = const_cast<char*>(n);
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

    /** Scheduler annotation to indicate system uses add<T> */
    Base& add() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->inout = static_cast<ecs_inout_kind_t>(flecs::Out);
        if (m_term->oper != EcsNot) {
            m_term->subj.set.mask = flecs::Nothing;
        }
        return *this;
    }

    /** Scheduler annotation to indicate sytem uses add<T>, remove<T> or set<T> */
    Base& write() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->inout = static_cast<ecs_inout_kind_t>(flecs::Out);
        if (m_term->oper != EcsNot) {
            m_term->subj.set.mask = flecs::Nothing;
        }
        return *this;
    }

    /** Scheduler annotation to indicate sytem uses get<T> */
    Base& read() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->inout = static_cast<ecs_inout_kind_t>(flecs::In);
        if (m_term->oper != EcsNot) {
            m_term->subj.set.mask = flecs::Nothing;
        }
        return *this;
    }

    /** Scheduler annotation to indicate sytem uses get_mut<T> */
    Base& read_write() {
        ecs_assert(m_term != nullptr, ECS_INVALID_PARAMETER, NULL);
        m_term->inout = static_cast<ecs_inout_kind_t>(flecs::InOut);
        if (m_term->oper != EcsNot) {
            m_term->subj.set.mask = flecs::Nothing;
        }
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

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }   
};

}
