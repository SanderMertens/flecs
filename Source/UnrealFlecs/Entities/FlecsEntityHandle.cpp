// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppTooWideScopeInitStatement
#include "FlecsEntityHandle.h"

#include "Logging/StructuredLog.h"

#include "Logs/FlecsCategories.h"

#include "Worlds/FlecsWorld.h"
#include "Worlds/FlecsWorldSubsystem.h"

#include "Components/FlecsNetworkSerializeDefinitionComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsEntityHandle)

FFlecsEntityHandle FFlecsEntityHandle::GetNullHandle(const TSolidNotNull<const UFlecsWorld*> InWorld)
{
    return flecs::entity::null(InWorld->World);
}

FFlecsEntityHandle::FFlecsEntityHandle(const TSolidNotNull<const UFlecsWorld*> InWorld, const FFlecsId InEntity)
    : FFlecsEntityView(InWorld->World, InEntity)
{
}

FFlecsEntityHandle::FFlecsEntityHandle(const flecs::world_t* InWorld, const FFlecsId InEntity)
    : FFlecsEntityView(InWorld, InEntity)
{
}

bool FFlecsEntityHandle::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
    if (Has<FFlecsNetworkSerializeDefinitionComponent>())
    {
        const FFlecsNetworkSerializeDefinitionComponent& NetSerializeDefinition = Get<FFlecsNetworkSerializeDefinitionComponent>();

        if LIKELY_IF(NetSerializeDefinition.NetSerializeFunction)
        {
            return std::invoke(
                NetSerializeDefinition.NetSerializeFunction,
                *this,
                GetFlecsWorld(),
                Ar,
                Map,
                bOutSuccess);
        }
        else
        {
            UE_LOGFMT(LogFlecsEntity, Warning,
                "Trying to net serialize entity with a nullptr NetSerialize function ptr in "
                "its FFlecsNetworkSerializeDefinitionComponent for Entity {EntityId}, will try global Net Serialize function",);
        }
    }
    
    if LIKELY_IF(Unreal::Flecs::GNetSerializeFunctionPtr)
    {
        return std::invoke(
            *Unreal::Flecs::GNetSerializeFunctionPtr,
            *this,
            GetFlecsWorld(),
            Ar,
            Map,
            bOutSuccess);
    }
    else
    {
        UE_LOGFMT(LogFlecsEntity, Error,
            "Trying to net serialize entity with a nullptr NetSerialize function ptr");
        
        bOutSuccess = false;
        return false;
    }
}
