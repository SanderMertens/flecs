
using System.IO;
using UnrealBuildTool;

public class FlecsLibrary : ModuleRules
{
    public FlecsLibrary(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        OptimizationLevel = OptimizationMode.Speed;
        
        OptimizeCode = CodeOptimization.Always;
        
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
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "SolidMacros"
            }
        );
    }
}