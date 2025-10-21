// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsDeveloperSettings.h"

#include "HAL/IConsoleManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsDeveloperSettings)

static bool bEnableFlecs = true;
static FAutoConsoleVariableRef CVarEnableFlecs(
	TEXT("Flecs.UseFlecs"),
	bEnableFlecs,
	TEXT("Enable Unreal Flecs Plugin.")
);

static bool bUseTaskThreads = false;
static FAutoConsoleVariableRef CVarUseTaskThreads(
	TEXT("Flecs.UseTaskThreads"),
	bUseTaskThreads,
	TEXT("Enable task threads for Flecs.")
);

static int32 TaskThreadCount = 4;
static FAutoConsoleVariableRef CVarTaskThreadCount(
	TEXT("Flecs.TaskThreadCount"),
	TaskThreadCount,
	TEXT("Number of threads to use for Flecs task processing.")
);

// @TODO: Re-enable once tested
/*static bool bShrinkMemoryOnGC = true;
static FAutoConsoleVariableRef CVarShrinkMemoryOnGC(
	TEXT("Flecs.ShrinkMemoryOnGC"),
	bShrinkMemoryOnGC,
	TEXT("Enable shrinking Flecs memory usage during Unreal Garbage Collection.")
);*/
