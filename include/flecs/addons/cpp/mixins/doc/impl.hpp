#pragma once

namespace flecs {

namespace doc {

inline const char* get_brief(const flecs::entity_view& e) {
    return ecs_doc_get_brief(e.world(), e);
}

inline const char* get_detail(const flecs::entity_view& e) {
    return ecs_doc_get_detail(e.world(), e);
}

inline const char* get_link(const flecs::entity_view& e) {
    return ecs_doc_get_link(e.world(), e);
}

inline void set_brief(flecs::entity& e, const char *description) {
    ecs_doc_set_brief(e.world(), e, description);
}

inline void set_detail(flecs::entity& e, const char *description) {
    ecs_doc_set_detail(e.world(), e, description);
}

inline void set_link(flecs::entity& e, const char *description) {
    ecs_doc_set_link(e.world(), e, description);
}

}

}
