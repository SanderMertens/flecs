// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsTransformWorldSubsystem.h"
#include "FlecsTransformComponent.h"
#include "FlecsTransformEngineSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsTransformWorldSubsystem)

void UFlecsTransformWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	solid_checkf(GetFlecsWorld(), TEXT("FlecsWorld is not initialized!"));

	GetFlecsWorld()->CreateSystemWithBuilder(TEXT("FlecsTransformPropagateSystem"))
		.with(FlecsLocalTrait)
		.or_()
		.with(FlecsGlobalTrait)
		.or_()
		.with<FFlecsRelativeTrait>()
		.with<FFlecsTransformComponent>().parent().cascade().read_write()
		.kind(flecs::PreUpdate)
		.cached()
		.run([](flecs::iter& It)
		{
			while (It.next())
			{
				if (!It.changed())
				{
					It.skip();
					continue;
				}
			}
		});
		
}

void UFlecsTransformWorldSubsystem::Deinitialize()
{
	Super::Deinitialize();
}
