// Elie Wiese-Namir © 2025. All Rights Reserved.

// ReSharper disable CppTooWideScopeInitStatement
#include "FlecsEntityHandle.h"
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

FFlecsEntityHandle::FFlecsEntityHandle(const UFlecsWorld* InWorld, const FFlecsId InEntity)
{
    SetEntity(flecs::entity(InWorld->World, InEntity));
}

FFlecsEntityHandle::FFlecsEntityHandle(const flecs::world_t* InWorld, const FFlecsId InEntity)
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
    return true;
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
    ComponentCollection->ApplyCollection_Internal(*this, GetFlecsWorld());
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

