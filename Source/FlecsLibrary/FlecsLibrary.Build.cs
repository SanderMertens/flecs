
using System.IO;
using UnrealBuildTool;

public class FlecsLibrary : ModuleRules
{
    public FlecsLibrary(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        CppStandard = CppStandardVersion.Cpp20;
        CStandard = CStandardVersion.Latest;
        
        OptimizationLevel = OptimizationMode.Speed;
        
        PublicDefinitions.AddRange(
            new string[]
            {
                "FLECS_CUSTOM_BUILD",
                "FLECS_CPP",
                "FLECS_MODULE",
                "FLECS_SCRIPT",
                "FLECS_SYSTEM",
                "FLECS_PIPELINE",
                "FLECS_TIMER",
                "FLECS_META",
                "FLECS_JSON",
                "FLECS_UNITS",
                "FLECS_HTTP",
                "FLECS_REST",
                //"flecs_EXPORTS"
            }
        );
        
        // Not Packaged
        if (Target.bBuildEditor)
        {
            PublicDefinitions.AddRange(
                new string[]
                {
                    "FLECS_STATS",
                    "FLECS_METRICS",
                    "FLECS_ALERTS",
                    "FLECS_DOC",
                    "FLECS_LOG",
                    "FLECS_PERF_TRACE",
                    "FLECS_ACCURATE_COUNTERS",
                    
                }
            );
        }
        
        PublicIncludePaths.AddRange(
            new string[] {
                ModuleDirectory + "/ThirdParty/Public",
            }
        );
        
        PrivateIncludePaths.AddRange(
            new string[] {
                ModuleDirectory + "/ThirdParty/Private",
            }
        );
        

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "SolidMacros",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
            }
        );
    }
}