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

protected:
    virtual flecs::world_t* world_v() = 0;

private:
    operator Base&() {
        return *static_cast<Base*>(this);
    }

    ecs_alert_desc_t *m_desc;
};

}
