// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppTooWideScopeInitStatement
#include "FlecsEntityHandle.h"
#include "Collections/CollectionTrackerComponent.h"
#include "Collections/FlecsComponentCollectionObject.h"
#include "Components/FlecsWorldPtrComponent.h"
#include "Networking/FlecsNetworkIdComponent.h"
#include "Worlds/FlecsWorld.h"
#include "Worlds/FlecsWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsEntityHandle)

FFlecsEntityHandle FFlecsEntityHandle::GetNullHandle(const UFlecsWorld* InWorld)
{
    solid_checkf(InWorld, TEXT("Flecs World not found"));
    return flecs::entity::null(InWorld->World);
}

FFlecsEntityHandle::FFlecsEntityHandle()
{
}

FFlecsEntityHandle::FFlecsEntityHandle(const UFlecsWorld* InWorld, const flecs::entity_t InEntity)
{
    SetEntity(flecs::entity(InWorld->World, InEntity));
}

FFlecsEntityHandle::FFlecsEntityHandle(const flecs::world_t* InWorld, const flecs::entity_t InEntity)
{
    SetEntity(flecs::entity(InWorld, InEntity));
}

UFlecsWorld* FFlecsEntityHandle::GetFlecsWorld() const
{
    return ToFlecsWorld(GetEntity().world());
}

UWorld* FFlecsEntityHandle::GetOuterWorld() const
{
    solid_checkf(::IsValid(GetFlecsWorld()), TEXT("Flecs World not found"));
    return GetFlecsWorld()->GetSingleton<FUWorldPtrComponent>().World.Get();
}

FString FFlecsEntityHandle::GetWorldName() const
{
    return GetFlecsWorld()->GetWorldName();
}

bool FFlecsEntityHandle::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
    /*if (Ar.IsLoading())
    {
        Info.NetSerialize(Ar, Map, bOutSuccess);

        const UFlecsWorldSubsystem* FlecsWorldSubsystem = GWorld->GetSubsystem<UFlecsWorldSubsystem>();
        solid_checkf(FlecsWorldSubsystem, TEXT("Flecs World Subsystem not found"));

        const flecs::query<const FFlecsNetworkIdComponent> Query = FlecsWorldSubsystem->GetDefaultWorld()->World
            .query<const FFlecsNetworkIdComponent>();

        const FFlecsEntityHandle EntityHandle = Query.find([](const FFlecsNetworkIdComponent& InNetworkId)
        {
            return InNetworkId == Info.NetworkId;
        });

        if (EntityHandle.IsValid())
        {
            SetEntity(EntityHandle.GetEntity());
        }
        else
        {
            SetEntity(FlecsWorldSubsystem->GetDefaultWorld()->LookupEntity(Info.EntityName).GetEntity());
        }
        
        bOutSuccess = true;
        return true;
    }
    else
    {
        solid_checkf(Has<FFlecsNetworkIdComponent>(), TEXT("Entity does not have a network ID component"));
        solid_checkf(Get<FFlecsNetworkIdComponent>().IsValid(), TEXT("Network ID component is not valid"));

        FNetworkedEntityInfo Info;
        Info.NetworkId = Get<FFlecsNetworkIdComponent>();
        Info.EntityName = GetName();
        Info.WorldName = GetWorldName();

        Info.NetSerialize(Ar, Map, bOutSuccess);
        bOutSuccess = true;
        return true;
    }*/

    return false;
}

FFlecsEntityHandle FFlecsEntityHandle::ObtainComponentTypeStruct(const UScriptStruct* StructType) const
{
    solid_checkf(::IsValid(StructType), TEXT("Struct type is not valid"));
    return GetFlecsWorld()->ObtainComponentTypeStruct(StructType);
}

void FFlecsEntityHandle::AddCollection(UObject* Collection) const
{
    solid_check(::IsValid(Collection));
    UFlecsComponentCollectionObject* ComponentCollection = CastChecked<UFlecsComponentCollectionObject>(Collection);
    ComponentCollection->ApplyCollection_Internal(const_cast<FFlecsEntityHandle&>(*this), GetFlecsWorld());
}

bool FFlecsEntityHandle::HasCollection(const UClass* CollectionClass) const
{
    solid_check(::IsValid(CollectionClass));

    const FString CollectionName = UFlecsComponentCollectionObject::GetCollectionTypeName(CollectionClass);
    const FCollectionTrackerComponent* CollectionTracker = GetPtr<FCollectionTrackerComponent>();
    return CollectionTracker && CollectionTracker->ComponentProperties.contains(CollectionName);
}

void FFlecsEntityHandle::RemoveCollection(const FString& CollectionName) const
{
    /*solid_checkf(!CollectionName.IsEmpty(), TEXT("Collection name is empty"));
    FCollectionTrackerComponent* CollectionTracker = GetEntity().get_mut<FCollectionTrackerComponent>();

    solid_checkf(CollectionTracker->ComponentProperties.contains(CollectionName),
        TEXT("Collection not found!"));
    
    const FFlecsEntityHandle CollectionEntity = CollectionTracker->ComponentProperties[CollectionName];
    
    RemovePrefab(CollectionEntity);
    CollectionTracker->ComponentProperties.erase(CollectionName);
    
    Modified<FCollectionTrackerComponent>();*/
}

void FFlecsEntityHandle::RemoveCollection(const UClass* Collection) const
{
    // solid_checkf(Collection, TEXT("Collection class is null!"));
    // RemoveCollection(UFlecsComponentCollectionObject::GetCollectionTypeName(Collection));
}

FFlecsEntityHandle FFlecsEntityHandle::GetTagEntity(const FGameplayTag& InTag) const
{
    return GetFlecsWorld()->GetTagEntity(InTag);
}

void FFlecsEntityHandle::ObtainFlecsWorld()
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FFlecsEntityHandle::ObtainFlecsWorld);
    
    if (!GWorld || !GWorld->IsGameWorld())
    {
        return;
    }
    
    if (GetEntity().world() == nullptr)
    {
        const UFlecsWorldSubsystem* FlecsWorldSubsystem = GWorld->GetSubsystem<UFlecsWorldSubsystem>();
        solid_check(::IsValid(FlecsWorldSubsystem));

        SetEntity(flecs::entity(FlecsWorldSubsystem->GetDefaultWorld()->World, GetEntity().id()));
    }
}

void FFlecsEntityHandle::PostScriptConstruct()
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FFlecsEntityHandle::PostScriptConstruct);
    
    if (GetEntity().id() == 0)
    {
        return;
    }
    
    if UNLIKELY_IF(!GWorld  || !GWorld->IsGameWorld())
    {
        return;
    }
    
    if (GetEntity().world() == nullptr)
    {
        UFlecsWorldSubsystem* FlecsWorldSubsystem = GWorld->GetSubsystem<UFlecsWorldSubsystem>();
        solid_check(::IsValid(FlecsWorldSubsystem));
        
        FlecsWorldSubsystem->ListenBeginPlay(FOnWorldBeginPlay::FDelegate::CreateLambda(
            [this, FlecsWorldSubsystem](MAYBE_UNUSED UWorld* InWorld)
        {
            SetEntity(flecs::entity(FlecsWorldSubsystem->GetDefaultWorld()->World, GetEntity().id()));
        }));
    }
}

