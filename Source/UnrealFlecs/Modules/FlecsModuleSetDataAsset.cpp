// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsModuleSetDataAsset.h"
#include "FlecsModuleInterface.h"
#include "Misc/DataValidation.h"
#include "SolidMacros/Macros.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsModuleSetDataAsset)

void UFlecsModuleSetDataAsset::ImportModules(const TSolidNotNull<UFlecsWorld*> InWorld)
{
	for (UObject* Module : Modules)
	{
		if UNLIKELY_IF(!ensureAlways(IsValid(Module)))
		{
			continue;
		}

		solid_check(Module->GetClass()->ImplementsInterface(UFlecsModuleInterface::StaticClass()));

		InWorld->ImportModule(Module);
	}
}

TArray<TSubclassOf<UObject>> UFlecsModuleSetDataAsset::GetModuleHardDependencies() const
{
	TArray<TSubclassOf<UObject>> HardDependencies;

	for (const UObject* Module : Modules)
	{
		if UNLIKELY_IF(!IsValid(Module))
		{
			continue;
		}

		solid_check(Module->GetClass()->ImplementsInterface(UFlecsModuleInterface::StaticClass()));

		const TSolidNotNull<const IFlecsModuleInterface*> ModuleInterface = Cast<IFlecsModuleInterface>(Module);

		const TArray<TSubclassOf<UObject>> ModuleHardDependencies = ModuleInterface->GetHardDependentModuleClasses();
		
		for (const TSubclassOf<UObject> HardDependencyClass : ModuleHardDependencies)
		{
			if (!HardDependencies.Contains(HardDependencyClass))
			{
				HardDependencies.AddUnique(HardDependencyClass);
			}
		}
	}

	return HardDependencies;
}

#if WITH_EDITOR

EDataValidationResult UFlecsModuleSetDataAsset::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult SuperResult = Super::IsDataValid(Context);
	
	EDataValidationResult Result = EDataValidationResult::Valid;

	for (const UObject* Module : Modules)
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

		if UNLIKELY_IF(!Module->GetClass()->ImplementsInterface(UFlecsModuleInterface::StaticClass()))
		{
			Context.AddError(FText::Format(
				NSLOCTEXT("Flecs",
					"FlecsModuleSetDataAsset_NonModule",
					"Module Set '{0}' has a non-module reference to '{1}'!"),
					FText::FromName(GetFName()), FText::FromName(Module->GetFName())));
			
			Result = EDataValidationResult::Invalid;
			continue;
		}

		const TSolidNotNull<const IFlecsModuleInterface*> ModuleInterface = Cast<IFlecsModuleInterface>(Module);

		const TArray<TSubclassOf<UObject>> HardDependencies = ModuleInterface->GetHardDependentModuleClasses();

		for (const TSubclassOf<UObject> HardDependencyClass : HardDependencies)
		{
			bool bFoundDependency = false;

			for (const UObject* OtherModule : Modules)
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
				Context.AddError(FText::Format(
					NSLOCTEXT("Flecs",
						"FlecsModuleSetDataAsset_MissingHardDependency",
						"Module Set '{0}' is missing hard dependency '{1}' required by module '{2}'!"),
						FText::FromName(GetFName()),
						FText::FromString(HardDependencyClass->GetClassPathName().ToString()),
						FText::FromName(Module->GetFName())));
				
				Result = EDataValidationResult::Invalid;
			}
		}
	}

	return CombineDataValidationResults(Result, SuperResult);
}

#endif // WITH_EDITOR