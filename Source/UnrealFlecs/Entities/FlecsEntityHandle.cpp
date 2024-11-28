// Solstice Games © 2023. All Rights Reserved.

// ReSharper disable CppTooWideScopeInitStatement
#include "FlecsEntityHandle.h"
#include "Components/FlecsWorldPtrComponent.h"
#include "Components/UWorldPtrComponent.h"
#include "Networking/FlecsNetworkingActorComponent.h"
#include "Worlds/FlecsWorld.h"
#include "Worlds/FlecsWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsEntityHandle)

FFlecsEntityHandle::FFlecsEntityHandle()
{
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
    solid_checkf(GetFlecsWorld(), TEXT("Flecs World not found"));
    return GetFlecsWorld()->GetSingleton<FUWorldPtrComponent>().World.Get();
}

FString FFlecsEntityHandle::GetWorldName() const
{
    return GetFlecsWorld()->GetWorldName();
}

bool FFlecsEntityHandle::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
    if (Ar.IsLoading())
    {
        FNetworkedEntityInfo Info;
        Info.NetSerialize(Ar, Map, bOutSuccess);

        const UFlecsWorldSubsystem* FlecsWorldSubsystem = GWorld->GetSubsystem<UFlecsWorldSubsystem>();
        solid_checkf(FlecsWorldSubsystem, TEXT("Flecs World Subsystem not found"));

        const flecs::query<const FFlecsNetworkIdComponent> Query = FlecsWorldSubsystem->GetDefaultWorld()->World
            .query<const FFlecsNetworkIdComponent>();

        const FFlecsEntityHandle EntityHandle = Query.find([&](const FFlecsNetworkIdComponent& InNetworkId)
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
    }
}

FFlecsEntityHandle FFlecsEntityHandle::ObtainComponentTypeStruct(const UScriptStruct* StructType) const
{
    return GetFlecsWorld()
        ->ObtainComponentTypeStruct(const_cast<UScriptStruct*>(StructType));
}

FFlecsEntityHandle FFlecsEntityHandle::GetTagEntity(const FGameplayTag& InTag) const
{
    return GetFlecsWorld()->GetTagEntity(InTag);
}

void FFlecsEntityHandle::ObtainFlecsWorld()
{
    if (!GWorld || !GWorld->IsGameWorld())
    {
        return;
    }
    
    if (GetEntity().world() == nullptr)
    {
        const UFlecsWorldSubsystem* FlecsWorldSubsystem = GWorld->GetSubsystem<UFlecsWorldSubsystem>();
        solid_checkf(FlecsWorldSubsystem, TEXT("Flecs World Subsystem not found"));

        SetEntity(flecs::entity(FlecsWorldSubsystem->GetDefaultWorld()->World,
            GetEntity().id()));
    }
}

void FFlecsEntityHandle::PostScriptConstruct()
{
    if (GetEntity().id() == 0)
    {
        return;
    }
    
    if (!GWorld  || !GWorld->IsGameWorld())
    {
        return;
    }
    
    if (GetEntity().world() == nullptr)
    {
        UFlecsWorldSubsystem* FlecsWorldSubsystem = GWorld->GetSubsystem<UFlecsWorldSubsystem>();
        if LIKELY_IF(FlecsWorldSubsystem->HasValidFlecsWorld())
        {
            SetEntity(flecs::entity(FlecsWorldSubsystem->GetDefaultWorld()->World,
                GetEntity().id()));
        }
        else
        {
            FDelegateHandle OnWorldCreatedHandle = FlecsWorldSubsystem
                ->OnWorldCreated.AddLambda([&](const UFlecsWorld* InFlecsWorld)
            {
                SetEntity(flecs::entity(InFlecsWorld->World, GetEntity().id()));

                FlecsWorldSubsystem->OnWorldCreated.Remove(OnWorldCreatedHandle);
            });
        }
    }
}

