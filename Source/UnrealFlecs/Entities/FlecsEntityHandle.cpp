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

    if (WorldName == NAME_None || GetEntity().id() == 0)
    {
        return;
    }
    
    if (GetEntity().world() == nullptr)
    {
        UFlecsWorldSubsystem* FlecsWorldSubsystem = GWorld->GetSubsystem<UFlecsWorldSubsystem>();
        if LIKELY_IF(FlecsWorldSubsystem->HasWorld(WorldName))
        {
            SetEntity(flecs::entity(FlecsWorldSubsystem->GetFlecsWorld(WorldName)->World, GetEntity().id()));
        }
        else
        {
            FDelegateHandle OnWorldCreatedHandle = FlecsWorldSubsystem
                ->OnWorldCreated.AddLambda([&](const FName& Name, const UFlecsWorld* InFlecsWorld)
            {
                if (Name == WorldName)
                {
                    SetEntity(flecs::entity(InFlecsWorld->World, GetEntity().id()));
                }

                FlecsWorldSubsystem->OnWorldCreated.Remove(OnWorldCreatedHandle);
            });
        }
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

bool FFlecsEntityHandle::Has(const FGameplayTag& InTag) const
{
    const FFlecsEntityHandle TypeHandle = GetFlecsWorld()->GetTagEntity(InTag);
    return Has(TypeHandle);
}

void FFlecsEntityHandle::Add(UScriptStruct* StructType) const
{
    const FFlecsEntityHandle TypeHandle = GetFlecsWorld()->ObtainComponentTypeStruct(StructType);
    Add(TypeHandle);
}

void FFlecsEntityHandle::Add(const FGameplayTag& InTag) const
{
    const FFlecsEntityHandle TypeHandle = GetFlecsWorld()->GetTagEntity(InTag);
    Add(TypeHandle);
}

void FFlecsEntityHandle::Remove(UScriptStruct* StructType) const
{
    const FFlecsEntityHandle TypeHandle = GetFlecsWorld()->ObtainComponentTypeStruct(StructType);
    Remove(TypeHandle);
}

void FFlecsEntityHandle::Remove(const FGameplayTag& InTag) const
{
    const FFlecsEntityHandle TypeHandle = GetFlecsWorld()->GetTagEntity(InTag);
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
