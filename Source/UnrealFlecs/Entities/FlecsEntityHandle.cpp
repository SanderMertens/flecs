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

bool FFlecsEntityHandle::Has(const UScriptStruct* StructType) const
{
    return Has(ObtainComponentTypeStruct(StructType));
}

bool FFlecsEntityHandle::Has(const FGameplayTag& InTag) const
{
    return Has(GetTagEntity(InTag));
}

void FFlecsEntityHandle::Add(const UScriptStruct* StructType) const
{
    Add(ObtainComponentTypeStruct(StructType));
}

void FFlecsEntityHandle::Add(const FGameplayTag& InTag) const
{
    Add(GetTagEntity(InTag));
}

void FFlecsEntityHandle::Remove(const UScriptStruct* StructType) const
{
    Remove(ObtainComponentTypeStruct(StructType));
}

void FFlecsEntityHandle::Remove(const FGameplayTag& InTag) const
{
    Remove(GetTagEntity(InTag));
}

void FFlecsEntityHandle::Set(const UScriptStruct* StructType, const void* InValue) const
{
    Set(ObtainComponentTypeStruct(StructType), InValue);
}

void FFlecsEntityHandle::Set(const FInstancedStruct& InValue) const
{
    Set(ObtainComponentTypeStruct(InValue.GetScriptStruct()), InValue.GetMemory());
}

const void* FFlecsEntityHandle::GetPtr(const UScriptStruct* StructType) const
{
    return GetPtr(ObtainComponentTypeStruct(StructType));
}

void FFlecsEntityHandle::Enable(const UScriptStruct* StructType) const
{
    Enable(ObtainComponentTypeStruct(StructType));
}

void FFlecsEntityHandle::Disable(const UScriptStruct* StructType) const
{
    Disable(ObtainComponentTypeStruct(StructType));
}

bool FFlecsEntityHandle::IsEnabled(const UScriptStruct* StructType) const
{
    return IsEnabled(ObtainComponentTypeStruct(StructType));
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

