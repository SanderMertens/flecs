// Solstice Games © 2024. All Rights Reserved.

#include "EntityFunctionLibrary.h"
#include "Worlds/FlecsWorld.h"
#include "Worlds/FlecsWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EntityFunctionLibrary)

FFlecsEntityHandle UEntityFunctionLibrary::CreateEntity(const UObject* WorldContextObject, const FString& EntityName)
{
    FFlecsWorld& World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
    return FFlecsEntityHandle(World.GetFlecsWorld().entity(TCHAR_TO_ANSI(*EntityName)));
}

FFlecsEntityHandle UEntityFunctionLibrary::GetEntityWithName(const UObject* WorldContextObject,
    const FString& EntityName, const bool bSearchPath)
{
    FFlecsWorld& FlecsWorld = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
    return FFlecsEntityHandle(FlecsWorld.GetFlecsWorld().lookup(TCHAR_TO_ANSI(*EntityName), bSearchPath));
}

void UEntityFunctionLibrary::DestroyEntity(const FFlecsEntityHandle& Entity)
{
    Entity.GetEntity().destruct();
}

void UEntityFunctionLibrary::DestroyEntityByName(const UObject* WorldContextObject, const FString& EntityName,
    const bool bSearchPath)
{
    FFlecsWorld& FlecsWorld = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
    FlecsWorld.GetFlecsWorld().delete_with(*TCHAR_TO_ANSI(*EntityName), bSearchPath);
}