
const char* doc_name() {
    return ecs_doc_get_name(m_world, m_id);
}

const char* doc_brief() {
    return ecs_doc_get_brief(m_world, m_id);
}

const char* doc_detail() {
    return ecs_doc_get_detail(m_world, m_id);
}

const char* doc_link() {
    return ecs_doc_get_link(m_world, m_id);
}
