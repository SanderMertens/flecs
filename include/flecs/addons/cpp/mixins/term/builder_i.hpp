/**
 * @file addons/cpp/mixins/term/builder_i.hpp
 * @brief Term builder interface.
 */

#pragma once

#include "../../utils/signature.hpp"

namespace flecs 
{

/** Term identifier builder.
 * A term identifier describes a single identifier in a term. Identifier
 * descriptions can reference entities by id, name or by variable, which means
 * the entity will be resolved when the term is evaluated.
 * 
 * \ingroup cpp_core_filters
 */
template<typename Base>
struct term_id_builder_i {
    term_id_builder_i() : m_term_id(nullptr) { }

    virtual ~term_id_builder_i() { }

    /* The self flag indicates the term identifier itself is used */
    Base& self() {
        this->assert_term_id();
        m_term_id->flags |= flecs::Self;
        return *this;
    }

    /* The up flag indicates that the term identifier may be substituted by
     * traversing a relationship upwards. For example: substitute the identifier
     * with its parent by traversing the ChildOf relationship. */
    Base& up(flecs::entity_t trav = 0) {
        this->assert_term_id();
        m_term_id->flags |= flecs::Up;
        if (trav) {
            m_term_id->trav = trav;
        }
        return *this;
    }

    template <typename Trav>
    Base& up() {
        return this->up(_::cpp_type<Trav>::id(this->world_v()));
    }

    /* The cascade flag is like up, but returns results in breadth-first order.
     * Only supported for flecs::query */
    Base& cascade(flecs::entity_t trav = 0) {
        this->assert_term_id();
        m_term_id->flags |= flecs::Cascade;
        if (trav) {
            m_term_id->trav = trav;
        }
        return *this;
    }

    template <typename Trav>
    Base& cascade() {
        return this->cascade(_::cpp_type<Trav>::id(this->world_v()));
    }

    /* The parent flag is short for up(flecs::ChildOf) */
    Base& parent() {
        this->assert_term_id();
        m_term_id->flags |= flecs::Parent;
        return *this;
    }

    /* Specify relationship to traverse, and flags to indicate direction */
    Base& trav(flecs::entity_t trav, flecs::flags32_t flags = 0) {
        this->assert_term_id();
        m_term_id->trav = trav;
        m_term_id->flags |= flags;
        return *this;
    }

    /* Specify value of identifier by id */
    Base& id(flecs::entity_t id) {
        this->assert_term_id();
        m_term_id->id = id;
        return *this;
    }

    /* Specify value of identifier by id. Amost the same as id(entity), but this
     * operation explicitly sets the flecs::IsEntity flag. This forces the id to 
     * be interpreted as entity, whereas not setting the flag would implicitly
     * convert ids for builtin variables such as flecs::This to a variable.
     * 
     * This function can also be used to disambiguate id(0), which would match
     * both id(entity_t) and id(const char*).
     */
    Base& entity(flecs::entity_t entity) {
        this->assert_term_id();
        m_term_id->flags = flecs::IsEntity;
        m_term_id->id = entity;
        return *this;
    }

    /* Specify value of identifier by name */
    Base& name(const char *name) {
        this->assert_term_id();
        m_term_id->flags |= flecs::IsEntity;
        m_term_id->name = const_cast<char*>(name);
        return *this;
    }

    /* Specify identifier is a variable (resolved at query evaluation time) */
    Base& var(const char *var_name) {
        this->assert_term_id();
        m_term_id->flags |= flecs::IsVariable;
        m_term_id->name = const_cast<char*>(var_name);
        return *this;
    }

    ecs_term_id_t *m_term_id;
    
protected:
    virtual flecs::world_t* world_v() = 0;

private:
    void assert_term_id() {
        ecs_assert(m_term_id != NULL, ECS_INVALID_PARAMETER, 
            "no active term (call .term() first)");
    }

    operator Base&() {
        return *static_cast<Base*>(this);
    }
};

/** Term builder interface. 
 * A term is a single element of a query expression. 
 * 
 * \ingroup cpp_addons_filter
 */
template<typename Base>
struct term_builder_i : term_id_builder_i<Base> {
    term_builder_i() : m_term(nullptr) { }

    term_builder_i(ecs_term_t *term_ptr) { 
        set_term(term_ptr);
    }

    Base& term(id_t id) {
        return this->id(id);
    }

    /* Call prior to setting values for src identifier */
    Base& src() {
        this->assert_term();
        this->m_term_id = &m_term->src;
        return *this;
    }

    /* Call prior to setting values for first identifier. This is either the
     * component identifier, or first element of a pair (in case second is
     * populated as well). */
    Base& first() {
        this->assert_term();
        this->m_term_id = &m_term->first;
        return *this;
    }

    /* Call prior to setting values for second identifier. This is the second 
     * element of a pair. Requires that first() is populated as well. */
    Base& second() {
        this->assert_term();
        this->m_term_id = &m_term->second;
        return *this;
    }

    /* Select src identifier, initialize it with entity id */
    Base& src(flecs::entity_t id) {
        this->src();
        this->id(id);
        return *this;
    }

    /* Select src identifier, initialize it with id associated with type */
    template<typename T>
    Base& src() {
        this->src(_::cpp_type<T>::id(this->world_v()));
        return *this;
    }

    /* Select src identifier, initialize it with name. If name starts with a $
     * the name is interpreted as a variable. */
    Base& src(const char *name) {
        ecs_assert(name != NULL, ECS_INVALID_PARAMETER, NULL);
        this->src();
        if (name[0] == '$') {
            this->var(&name[1]);
        } else {
            this->name(name);
        }
        return *this;
    }

