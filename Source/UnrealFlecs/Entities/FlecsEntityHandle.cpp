// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppTooWideScopeInitStatement
#include "FlecsEntityHandle.h"

#include "Collections/FlecsCollectionWorldSubsystem.h"
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
                GetFlecsWorldChecked(),
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
            GetFlecsWorldChecked(),
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

const FFlecsEntityHandle::FSelfType& FFlecsEntityHandle::AddCollection(const FFlecsId InCollection,
    const FInstancedStruct& InParams) const
{
    solid_checkf(InCollection.IsValid(),
        TEXT("Trying to add an invalid collection to an entity!"));
    solid_checkf(GetNativeFlecsWorld().has<FFlecsCollectionSubsystemSingleton>(),
        TEXT("Trying to add a collection to an entity, but the FlecsCollectionSubsystemSingleton is not registered in the world!"));
    
    const FFlecsCollectionSubsystemSingleton& CollectionSubsystemSingleton
       = GetNativeFlecsWorld().get<FFlecsCollectionSubsystemSingleton>();

    solid_checkf(CollectionSubsystemSingleton.WorldSubsystem.IsValid(),
        TEXT("Trying to add a collection to an entity, but the FlecsCollectionWorldSubsystem is not initialized!"));

    CollectionSubsystemSingleton.WorldSubsystem->AddCollectionToEntity(*this, InCollection, InParams);
    return *this;
}

const FFlecsEntityHandle::FSelfType& FFlecsEntityHandle::AddCollection(const FFlecsCollectionReference& InCollectionRef,
    const FInstancedStruct& InParams) const
{
    solid_checkf(GetNativeFlecsWorld().has<FFlecsCollectionSubsystemSingleton>(),
        TEXT("Trying to add a collection to an entity, but the FlecsCollectionSubsystemSingleton is not registered in the world!"));
    
    const FFlecsCollectionSubsystemSingleton& CollectionSubsystemSingleton
       = GetNativeFlecsWorld().get<FFlecsCollectionSubsystemSingleton>();

    solid_checkf(CollectionSubsystemSingleton.WorldSubsystem.IsValid(),
        TEXT("Trying to add a collection to an entity, but the FlecsCollectionWorldSubsystem is not initialized!"));

    CollectionSubsystemSingleton.WorldSubsystem->AddCollectionToEntity(*this, InCollectionRef, InParams);
    return *this;
}

const FFlecsEntityHandle::FSelfType& FFlecsEntityHandle::AddCollection(
    const FFlecsCollectionInstancedReference& InCollectionRef) const
{
    return AddCollection(InCollectionRef.Collection, InCollectionRef.Parameters);
}

const FFlecsEntityHandle::FSelfType& FFlecsEntityHandle::RemoveCollection(const FFlecsId InCollection) const
{
    solid_checkf(InCollection.IsValid(),
      TEXT("Trying to add an invalid collection to an entity!"));
    solid_checkf(GetNativeFlecsWorld().has<FFlecsCollectionSubsystemSingleton>(),
        TEXT("Trying to add a collection to an entity, but the FlecsCollectionSubsystemSingleton is not registered in the world!"));

    const FFlecsCollectionSubsystemSingleton& CollectionSubsystemSingleton
       = GetNativeFlecsWorld().get<FFlecsCollectionSubsystemSingleton>();

    solid_checkf(CollectionSubsystemSingleton.WorldSubsystem.IsValid(),
        TEXT("Trying to add a collection to an entity, but the FlecsCollectionWorldSubsystem is not initialized!"));

    CollectionSubsystemSingleton.WorldSubsystem->RemoveCollectionFromEntity(*this, InCollection);
    return *this;
}
