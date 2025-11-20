// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "UnrealFlecs.h"

#include "Misc/CoreDelegates.h"

#include "Versioning/UnrealFlecsVersion.h"

#include "General/FlecsOSAPI.h"
#include "Entities/FlecsDefaultEntityEngine.h"

#if !defined(SOLID_MACROS_VERSION) || SOLID_MACROS_VERSION != EXPECTED_SOLID_MACROS_VERSION
#error "The Solid Macros version is not as expected. Please update the Solid Macros to the latest version."
#endif // #if !defined(SOLID_MACROS_VERSION) || SOLID_MACROS_VERSION != EXPECTED_SOLID_MACROS_VERSION

#define LOCTEXT_NAMESPACE "FUnrealFlecsModule"

namespace Unreal::Flecs
{
	// ReSharper disable once CppDeclaratorNeverUsed
	static FOSApiInitializer OSApiInitializer;
	
} // namespace Unreal::Flecs

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

#undef LOCTEXT_NAMESPACE // "FUnrealFlecsModule"
	
IMPLEMENT_MODULE(FUnrealFlecsModule, UnrealFlecs)