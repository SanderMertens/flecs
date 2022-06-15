
Self& set_doc_name(const char *name) {
    ecs_doc_set_name(m_world, m_id, name);
    return to_base();
}

Self& set_doc_brief(const char *brief) {
    ecs_doc_set_brief(m_world, m_id, brief);
    return to_base();
}

Self& set_doc_detail(const char *detail) {
    ecs_doc_set_detail(m_world, m_id, detail);
    return to_base();
}

Self& set_doc_link(const char *link) {
    ecs_doc_set_link(m_world, m_id, link);
    return to_base();
}

Self& set_doc_color(const char *link) {
    ecs_doc_set_color(m_world, m_id, link);
    return to_base();
}
