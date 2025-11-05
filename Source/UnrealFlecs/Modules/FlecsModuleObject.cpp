// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsModuleObject.h"

#include "Logs/FlecsCategories.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsModuleObject)

UFlecsModuleObject::UFlecsModuleObject()
{
}

UFlecsModuleObject::UFlecsModuleObject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UFlecsModuleObject::HasHardModuleDependency(const TSubclassOf<UObject> ModuleClass) const
{
	return HardModuleDependencies.Contains(ModuleClass);
}

void UFlecsModuleObject::AddHardModuleDependency(const TSubclassOf<UObject> ModuleClass)
{
	if UNLIKELY_IF(ensureAlwaysMsgf(!IsValid(ModuleClass), TEXT("ModuleClass is not valid!")))
	{
		return;
	}

	if UNLIKELY_IF(!IsValidModuleClass(ModuleClass))
	{
		UE_LOGFMT(LogFlecsWorld, Error,
			"ModuleClass {ModuleName} does not implement UFlecsModuleInterface!", ModuleClass->GetName());
		return;
	}

	HardModuleDependencies.AddUnique(ModuleClass);
}

TArray<TSubclassOf<UObject>> UFlecsModuleObject::GetHardDependentModuleClasses() const
{
	return HardModuleDependencies;
}

bool UFlecsModuleObject::IsValidModuleClass(const TSubclassOf<UObject> ModuleClass) const
{
	return ModuleClass->ImplementsInterface(UFlecsModuleInterface::StaticClass());
}
