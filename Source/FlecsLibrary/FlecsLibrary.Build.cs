
using System.IO;
using UnrealBuildTool;

public class FlecsLibrary : ModuleRules
{
    public FlecsLibrary(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        const bool bCompileWithLibraryTests = false;
        
        Type = ModuleType.CPlusPlus;
        
        CppStandard = CppStandardVersion.Cpp20;
        CStandard = CStandardVersion.Latest;
        
        OptimizationLevel = OptimizationMode.Speed;
        
        bool bIsMonolithic = Target.LinkType == TargetLinkType.Monolithic;
        
        if (bIsMonolithic)
        {
            PublicDefinitions.Add("flecs_STATIC");
        }
        else
        {
            // When compiling flecs.c, make sure to define flecs_EXPORTS, for example by adding -Dflecs_EXPORTS to the compiler command.
            // https://www.flecs.dev/flecs/md_docs_2Quickstart.html
            PrivateDefinitions.Add("flecs_EXPORTS");
        }
        
        if (bCompileWithLibraryTests)
        {
            PublicDefinitions.Add("FLECS_TESTS");
        }
        
        PublicDefinitions.AddRange(
            new string[]
            {
                "FLECS_CPP",
                "FLECS_MODULE",
                "FLECS_SCRIPT",
                "FLECS_PARSER",
                "FLECS_QUERY_DSL",
                "FLECS_SYSTEM",
                "FLECS_PIPELINE",
                "FLECS_TIMER",
                "FLECS_META",
                "FLECS_JSON",
                "FLECS_SCRIPT_MATH",
            }
        );
        
        // Not Packaged
        if (Target.Configuration != UnrealTargetConfiguration.Shipping)
        {
            PublicDefinitions.AddRange(
                new string[]
                {
                    "FLECS_STATS",
                    "FLECS_METRICS",
                    "FLECS_HTTP",
                    "FLECS_REST",
                    "FLECS_DOC",
                    "FLECS_LOG",
                    "FLECS_PERF_TRACE",
                    "FLECS_ACCURATE_COUNTERS",
                    "FLECS_UNITS",
                    "FLECS_ALERTS",
                    "FLECS_KEEP_ASSERT",
                   // "FLECS_SANITIZE",
                }
            );
        }
        
        PublicIncludePaths.AddRange(
            new string[] {
                ModuleDirectory + "/Public",
            }
        );
        
        PrivateIncludePaths.AddRange(
            new string[] {
                ModuleDirectory + "/Private",
                ModuleDirectory + "/Tests",
                ModuleDirectory + "/Fixtures",
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
