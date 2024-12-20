// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealFlecs.h"

#include "Entities/FlecsDefaultEntityEngineSubsystem.h"
#include "Worlds/FlecsOSAPI.h"

#define LOCTEXT_NAMESPACE "FUnrealFlecsModule"

static FOSApiInitializer OSApiInitializer;

void FUnrealFlecsModule::StartupModule()
{
	FCoreDelegates::OnPostEngineInit.AddLambda([]()
	{
		FFlecsDefaultEntityEngine::Get().Initialize();
	});
}

void FUnrealFlecsModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnrealFlecsModule, UnrealFlecs)