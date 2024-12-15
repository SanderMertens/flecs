// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UnrealFlecs : ModuleRules
{
	public UnrealFlecs(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		CppStandard = CppStandardVersion.Latest;

		OptimizationLevel = OptimizationMode.Speed;

		bUseUnity = false;
		
		IWYUSupport = IWYUSupport.Full;
		
		PublicIncludePaths.AddRange(
			new string[] {
				ModuleDirectory
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"FlecsLibrary",
				"SolidMacros",
				"GameplayTags",
				"AssetRegistry",
				"StructUtils",
				"NetCore",
				"NetCommon",
				"PhysicsCore",
                "Chaos",
                "ChaosCore",
                "ChaosSolverEngine",
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"DeveloperSettings",
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
