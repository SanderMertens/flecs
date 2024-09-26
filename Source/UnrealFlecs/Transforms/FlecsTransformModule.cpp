// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsTransformModule.h"
#include "FlecsTransformComponent.h"
#include "FlecsTransformEngineSubsystem.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsTransformModule)

UFlecsTransformModule::UFlecsTransformModule()
{
}

void UFlecsTransformModule::InitializeModule(UFlecsWorld* InWorld, const FFlecsEntityHandle& InModuleEntity)
{
    InWorld->CreateSystemWithBuilder(TEXT("FlecsTransformPropagateSystem"))
        .kind(flecs::PreUpdate)
        .with<FFlecsTransformComponent>()
        .with<FFlecsTransformComponent>().up()
        .cached()
        .immediate()
        .run([](flecs::iter& It)
        {
        });
}

void UFlecsTransformModule::DeinitializeModule(UFlecsWorld* InWorld)
{
}