    /* Select first identifier, initialize it with entity id */
    Base& first(flecs::entity_t id) {
        this->first();
        this->id(id);
        return *this;
    }

    /* Select first identifier, initialize it with id associated with type */
    template<typename T>
    Base& first() {
        this->first(_::cpp_type<T>::id(this->world_v()));
        return *this;
    }

    /* Select first identifier, initialize it with name. If name starts with a $
     * the name is interpreted as a variable. */
    Base& first(const char *name) {
        ecs_assert(name != NULL, ECS_INVALID_PARAMETER, NULL);
        this->first();
        if (name[0] == '$') {
            this->var(&name[1]);
        } else {
            this->name(name);
        }
        return *this;
    }

    /* Select second identifier, initialize it with entity id */
    Base& second(flecs::entity_t id) {
        this->second();
        this->id(id);
        return *this;
    }

    /* Select second identifier, initialize it with id associated with type */
    template<typename T>
    Base& second() {
        this->second(_::cpp_type<T>::id(this->world_v()));
        return *this;
    }

    /* Select second identifier, initialize it with name. If name starts with a $
     * the name is interpreted as a variable. */
    Base& second(const char *name) {
        ecs_assert(name != NULL, ECS_INVALID_PARAMETER, NULL);
        this->second();
        if (name[0] == '$') {
            this->var(&name[1]);
        } else {
            this->name(name);
        }
        return *this;
    }

    /** Set role of term. */
    Base& role(id_t role) {
        this->assert_term();
        m_term->id_flags = role;
        return *this;
    }

    /** Set read/write access of term. */
    Base& inout(flecs::inout_kind_t inout) {
        this->assert_term();
        m_term->inout = static_cast<ecs_inout_kind_t>(inout);
        return *this;
    }

    /** Set read/write access for stage. Use this when a system reads or writes
     * components other than the ones provided by the query. This information 
     * can be used by schedulers to insert sync/merge points between systems
     * where deferred operations are flushed.
     * 
     * Setting this is optional. If not set, the value of the accessed component
     * may be out of sync for at most one frame.
     */
    Base& inout_stage(flecs::inout_kind_t inout) {
        this->assert_term();
        m_term->inout = static_cast<ecs_inout_kind_t>(inout);
        if (m_term->oper != EcsNot) {
            this->src().entity(0);
        }
        return *this;
    }

    /** Short for inout_stage(flecs::Out). 
     *   Use when system uses add, remove or set. 
     */
    Base& write() {
        return this->inout_stage(flecs::Out);
    }

    /** Short for inout_stage(flecs::In).
     *   Use when system uses get.
     */
    Base& read() {
        return this->inout_stage(flecs::In);
    }

    /** Short for inout_stage(flecs::InOut).
     *   Use when system uses get_mut.
     */
    Base& read_write() {
        return this->inout_stage(flecs::InOut);
    }

    /** Short for inout(flecs::In) */
    Base& in() {
        return this->inout(flecs::In);
    }

    /** Short for inout(flecs::Out) */
    Base& out() {
        return this->inout(flecs::Out);
    }

    /** Short for inout(flecs::InOut) */
    Base& inout() {
        return this->inout(flecs::InOut);
    }

    /** Short for inout(flecs::In) */
    Base& inout_none() {
        return this->inout(flecs::InOutNone);
    }

    /** Set operator of term. */
    Base& oper(flecs::oper_kind_t oper) {
        this->assert_term();
        m_term->oper = static_cast<ecs_oper_kind_t>(oper);
        return *this;
    }

    /* Short for oper(flecs::And) */
    Base& and_() {
        return this->oper(flecs::And);
    }

    /* Short for oper(flecs::Or) */
    Base& or_() {
        return this->oper(flecs::Or);
    }

    /* Short for oper(flecs::Or) */
    Base& not_() {
        return this->oper(flecs::Not);
    }

    /* Short for oper(flecs::Or) */
    Base& optional() {
        return this->oper(flecs::Optional);
    }

    /* Short for oper(flecs::AndFrom) */
    Base& and_from() {
        return this->oper(flecs::AndFrom);
    }

    /* Short for oper(flecs::OrFrom) */
    Base& or_from() {
        return this->oper(flecs::OrFrom);
    }

    /* Short for oper(flecs::NotFrom) */
    Base& not_from() {
        return this->oper(flecs::NotFrom);
    }

    /** Match singleton. */
    Base& singleton() {
        this->assert_term();
        ecs_assert(m_term->id || m_term->first.id, ECS_INVALID_PARAMETER, 
                "no component specified for singleton");
        
        flecs::id_t sid = m_term->id;
        if (!sid) {
            sid = m_term->first.id;
        }

        ecs_assert(sid != 0, ECS_INVALID_PARAMETER, NULL);
        m_term->src.id = sid;
        return *this;
    }

    /* Filter terms are not triggered on by observers */
    Base& filter() {
        m_term->src.flags |= flecs::Filter;
        return *this;
    }

    ecs_term_t *m_term;

protected:
    virtual flecs::world_t* world_v() = 0;

    void set_term(ecs_term_t *term) {
        m_term = term;
        if (term) {
            this->m_term_id = &m_term->src; // default to subject
        } else {
            this->m_term_id = nullptr;
        }
    }

private:
    void assert_term() {
        ecs_assert(m_term != NULL, ECS_INVALID_PARAMETER, 
            "no active term (call .term() first)");
    }

    operator Base&() {
        return *static_cast<Base*>(this);
    }   
};

}
