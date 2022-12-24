/**
 * @file addons/cpp/mixins/filter/builder_i.hpp
 * @brief Filter builder interface.
 */

#pragma once

#include "../term/builder_i.hpp"

namespace flecs 
{

/** Filter builder interface.
 * 
 * \ingroup cpp_filters
 */
template<typename Base, typename ... Components>
struct filter_builder_i : term_builder_i<Base> {
    filter_builder_i(ecs_filter_desc_t *desc, int32_t term_index = 0) 
        : m_term_index(term_index)
        , m_expr_count(0)
        , m_desc(desc) { }

    Base& instanced() {
        m_desc->instanced = true;
        return *this;
    }

    Base& expr(const char *expr) {
        ecs_check(m_expr_count == 0, ECS_INVALID_OPERATION,
            "filter_builder::expr() called more than once");
        m_desc->expr = expr;
        m_expr_count ++;

    error:
        return *this;
    }

    /* With/without shorthand notation. */

    template <typename ... Args>
    Base& with(Args&&... args) {
        return this->term(FLECS_FWD(args)...);
    }

    template <typename T, typename ... Args>
    Base& with(Args&&... args) {
        return this->term<T>(FLECS_FWD(args)...);
    }

    template <typename First, typename Second>
    Base& with() {
        return this->term<First, Second>();
    }

    template <typename ... Args>
    Base& without(Args&&... args) {
        return this->term(FLECS_FWD(args)...).not_();
    }

    template <typename T, typename ... Args>
    Base& without(Args&&... args) {
        return this->term<T>(FLECS_FWD(args)...).not_();
    }

    template <typename First, typename Second>
    Base& without() {
        return this->term<First, Second>().not_();
    }

    /* Write/read shorthand notation */

    Base& write() {
        term_builder_i<Base>::write();
        return *this;
    }

    template <typename ... Args>
    Base& write(Args&&... args) {
        return this->term(FLECS_FWD(args)...).write();
    }

    template <typename T, typename ... Args>
    Base& write(Args&&... args) {
        return this->term<T>(FLECS_FWD(args)...).write();
    }

    template <typename First, typename Second>
    Base& write() {
        return this->term<First, Second>().write();
    }

    Base& read() {
        term_builder_i<Base>::read();
        return *this;
    }

    template <typename ... Args>
    Base& read(Args&&... args) {
        return this->term(FLECS_FWD(args)...).read();
    }

    template <typename T, typename ... Args>
    Base& read(Args&&... args) {
        return this->term<T>(FLECS_FWD(args)...).read();
    }

    template <typename First, typename Second>
    Base& read() {
        return this->term<First, Second>().read();
    }

    /* Term notation for more complex query features */

    Base& term() {
        if (this->m_term) {
            ecs_check(ecs_term_is_initialized(this->m_term), 
                ECS_INVALID_OPERATION, 
                    "filter_builder::term() called without initializing term");
        }

        if (m_term_index >= ECS_TERM_DESC_CACHE_SIZE) {
            if (m_term_index == ECS_TERM_DESC_CACHE_SIZE) {
                m_desc->terms_buffer = ecs_os_calloc_n(
                    ecs_term_t, m_term_index + 1);
                ecs_os_memcpy_n(m_desc->terms_buffer, m_desc->terms, 
                    ecs_term_t, m_term_index);
                ecs_os_memset_n(m_desc->terms, 0, 
                    ecs_term_t, ECS_TERM_DESC_CACHE_SIZE);
            } else {
                m_desc->terms_buffer = ecs_os_realloc_n(m_desc->terms_buffer, 
                    ecs_term_t, m_term_index + 1);
            }

            m_desc->terms_buffer_count = m_term_index + 1;

            this->set_term(&m_desc->terms_buffer[m_term_index]);
        } else {
            this->set_term(&m_desc->terms[m_term_index]);
        }

        m_term_index ++;
    
    error:
        return *this;
    }

    Base& term_at(int32_t term_index) {
        ecs_assert(term_index > 0, ECS_INVALID_PARAMETER, NULL);
        int32_t prev_index = m_term_index;
        m_term_index = term_index - 1;
        this->term();
        m_term_index = prev_index;
        ecs_assert(ecs_term_is_initialized(this->m_term), 
            ECS_INVALID_PARAMETER, NULL);
        return *this;
    }

    Base& arg(int32_t term_index) {
        return this->term_at(term_index);
    }

    template<typename T>
    Base& term() {
        this->term();
        *this->m_term = flecs::term(_::cpp_type<T>::id(this->world_v())).move();
        this->m_term->inout = static_cast<ecs_inout_kind_t>(
            _::type_to_inout<T>());
        return *this;
    }

    Base& term(id_t id) {
        this->term();
        *this->m_term = flecs::term(id).move();
        return *this;
    }

    Base& term(const char *name) {
        this->term();
        *this->m_term = flecs::term().first(name).move();
        return *this;
    }

    Base& term(const char *first, const char *second) {
        this->term();
        *this->m_term = flecs::term().first(first).second(second).move();
        return *this;
    }

    Base& term(entity_t r, entity_t o) {
        this->term();
        *this->m_term = flecs::term(r, o).move();
        return *this;
    }

    Base& term(entity_t r, const char *o) {
        this->term();
        *this->m_term = flecs::term(r).second(o).move();
        return *this;
    }

    template<typename First>
    Base& term(id_t o) {
        return this->term(_::cpp_type<First>::id(this->world_v()), o);
    }

    template<typename First>
    Base& term(const char *second) {
        return this->term(_::cpp_type<First>::id(this->world_v())).second(second);
    }

    template<typename First, typename Second>
    Base& term() {
        return this->term<First>(_::cpp_type<Second>::id(this->world_v()));
    }

    template <typename E, if_t< is_enum<E>::value > = 0>
    Base& term(E value) {
        flecs::entity_t r = _::cpp_type<E>::id(this->world_v());
        auto o = enum_type<E>(this->world_v()).entity(value);
        return this->term(r, o);
    }

    Base& term(flecs::term& term) {
        this->term();
        *this->m_term = term.move();
        return *this;
    }

    Base& term(flecs::term&& term) {
        this->term();
        *this->m_term = term.move();
        return *this;
    }

protected:
    virtual flecs::world_t* world_v() = 0;
    int32_t m_term_index;
    int32_t m_expr_count;

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }

    ecs_filter_desc_t *m_desc;
};

}
