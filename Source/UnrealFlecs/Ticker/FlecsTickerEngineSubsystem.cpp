// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsTickerEngineSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsTickerEngineSubsystem)

UFlecsTickerEngineSubsystem::UFlecsTickerEngineSubsystem()
{
}

void UFlecsTickerEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	DEFINE_DEFAULT_ENTITY_OPTION(FlecsFixedTick);
}
