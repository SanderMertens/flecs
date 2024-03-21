// Solstice Games © 2024. All Rights Reserved.

#include "EntityFunctionLibrary.h"
#include "Worlds/FlecsWorld.h"
#include "Worlds/FlecsWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EntityFunctionLibrary)

FFlecsEntityHandle UEntityFunctionLibrary::CreateEntity(const UObject* WorldContextObject, const FString& EntityName)
{
    const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
    return FFlecsEntityHandle(World->World.entity(TCHAR_TO_ANSI(*EntityName)));
}

FFlecsEntityHandle UEntityFunctionLibrary::GetEntityWithName(const UObject* WorldContextObject,
    const FString& EntityName, const bool bSearchPath)
{
    const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
    return FFlecsEntityHandle(World->World.lookup(TCHAR_TO_ANSI(*EntityName), bSearchPath));
}

void UEntityFunctionLibrary::DestroyEntity(const FFlecsEntityHandle& Entity)
{
    Entity.Destroy();
}

void UEntityFunctionLibrary::DestroyEntityByName(const UObject* WorldContextObject, const FString& EntityName,
    const bool bSearchPath)
{
    const UFlecsWorld* World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
    World->World.delete_with(*TCHAR_TO_ANSI(*EntityName), bSearchPath);
}