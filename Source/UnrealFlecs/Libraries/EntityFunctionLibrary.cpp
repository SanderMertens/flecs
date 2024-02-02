// Solstice Games © 2024. All Rights Reserved.

#include "EntityFunctionLibrary.h"

#include "Worlds/FlecsWorld.h"
#include "Worlds/FlecsWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EntityFunctionLibrary)

FFlecsEntityHandle UEntityFunctionLibrary::CreateEntity(const UObject* WorldContextObject, const FString& EntityName)
{
    FFlecsWorld& World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
    return FFlecsEntityHandle(World.GetWorld().entity(EntityName));
}

FFlecsEntityHandle UEntityFunctionLibrary::GetEntityWithName(const UObject* WorldContextObject,
    const FString& EntityName, const bool bSearchPath)
{
    FFlecsWorld& World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
    const char* EntityNameCStr = TCHAR_TO_ANSI(*EntityName);
    return FFlecsEntityHandle(World.GetWorld().lookup(EntityNameCStr, bSearchPath));
}

void UEntityFunctionLibrary::DestroyEntity(const UObject* WorldContextObject, const FFlecsEntityHandle& Entity)
{
    FFlecsWorld& World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
    World.GetWorld().delete_with(Entity.GetEntity());
}

void UEntityFunctionLibrary::DestroyEntityByName(const UObject* WorldContextObject, const FString& EntityName,
    const bool bSearchPath)
{
    FFlecsWorld& World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
    const char* EntityNameCStr = TCHAR_TO_ANSI(*EntityName);
    World.GetWorld().delete_with(EntityNameCStr, bSearchPath);
}

template <typename ...Args>
FFlecsEntityHandle UEntityFunctionLibrary::CreateEntity(const UObject* WorldContextObject, Args&&... InArgs)
{
    FFlecsWorld& World = UFlecsWorldSubsystem::GetDefaultWorld(WorldContextObject);
    FFlecsEntityHandle EntityHandle(World.GetWorld().entity(std::forward<Args>(InArgs)...));
    return EntityHandle;
}
