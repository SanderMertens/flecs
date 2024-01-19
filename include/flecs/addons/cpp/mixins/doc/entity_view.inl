/**
 * @file addons/cpp/mixins/doc/entity_view.inl
 * @brief Doc entity view mixin.
 */

const char* doc_name() const {
    return ecs_doc_get_name(m_world, m_id);
}

const char* doc_brief() const {
    return ecs_doc_get_brief(m_world, m_id);
}

const char* doc_detail() const {
    return ecs_doc_get_detail(m_world, m_id);
}

const char* doc_link() const {
    return ecs_doc_get_link(m_world, m_id);
}

const char* doc_color() const {
    return ecs_doc_get_color(m_world, m_id);
}
