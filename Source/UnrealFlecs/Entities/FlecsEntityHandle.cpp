// Solstice Games © 2023. All Rights Reserved.

#include "FlecsEntityHandle.h"
#include "Worlds/FlecsWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsEntityHandle)

UFlecsWorldSubsystem* FFlecsEntityHandle::GetFlecsWorldSubsystem() const
{
    return static_cast<UFlecsWorldSubsystem*>(GetEntity()
                                              .world().get_ctx());
}

FFlecsWorld& FFlecsEntityHandle::GetFlecsWorld() const
{
    return GetFlecsWorldSubsystem()->GetFlecsWorld(*GetEntity().world().get<FName>());
}

bool FFlecsEntityHandle::Has(UScriptStruct* StructType) const
{
    const FFlecsEntityHandle TypeHandle = GetFlecsWorldSubsystem()->ObtainComponentTypeStruct(StructType);
    return Has(TypeHandle);
}

void FFlecsEntityHandle::Add(UScriptStruct* StructType) const
{
    const FFlecsEntityHandle TypeHandle = GetFlecsWorldSubsystem()->ObtainComponentTypeStruct(StructType);
    Add(TypeHandle);
}

void FFlecsEntityHandle::Remove(UScriptStruct* StructType) const
{
    const FFlecsEntityHandle TypeHandle = GetFlecsWorldSubsystem()->ObtainComponentTypeStruct(StructType);
    Remove(TypeHandle);
}

void FFlecsEntityHandle::Set(UScriptStruct* StructType, const void* InValue) const
{
    const FFlecsEntityHandle TypeHandle = GetFlecsWorldSubsystem()->ObtainComponentTypeStruct(StructType);
    Set(TypeHandle, InValue);
}

void FFlecsEntityHandle::Set(const FInstancedStruct& InValue) const
{
    const FFlecsEntityHandle TypeHandle = GetFlecsWorldSubsystem()
        ->ObtainComponentTypeStruct(const_cast<UScriptStruct*>(InValue.GetScriptStruct()));

    Set(TypeHandle, InValue.GetMemory());
}
