#pragma once

namespace flecs {

#define me_ this->me()

// Doc mixin implementation
inline void doc_m_world::init() {
    me_.template component<doc::Description>("flecs::doc::Description");
}

#undef me_

}
