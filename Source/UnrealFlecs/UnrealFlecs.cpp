// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "UnrealFlecs.h"

#include "Entities/FlecsDefaultEntityEngineSubsystem.h"
#include "Worlds/FlecsOSAPI.h"
#include "Versioning/UnrealFlecsVersion.h"

#if !defined(SOLID_MACROS_VERSION) || SOLID_MACROS_VERSION != EXPECTED_SOLID_MACROS_VERSION
#error "The Solid Macros version is not as expected. Please update the Solid Macros to the latest version."
#endif

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