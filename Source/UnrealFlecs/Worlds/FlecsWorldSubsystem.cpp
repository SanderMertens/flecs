// Solstice Games © 2024. All Rights Reserved.

#include "FlecsWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecWorldSubsystem)

void UFlecsWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    
}

FFlecsWorld& UFlecsWorldSubsystem::CreateWorld(const FName& Name, const TSet<FName>& Modules)
{
    flecs::world NewWorld = flecs::world();
    FFlecsWorld World(NewWorld);
    
    Worlds.emplace_back(World);
    WorldNameMap[Name] = &Worlds.back();

    World->add<FName>(Name);

    for (const FName& Module : Modules)
    {
        World->
    }
    
    return Worlds.back();
}
