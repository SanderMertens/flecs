// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsWorldDeveloperSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsWorldDeveloperSettings)

UFlecsWorldDeveloperSettings::UFlecsWorldDeveloperSettings()
{
}

void UFlecsWorldDeveloperSettings::PostInitProperties()
{
	Super::PostInitProperties();

	if (Worlds.Num() == 0)
	{
		Worlds.Add(FFlecsWorldSettings());
	}
}
