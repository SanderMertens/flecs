/**
 * @file addons/cpp/mixins/term/builder_i.hpp
 * @brief Term builder interface.
 */

#pragma once

#include "../../utils/signature.hpp"
#include <stdio.h>

namespace flecs 
{

/** Term identifier builder.
 * A term identifier describes a single identifier in a term. Identifier
 * descriptions can reference entities by ID, name, or by variable, which means
 * the entity will be resolved when the term is evaluated.
 * 
 * @ingroup cpp_core_queries
 */
template<typename Base>
struct term_ref_builder_i {
    /** Default constructor. */
    term_ref_builder_i() : term_ref_(nullptr) { }

    /** Destructor. */
    virtual ~term_ref_builder_i() { }

    /** The self flag indicates that the term identifier itself is used. */
    Base& self() {
        this->assert_term_ref();
        term_ref_->id |= flecs::Self;
        return *this;
    }

    /** Specify the value of the identifier by ID. */
    Base& id(flecs::entity_t id) {
        this->assert_term_ref();
        term_ref_->id = id;
        return *this;
    }

    /** Specify the value of the identifier by ID. Almost the same as id(entity_t), but this
     * operation explicitly sets the flecs::IsEntity flag. This forces the ID to
     * be interpreted as an entity, whereas not setting the flag would implicitly
     * convert IDs for built-in variables such as flecs::This to a variable.
     *
     * This function can also be used to disambiguate id(0), which would match
     * both id(entity_t) and id(const char*).
     */
    Base& entity(flecs::entity_t entity) {
        this->assert_term_ref();
        term_ref_->id = entity | flecs::IsEntity;
        return *this;
    }

    /** Specify the value of the identifier by name. */
    Base& name(const char *name) {
        this->assert_term_ref();
        term_ref_->id |= flecs::IsEntity;
        term_ref_->name = const_cast<char*>(name);
        return *this;
    }

    /** Specify that the identifier is a variable (resolved at query evaluation time). */
    Base& var(const char *var_name) {
        this->assert_term_ref();
        term_ref_->id |= flecs::IsVariable;
        term_ref_->name = const_cast<char*>(var_name);
        return *this;
    }

    /** Override the term ID flags. */
    Base& flags(flecs::flags64_t flags) {
        this->assert_term_ref();
        term_ref_->id = flags;
        return *this;
    }

    /** Pointer to the current term reference. */
    ecs_term_ref_t *term_ref_;

protected:
    virtual flecs::world_t* world_v() = 0;

    void assert_term_ref() {
        ecs_assert(term_ref_ != nullptr, ECS_INVALID_PARAMETER, 
            "no active term (call .with() first)");
    }

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }
};

/** Term builder interface. 
 * A term is a single element of a query expression. 
 * 
 * @ingroup cpp_core_queries
 */
template<typename Base>
struct term_builder_i : term_ref_builder_i<Base> {
    /** Default constructor. */
    term_builder_i() : term_(nullptr) { }

    /** Construct from a term pointer. */
    term_builder_i(ecs_term_t *term_ptr) {
        set_term(term_ptr);
    }

    /** Set the term ID. */
    Base& term(id_t id) {
        return this->id(id);
    }

    template <typename T = void, typename... Args>
    Base& src(Args... args) {
        return select_ref<T>(&ecs_term_t::src, args...);
    }

    template <typename T = void, typename... Args>
    Base& first(Args... args) {
        return select_ref<T>(&ecs_term_t::first, args...);
    }

    template <typename T = void, typename... Args>
    Base& second(Args... args) {
        return select_ref<T>(&ecs_term_t::second, args...);
    }

    /** The up flag indicates that the term identifier may be substituted by
     * traversing a relationship upwards. For example, substitute the identifier
     * with its parent by traversing the ChildOf relationship. */
    Base& up(flecs::entity_t trav = 0) {
        this->assert_term_ref();
        ecs_check(this->term_ref_ != &term_->first, ECS_INVALID_PARAMETER,
            "up traversal can only be applied to term source");
        ecs_check(this->term_ref_ != &term_->second, ECS_INVALID_PARAMETER,
            "up traversal can only be applied to term source");
        this->term_ref_->id |= flecs::Up;
        if (trav) {
            term_->trav = trav;
        }
    error:
        return *this;
    }

    /** Traverse upwards using the specified relationship type. */
    template <typename Trav>
    Base& up() {
        return this->up(_::type<Trav>::id(this->world_v()));
    }

    /** The cascade flag is like up(), but returns results in breadth-first order.
     * Only supported for flecs::query. */
    Base& cascade(flecs::entity_t trav = 0) {
        this->assert_term_ref();
        this->up();
        this->term_ref_->id |= flecs::Cascade;
        if (trav) {
            term_->trav = trav;
        }
        return *this;
    }

    /** Cascade using the specified relationship type. */
    template <typename Trav>
    Base& cascade() {
        return this->cascade(_::type<Trav>::id(this->world_v()));
    }

