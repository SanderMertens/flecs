// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/FlecsEntityInterface.h"
#include "UObject/Object.h"
#include "UnrealFlecsObject.generated.h"

class UFlecsWorld;

UCLASS(Abstract, BlueprintType)
class UNREALFLECS_API UUnrealFlecsObject : public UObject, public IFlecsEntityInterface
{
	GENERATED_BODY()

public:
	UUnrealFlecsObject();
	UUnrealFlecsObject(const FObjectInitializer& ObjectInitializer);

	virtual void InitializeFlecsObject(const TSolidNotNull<UFlecsWorld*> InFlecsWorld);
	
	// Inheriting class can override this if they want to customize their Entity creation
	virtual FFlecsEntityHandle CreateObjectEntity();

	virtual void BeginPlay();

	UFUNCTION(BlueprintImplementableEvent, Category = "Flecs", meta = (DisplayName = "Begin Play"))
	void BP_BeginPlay();

	FORCEINLINE virtual FFlecsEntityHandle GetEntityHandle() const override
	{
		return ObjectEntityHandle;
	}

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	UFlecsWorld* GetFlecsWorld() const;
	
	virtual UWorld* GetWorld() const override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	UPROPERTY()
	FFlecsEntityHandle ObjectEntityHandle;

	UPROPERTY()
	TWeakObjectPtr<UFlecsWorld> FlecsWorld;
	
}; // class UUnrealFlecsObject
