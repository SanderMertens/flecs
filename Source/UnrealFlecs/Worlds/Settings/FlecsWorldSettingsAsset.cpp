// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsWorldSettingsAsset.h"

#include "Logging/StructuredLog.h"

#include "Logs/FlecsCategories.h"
#include "Modules/FlecsModuleInterface.h"

#include "Modules/FlecsModuleSetDataAsset.h"
#include "Pipelines/FlecsDefaultGameLoop.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsWorldSettingsAsset)

#define LOCTEXT_NAMESPACE "FlecsWorldSettingsAsset"

UFlecsWorldSettingsAsset::UFlecsWorldSettingsAsset()
{
}

UFlecsWorldSettingsAsset::UFlecsWorldSettingsAsset(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	WorldSettings.WorldName = "DefaultFlecsWorld";
}

void UFlecsWorldSettingsAsset::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITOR
	

#endif // WITH_EDITOR
}

#if WITH_EDITOR

EDataValidationResult UFlecsWorldSettingsAsset::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);
	
	/*if (!IsValid(WorldSettings.GameLoop))
	{
		Context.AddError(FText::Format(
			LOCTEXT("InvalidGameLoop", "WorldSettings {0} has an invalid GameLoop reference."),
			FText::FromString(GetPathName())));
			
		
		Result = EDataValidationResult::Invalid;
	}*/

	if (WorldSettings.GameLoops.IsEmpty())
	{
		Context.AddError(FText::Format(
			LOCTEXT("NoGameLoops", "WorldSettings {0} has no GameLoops assigned."),
			FText::FromString(GetPathName())));
		
		Result = EDataValidationResult::Invalid;
	}
	else
	{
		bool bHasMainLoop = false;
		TArray<TObjectPtr<UObject>> MainLoops;
		
		for (const TObjectPtr<UObject> GameLoop : WorldSettings.GameLoops)
		{
			if (!IsValid(GameLoop))
			{
				Context.AddError(FText::Format(
					LOCTEXT("InvalidGameLoopInArray",
						"WorldSettings {0} has an invalid GameLoop reference in its GameLoops array."),
					FText::FromString(GetPathName())));
				
				Result = EDataValidationResult::Invalid;
				continue;
			}

			if (!GameLoop->Implements<UFlecsGameLoopInterface>())
			{
				Context.AddError(FText::Format(
					LOCTEXT("NonGameLoopInArray",
						"WorldSettings {0} has a GameLoop reference in its GameLoops array that does not implement the FlecsGameLoopInterface."),
					FText::FromString(GetPathName())));
				
				Result = EDataValidationResult::Invalid;
			}

			if (Cast<IFlecsGameLoopInterface>(GameLoop)->IsMainLoop())
			{
				bHasMainLoop = true;
				MainLoops.AddUnique(GameLoop);
			}
		}

		if (!bHasMainLoop)
		{
			Context.AddError(FText::Format(
				LOCTEXT("NoMainLoop",
					"WorldSettings {0} has no GameLoop marked as Main Loop in its GameLoops array."),
				FText::FromString(GetPathName())));
			
			Result = EDataValidationResult::Invalid;
		}

		for (int32 Index = 0; Index < MainLoops.Num(); ++Index)
		{
			const TSolidNotNull<const UObject*> MainLoop = MainLoops[Index];
			
			if (MainLoops.Num() > 1)
			{
				Context.AddError(FText::Format(
					LOCTEXT("MultipleMainLoops",
						"WorldSettings {0} has multiple GameLoops marked as Main Loop in its GameLoops array. Found multiple instances of {1}."),
					FText::FromString(GetPathName()),
					FText::FromString(MainLoop->GetClass()->GetClassPathName().ToString())));
				
				Result = EDataValidationResult::Invalid;
			}
		}
	}

	TArray<TObjectPtr<UObject>> ImportedModules;
	
	ImportedModules.Append(WorldSettings.Modules);
	ImportedModules.Append(WorldSettings.EditorModules);
	
	for (const UFlecsModuleSetDataAsset* ModuleSet : WorldSettings.ModuleSets)
	{
		if (!IsValid(ModuleSet))
		{
			Context.AddError(FText::Format(
				LOCTEXT("InvalidModuleSet",
					"WorldSettings {0} has an invalid ModuleSet reference."),
				FText::FromString(GetPathName())));
			
			Result = EDataValidationResult::Invalid;
			continue;
		}

		ImportedModules.Append(ModuleSet->Modules);
	}

	for (const UFlecsModuleSetDataAsset* ModuleSet : WorldSettings.EditorModuleSets)
	{
		if (!IsValid(ModuleSet))
		{
			Context.AddError(FText::Format(
				LOCTEXT("InvalidEditorModuleSet",
					"WorldSettings {0} has an invalid EditorModuleSet reference."),
				FText::FromString(GetPathName())));
			
			Result = EDataValidationResult::Invalid;
			continue;
		}

		ImportedModules.Append(ModuleSet->Modules);
	}

	for (const UObject* ImportedModule : ImportedModules)
	{
		if UNLIKELY_IF(!IsValid(ImportedModule))
		{
			Context.AddError(FText::Format(
				LOCTEXT("InvalidImportedModule",
					"WorldSettings {0} has an invalid module reference in its imported modules."),
				FText::FromString(GetPathName())));
			
			Result = EDataValidationResult::Invalid;
			continue;
		}

		if (ImportedModule->Implements<UFlecsGameLoopInterface>())
		{
			Context.AddError(FText::Format(
				LOCTEXT("NonModuleImportedModule",
					"WorldSettings {0} has a module reference in its imported modules that implements GameLoopInterface, use GameLoops instead of Module Array."),
				FText::FromString(GetPathName())));
		}
	}

	ImportedModules.Append(WorldSettings.GameLoops);

	const EDataValidationResult DuplicateModuleResult = CheckForDuplicateModules(Context, ImportedModules);
	Result = CombineDataValidationResults(Result, DuplicateModuleResult);

	const EDataValidationResult HardDependencyResult = CheckForHardDependencies(Context, ImportedModules);
	Result = CombineDataValidationResults(Result, HardDependencyResult);
	
	return Result;
}

