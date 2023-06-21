/**
 * @file addons/cpp/mixins/alerts/builder_i.hpp
 * @brief Alert builder interface.
 */

#pragma once

#include "../filter/builder_i.hpp"

namespace flecs {

/** Alert builder interface.
 * 
 * \ingroup cpp_addons_alerts
 */
template<typename Base, typename ... Components>
struct alert_builder_i : filter_builder_i<Base, Components ...> {
private:
    using BaseClass = filter_builder_i<Base, Components ...>;
    
public:
    alert_builder_i()
        : BaseClass(nullptr)
        , m_desc(nullptr) { }

    alert_builder_i(ecs_alert_desc_t *desc, int32_t term_index = 0) 
        : BaseClass(&desc->filter, term_index)
        , m_desc(desc) { }

    /** Alert message.
     *
     * @see ecs_alert_desc_t::message
     */      
    Base& message(const char *message) {
        m_desc->message = message;
        return *this;
    }

    /** Set brief description for alert.
     * 
     * @see ecs_alert_desc_t::brief
     */
    Base& brief(const char *brief) {
        m_desc->brief = brief;
        return *this;
    }

    /** Set doc name for alert.
     * 
     * @see ecs_alert_desc_t::doc_name
     */
    Base& doc_name(const char *doc_name) {
        m_desc->doc_name = doc_name;
        return *this;
    }

    /** Set severity of alert (default is Error) 
     * 
     * @see ecs_alert_desc_t::severity
     */
    Base& severity(flecs::entity_t kind) {
        m_desc->severity = kind;
        return *this;
    }

    /* Set retain period of alert. 
     * 
     * @see ecs_alert_desc_t::retain_period
     */
    Base& retain_period(ecs_ftime_t period) {
        m_desc->retain_period = period;
        return *this;
    }

    /** Set severity of alert (default is Error) 
     * 
     * @see ecs_alert_desc_t::severity
     */
    template <typename Severity>
    Base& severity() {
        return severity(_::cpp_type<Severity>::id(world_v()));
    }

    /** Add severity filter */
    Base& severity_filter(flecs::entity_t kind, flecs::id_t with, const char *var = nullptr) {
        ecs_assert(severity_filter_count < ECS_ALERT_MAX_SEVERITY_FILTERS, 
            ECS_INVALID_PARAMETER, "Maxium number of severity filters reached");

        ecs_alert_severity_filter_t *filter = 
            &m_desc->severity_filters[severity_filter_count ++];

        filter->severity = kind;
        filter->with = with;
        filter->var = var;
        return *this;
    }

    /** Add severity filter */
    template <typename Severity>
    Base& severity_filter(flecs::id_t with, const char *var = nullptr) {
        return severity_filter(_::cpp_type<Severity>::id(world_v()), with, var);
    }

    /** Add severity filter */
    template <typename Severity, typename T, if_not_t< is_enum<T>::value > = 0>
    Base& severity_filter(const char *var = nullptr) {
        return severity_filter(_::cpp_type<Severity>::id(world_v()), 
            _::cpp_type<T>::id(world_v()), var);
    }

    /** Add severity filter */
    template <typename Severity, typename T, if_t< is_enum<T>::value > = 0 >
    Base& severity_filter(T with, const char *var = nullptr) {
        flecs::world w(world_v());
        flecs::entity constant = w.to_entity<T>(with);
        return severity_filter(_::cpp_type<Severity>::id(world_v()), 
            w.pair<T>(constant), var);
    }

    /** Set member to create an alert for out of range values */
    Base& member(flecs::entity_t m) {
        m_desc->member = m;
        return *this;
    }

    /** Set (component) id for member (optional). If .member() is set and id
     * is not set, the id will default to the member parent. */
    Base& id(flecs::id_t id) {
        m_desc->id = id;
        return *this;
    }

    /** Set member to create an alert for out of range values */
    template <typename T>
    Base& member(const char *m, const char *v = nullptr) {
        flecs::entity_t id = _::cpp_type<T>::id(world_v());
        flecs::entity_t mid = ecs_lookup_path_w_sep(
            world_v(), id, m, "::", "::", false);
        ecs_assert(m != 0, ECS_INVALID_PARAMETER, NULL);
        m_desc->var = v;
        return this->member(mid);
    }

    /** Set source variable for member (optional, defaults to $this) */
    Base& var(const char *v) {
        m_desc->var = v;
        return *this;
    }

protected:
    virtual flecs::world_t* world_v() = 0;

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }

    ecs_alert_desc_t *m_desc;
    int32_t severity_filter_count = 0;
};

}
