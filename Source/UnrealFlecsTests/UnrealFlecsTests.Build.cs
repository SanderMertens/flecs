using UnrealBuildTool;

public class UnrealFlecsTests : ModuleRules
{
    public UnrealFlecsTests(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PrivateIncludePaths.AddRange(
            new string[]
            {
                ModuleDirectory
            }
        );
            
        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "StructUtils",
                "SolidMacros",
                "FlecsLibrary",
                "UnrealFlecs",
                "UnrealEd",
                "AutomationUtils",
                "GameplayTags"
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