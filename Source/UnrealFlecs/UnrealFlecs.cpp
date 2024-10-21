// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealFlecs.h"

#include "Entities/FlecsDefaultEntityEngineSubsystem.h"

#define LOCTEXT_NAMESPACE "FUnrealFlecsModule"

void FUnrealFlecsModule::StartupModule()
{
	FCoreDelegates::OnAllModuleLoadingPhasesComplete.AddLambda([]()
	{
		FFlecsDefaultEntityEngine::Get().Initialize();
	});
}

void FUnrealFlecsModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnrealFlecsModule, UnrealFlecs)