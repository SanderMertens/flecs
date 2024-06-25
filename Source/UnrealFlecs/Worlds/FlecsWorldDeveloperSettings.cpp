// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsWorldDeveloperSettings.h"
#include "FlecsWorldSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsWorldDeveloperSettings)

UFlecsWorldDeveloperSettings::UFlecsWorldDeveloperSettings()
{
}

void UFlecsWorldDeveloperSettings::PostInitProperties()
{
	Super::PostInitProperties();

	#if WITH_EDITOR

	if (Worlds.IsEmpty())
	{
		FFlecsWorldSettings FlecsWorldSettings;
		FlecsWorldSettings.WorldName = DEFAULT_FLECS_WORLD_NAME.data();
		
		Worlds.Add(FlecsWorldSettings);
	}

	#endif // WITH_EDITOR
}
