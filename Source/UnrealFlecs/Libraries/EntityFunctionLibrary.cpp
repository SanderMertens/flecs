// Solstice Games © 2024. All Rights Reserved.

#include "EntityFunctionLibrary.h"
#include "Worlds/FlecsWorld.h"
#include "Worlds/FlecsWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EntityFunctionLibrary)

FFlecsEntityHandle UEntityFunctionLibrary::CreateEntity(const UObject* WorldContextObject, const FString& EntityName)
{
    FFlecsWorld& World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
    char* EntityNameCStr = TCHAR_TO_ANSI(*EntityName);
    return FFlecsEntityHandle(World.GetFlecsWorld().entity(EntityNameCStr));
}

FFlecsEntityHandle UEntityFunctionLibrary::GetEntityWithName(const UObject* WorldContextObject,
    const FString& EntityName, const bool bSearchPath)
{
    FFlecsWorld& FlecsWorld = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
    const char* EntityNameCStr = TCHAR_TO_ANSI(*EntityName);
    return FFlecsEntityHandle(FlecsWorld.GetFlecsWorld().lookup(EntityNameCStr, bSearchPath));
}

void UEntityFunctionLibrary::DestroyEntity(const UObject* WorldContextObject, const FFlecsEntityHandle& Entity)
{
    FFlecsWorld& FlecsWorld = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
    FlecsWorld.GetFlecsWorld().delete_with(Entity.GetEntity());
}

void UEntityFunctionLibrary::DestroyEntityByName(const UObject* WorldContextObject, const FString& EntityName,
    const bool bSearchPath)
{
    FFlecsWorld& FlecsWorld = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
    const char* EntityNameCStr = TCHAR_TO_ANSI(*EntityName);
    FlecsWorld.GetFlecsWorld().delete_with(*EntityNameCStr, bSearchPath);
}