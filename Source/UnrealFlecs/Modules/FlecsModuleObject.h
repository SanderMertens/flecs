// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsModuleInterface.h"
#include "UObject/Object.h"
#include "FlecsModuleObject.generated.h"

UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced, ClassGroup = (Flecs))
class UNREALFLECS_API UFlecsModuleObject : public UObject, public IFlecsModuleInterface
{
	GENERATED_BODY()

public:
	UFlecsModuleObject();
	UFlecsModuleObject(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, Category = "Flecs Module",
		meta = (NoElementDuplicate, MustImplement = "/Script/UnrealFlecs.FlecsModuleInterface"))
	TArray<TSubclassOf<UObject>> HardModuleDependencies;
	
	template <Solid::TStaticClassConcept T>
	NO_DISCARD bool HasHardModuleDependency() const
	{
		return HasHardModuleDependency(T::StaticClass());
	}

	UFUNCTION()
	bool HasHardModuleDependency(TSubclassOf<UObject> ModuleClass) const;

	template <Solid::TStaticClassConcept T>
	void AddHardModuleDependency()
	{
		AddModuleDependency(T::StaticClass());
	}

	UFUNCTION()
	void AddHardModuleDependency(TSubclassOf<UObject> ModuleClass);

	virtual TArray<TSubclassOf<UObject>> GetHardDependentModuleClasses() const override;

private:
	NO_DISCARD bool IsValidModuleClass(const TSubclassOf<UObject> ModuleClass) const;
	
}; // class UFlecsModuleObject