    /** Use with cascade() to iterate results in descending (bottom-to-top) order. */
    Base& desc() {
        this->assert_term_ref();
        this->term_ref_->id |= flecs::Desc;
        return *this;
    }

    /** Same as up(). Exists for backwards compatibility. */
    Base& parent() {
        return this->up();
    }

    /** Specify the relationship to traverse, and flags to indicate direction. */
    Base& trav(flecs::entity_t trav, flecs::flags32_t flags = 0) {
        this->assert_term_ref();
        term_->trav = trav;
        this->term_ref_->id |= flags;
        return *this;
    }

    /** Set ID flags for the term. */
    Base& id_flags(id_t flags) {
        this->assert_term();
        term_->id |= flags;
        return *this;
    }

    /** Set read/write access of the term. */
    Base& inout(flecs::inout_kind_t inout) {
        this->assert_term();
        term_->inout = static_cast<int16_t>(inout);
        return *this;
    }

    /** Set read/write access for a stage. Use this when a system reads or writes
     * components other than the ones provided by the query. This information 
     * can be used by schedulers to insert sync/merge points between systems
     * where deferred operations are flushed.
     * 
     * Setting this is optional. If not set, the value of the accessed component
     * may be out of sync for at most one frame.
     */
    Base& inout_stage(flecs::inout_kind_t inout) {
        this->assert_term();
        term_->inout = static_cast<int16_t>(inout);
        if (term_->oper != EcsNot) {
            this->src().entity(0);
        }
        return *this;
    }

    /** Short for inout_stage(flecs::Out).
     * Use when the system uses add(), remove(), or set().
     */
    Base& write() {
        return this->inout_stage(flecs::Out);
    }

    /** Short for inout_stage(flecs::In).
     * Use when the system uses get().
     */
    Base& read() {
        return this->inout_stage(flecs::In);
    }

    /** Short for inout_stage(flecs::InOut).
     * Use when the system uses ensure().
     */
    Base& read_write() {
        return this->inout_stage(flecs::InOut);
    }

    /** Short for inout(flecs::In). */
    Base& in() {
        return this->inout(flecs::In);
    }

    /** Short for inout(flecs::Out). */
    Base& out() {
        return this->inout(flecs::Out);
    }

    /** Short for inout(flecs::InOut). */
    Base& inout() {
        return this->inout(flecs::InOut);
    }

    /** Short for inout(flecs::InOutNone). */
    Base& inout_none() {
        return this->inout(flecs::InOutNone);
    }

    /** Set the operator of the term. */
    Base& oper(flecs::oper_kind_t oper) {
        this->assert_term();
        term_->oper = static_cast<int16_t>(oper);
        return *this;
    }

    /** Short for oper(flecs::And). */
    Base& and_() {
        return this->oper(flecs::And);
    }

    /** Short for oper(flecs::Or). */
    Base& or_() {
        return this->oper(flecs::Or);
    }

    /** Short for oper(flecs::Not). */
    Base& not_() {
        return this->oper(flecs::Not);
    }

    /** Short for oper(flecs::Optional). */
    Base& optional() {
        return this->oper(flecs::Optional);
    }

    /** Short for oper(flecs::AndFrom). */
    Base& and_from() {
        return this->oper(flecs::AndFrom);
    }

    /** Short for oper(flecs::OrFrom). */
    Base& or_from() {
        return this->oper(flecs::OrFrom);
    }

    /** Short for oper(flecs::NotFrom). */
    Base& not_from() {
        return this->oper(flecs::NotFrom);
    }

    /** Mark the term as a filter. Query terms marked as a filter are not triggered
     * by observers. */
    Base& filter() {
        term_->inout = EcsInOutFilter;
        return *this;
    }

    /** Pointer to the current term. */
    ecs_term_t *term_;

protected:
    virtual flecs::world_t* world_v() override = 0;

    /** Set the current term pointer. */
    void set_term(ecs_term_t *term) {
        term_ = term;
        if (term) {
            this->term_ref_ = &term_->src; // default to source
        } else {
            this->term_ref_ = nullptr;
        }
    }

private:
    Base& set_ref() {
        return *this;
    }

    Base& set_ref(flecs::entity_t id) {
        return this->id(id);
    }

    Base& set_ref(const char *name) {
        ecs_assert(name != nullptr, ECS_INVALID_PARAMETER, nullptr);
        return name[0] == '$' ? this->var(name + 1) : this->name(name);
    }

    template <typename T, typename... Args>
    Base& select_ref(ecs_term_ref_t ecs_term_t::*ref, Args... args) {
        assert_term();
        this->term_ref_ = &(term_->*ref);
        if constexpr (std::is_void_v<T>) {
            return set_ref(args...);
        } else {
            return set_ref(_::type<T>::id(this->world_v()));
        }
    }

    void assert_term() {
        ecs_assert(term_ != nullptr, ECS_INVALID_PARAMETER, 
            "no active term (call .with() first)");
    }

    operator Base&() {
        return *static_cast<Base*>(this);
    }   
};

}
