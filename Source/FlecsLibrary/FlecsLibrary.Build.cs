
using System.IO;
using UnrealBuildTool;

public class FlecsLibrary : ModuleRules
{
    public FlecsLibrary(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
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
        
        if (Target.bBuildEditor)
        {
            PublicDefinitions.Add("FLECS_LIBRARY_WITH_EDITOR=1");
        }
        else
        {
            PublicDefinitions.Add("FLECS_LIBRARY_WITH_EDITOR=0");
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
                "FLECS_ENABLE_SYSTEM_PRIORITY",
            }
        );
        
        // Not Test
        if (Target.Configuration < UnrealTargetConfiguration.Test)
        {
            PublicDefinitions.AddRange(
                new string[]
                {
                    "FLECS_STATS",
                    "FLECS_METRICS",
                    "FLECS_HTTP",
                    "FLECS_REST",
                    "FLECS_DOC",
                    "FLECS_LOG", // @TODO: maybe shouldnt only be Test and below?
                    "FLECS_PERF_TRACE",
                    "FLECS_ACCURATE_COUNTERS",
                    "FLECS_UNITS", // @TODO: maybe shouldnt only be Test and below?
                    "FLECS_ALERTS",
                }
            );
        }
        else // Shipping or Test
        {
            PublicDefinitions.AddRange(
                new string[]
                {
                    "FLECS_DISABLE_COUNTERS",
                }
            );
        }

        if (Target.bCompileAgainstEditor)
        {
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
                "Core", 
                "SolidMacros",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
            }
        );
    }
}
