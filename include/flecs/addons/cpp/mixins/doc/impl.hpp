#pragma once

namespace flecs {

#define flecs_me_ this->me()

// Doc mixin implementation
inline void doc_m_world::init() {
    flecs_me_.template component<doc::Description>("flecs::doc::Description");
}

#undef flecs_me_

}
