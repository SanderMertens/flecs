// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsTransformEngineSubsystem.h"

#include "Entities/FlecsDefaultEntityEngineSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsTransformEngineSubsystem)

UFlecsTransformEngineSubsystem::UFlecsTransformEngineSubsystem()
{
}

void UFlecsTransformEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	DEFINE_DEFAULT_ENTITY_OPTION(FlecsLocalTrait);
	DEFINE_DEFAULT_ENTITY_OPTION(FlecsRelativeTrait);
	DEFINE_DEFAULT_ENTITY_OPTION(FlecsGlobalTrait);
}