EDataValidationResult UFlecsWorldSettingsAsset::CheckForDuplicateModules(FDataValidationContext& Context,
	const TArrayView<TObjectPtr<UObject>> ImportedModules) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	TSet<TSubclassOf<UObject>> SeenModules;
	for (const UObject* Module : ImportedModules)
	{
		if UNLIKELY_IF(!Module)
		{
			// We should never reach this point due to validation previously done in the callstack
			UE_LOGFMT(LogFlecsCore, Warning,
				"WorldSettings {PathName} has a null module reference.", GetPathName());
			continue;
		}

		TSubclassOf<UObject> ModuleClass = Module->GetClass();
		
		if (SeenModules.Contains(ModuleClass))
		{
			Context.AddError(FText::Format(
				LOCTEXT("DuplicateModule", "WorldSettings {0} has duplicate module of class {1}."),
				FText::FromString(GetPathName()),
				FText::FromString(ModuleClass->GetClassPathName().ToString())));
			
			Result = EDataValidationResult::Invalid;
		}
		else
		{
			SeenModules.Add(ModuleClass);
		}
	}

	return Result;
}

EDataValidationResult UFlecsWorldSettingsAsset::CheckForHardDependencies(FDataValidationContext& Context,
	TArrayView<TObjectPtr<UObject>> ImportedModules) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;
	
	for (const UObject* Module : ImportedModules)
	{
		if UNLIKELY_IF(!IsValid(Module))
		{
			Context.AddError(FText::Format(
				NSLOCTEXT("Flecs",
				"FlecsModuleSetDataAsset_InvalidModule",
				"Module Set '{0}' has an invalid module reference!"),
				FText::FromName(GetFName())));
			
			Result = EDataValidationResult::Invalid;
			continue;
		}

		if (!Module->Implements<UFlecsModuleInterface>())
		{
			Context.AddError(FText::Format(
				NSLOCTEXT("Flecs",
					"FlecsModuleSetDataAsset_NonModule",
					"Module Set '{0}' has a module '{1}' that does not implement the FlecsModuleInterface!"),
					FText::FromName(GetFName()),
					FText::FromName(Module->GetFName())));
			
			Result = EDataValidationResult::Invalid;
			continue;
		}

		const TSolidNotNull<const IFlecsModuleInterface*> ModuleInterface = CastChecked<IFlecsModuleInterface>(Module);

		const TArray<TSubclassOf<UObject>> HardDependencies = ModuleInterface->GetHardDependentModuleClasses();

		for (const TSubclassOf<UObject> HardDependencyClass : HardDependencies)
		{
			bool bFoundDependency = false;

			if (!HardDependencyClass->ImplementsInterface(UFlecsModuleInterface::StaticClass()))
			{
				Context.AddError(FText::Format(
					NSLOCTEXT("Flecs",
						"FlecsModuleSetDataAsset_NonModuleHardDependency",
						"Module Set '{0}' has a module '{1}' with a hard dependency '{2}' that does not implement the FlecsModuleInterface!"),
						FText::FromName(GetFName()),
						FText::FromName(Module->GetFName()),
						FText::FromName(HardDependencyClass->GetDefaultObjectName())));
				
				Result = EDataValidationResult::Invalid;
				continue;
			}

			for (const UObject* OtherModule : ImportedModules)
			{
				if UNLIKELY_IF(!IsValid(OtherModule))
				{
					continue;
				}

				if (OtherModule->IsA(HardDependencyClass))
				{
					bFoundDependency = true;
					break;
				}
			}

			if UNLIKELY_IF(!bFoundDependency)
			{
				Context.AddWarning(FText::Format(
					NSLOCTEXT("Flecs",
						"FlecsModuleSetDataAsset_MissingHardDependency",
						"Module Set '{0}' is missing hard dependency '{1}' required by module '{2}'!"),
						FText::FromName(GetFName()),
						FText::FromName(HardDependencyClass->GetDefaultObjectName()),
						FText::FromName(Module->GetFName())));
				
				Result = EDataValidationResult::Invalid;
			}
		}
	}

	return Result;
}

#endif // WITH_EDITOR

FPrimaryAssetId UFlecsWorldSettingsAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("FlecsWorld", GetFNameSafe(this));
}

#undef LOCTEXT_NAMESPACE
