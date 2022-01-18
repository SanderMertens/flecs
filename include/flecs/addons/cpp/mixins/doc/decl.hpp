#pragma once

namespace flecs {
namespace doc {

using Description = EcsDocDescription;
static const flecs::entity_t Brief = EcsDocBrief;
static const flecs::entity_t Detail = EcsDocDetail;
static const flecs::entity_t Link = EcsDocLink;

namespace _ {
void init(flecs::world& world);
}

}
}
