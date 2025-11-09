// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlecsDependenciesComponent.h"

#include "UObject/Object.h"

#include "FlecsModuleInterface.h"

#include "FlecsModuleObject.generated.h"

class UFlecsWorld;

// @TODO: add documentation for hard/soft dependencies

UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced, ClassGroup = (Flecs))
class UNREALFLECS_API UFlecsModuleObject : public UObject, public IFlecsModuleInterface
{
	GENERATED_BODY()

public:
	UFlecsModuleObject();
	UFlecsModuleObject(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, Category = "Flecs Module",
		meta = (NoElementDuplicate, MustImplement = "/Script/UnrealFlecs.FlecsModuleInterface"))
	TArray<TSubclassOf<UObject>> HardModuleDependencies;
	
	template <Solid::TStaticClassConcept T>
	NO_DISCARD bool HasModuleDependency() const
	{
		return HasModuleDependency(T::StaticClass());
	}

	UFUNCTION()
	bool HasHardDependency(TSubclassOf<UObject> ModuleClass) const;

	// Should be called in the constructor
	template <Solid::TStaticClassConcept T>
	void AddHardDependency()
	{
		AddHardDependency(T::StaticClass());
	}

	// Should be called in the constructor
	void AddHardDependency(TSubclassOf<UObject> ModuleClass);
	
	void RegisterSoftDependency(const TSubclassOf<UObject> ModuleClass,
		const FFlecsDependencyFunctionDefinition::FDependencyFunctionType& DependencyFunction);

	template <Solid::TStaticClassConcept T>
	FORCEINLINE void RegisterSoftDependency(const FFlecsDependencyFunctionDefinition::FDependencyFunctionType& DependencyFunction)
	{
		RegisterSoftDependency(T::StaticClass(), DependencyFunction);
	}

	virtual TArray<TSubclassOf<UObject>> GetHardDependentModuleClasses() const override;
	
}; // class UFlecsModuleObject
