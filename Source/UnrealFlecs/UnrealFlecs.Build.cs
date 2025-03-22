// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UnrealFlecs : ModuleRules
{
	public UnrealFlecs(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		CppStandard = CppStandardVersion.Latest;
		OptimizationLevel = OptimizationMode.Speed;
		IWYUSupport = IWYUSupport.Full;

		PublicIncludePaths.AddRange(
			new string[] {
				ModuleDirectory
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
                "DeveloperSettings",
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
