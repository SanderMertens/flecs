
using UnrealBuildTool;

public class FlecsLibrary : ModuleRules
{
    public FlecsLibrary(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        Type = ModuleType.External;
        
        AppendStringToPublicDefinition("flecs_EXPORTS", "0");
        
        PublicIncludePaths.AddRange(
            new string[] {
                ModuleDirectory,
                ModuleDirectory + "/flecs/include"
            }
        );
        
        PrivateIncludePaths.AddRange(
            new string[] {
                
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "SolidMacros"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore"
            }
        );
    }
}