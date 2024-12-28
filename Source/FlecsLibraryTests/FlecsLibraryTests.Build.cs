using UnrealBuildTool;

public class FlecsLibraryTests : ModuleRules
{
    public FlecsLibraryTests(ReadOnlyTargetRules Target) : base(Target)
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
                "UnrealEd",
                "AutomationUtils",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "SolidMacros",
                "FlecsLibrary"
            }
        );
    }
}