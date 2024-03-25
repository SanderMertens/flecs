// Solstice Games © 2023. All Rights Reserved.

#include "FlecsEntityHandle.h"
#include "Components/FlecsWorldPtrComponent.h"
#include "Worlds/FlecsWorld.h"
#include "Worlds/FlecsWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsEntityHandle)

FFlecsEntityHandle::FFlecsEntityHandle()
{
    if (!GWorld  || !GWorld->IsGameWorld())
    {
        return;
    }
    
    if (GetFlecsWorld() == nullptr)
    {
        const UFlecsWorldSubsystem* FlecsWorldSubsystem = GWorld->GetSubsystem<UFlecsWorldSubsystem>();
        SetEntity(flecs::entity(FlecsWorldSubsystem->GetFlecsWorld(WorldName)->World, GetEntity().id()));
    }
}

UFlecsWorld* FFlecsEntityHandle::GetFlecsWorld() const
{
    return GetEntity().world().get<FFlecsWorldPtrComponent>()->World.Get();
}

UWorld* FFlecsEntityHandle::GetOuterWorld() const
{
    return GetFlecsWorld()->GetSingleton<FFlecsWorldPtrComponent>().OwningWorld.Get();
}

FName FFlecsEntityHandle::GetWorldName() const
{
    return GetFlecsWorld()->GetWorldName();
}

bool FFlecsEntityHandle::Has(UScriptStruct* StructType) const
{
    const FFlecsEntityHandle TypeHandle = GetFlecsWorld()->ObtainComponentTypeStruct(StructType);
    return Has(TypeHandle);
}

void FFlecsEntityHandle::Add(UScriptStruct* StructType) const
{
    const FFlecsEntityHandle TypeHandle = GetFlecsWorld()->ObtainComponentTypeStruct(StructType);
    Add(TypeHandle);
}

void FFlecsEntityHandle::Remove(UScriptStruct* StructType) const
{
    const FFlecsEntityHandle TypeHandle = GetFlecsWorld()->ObtainComponentTypeStruct(StructType);
    Remove(TypeHandle);
}

void FFlecsEntityHandle::Set(UScriptStruct* StructType, const void* InValue) const
{
    const FFlecsEntityHandle TypeHandle = GetFlecsWorld()->ObtainComponentTypeStruct(StructType);
    Set(TypeHandle, InValue);
}

void FFlecsEntityHandle::Set(const FInstancedStruct& InValue) const
{
    const FFlecsEntityHandle TypeHandle = GetFlecsWorld()
        ->ObtainComponentTypeStruct(const_cast<UScriptStruct*>(InValue.GetScriptStruct()));

    Set(TypeHandle, InValue.GetMemory());
}
