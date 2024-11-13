// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/FlecsEntityHandle.h"
#include "UObject/Object.h"
#include "FlecsPrefabObject.generated.h"

class UFlecsWorld;

UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, ClassGroup = (Flecs))
class UNREALFLECS_API UFlecsPrefabObject : public UObject
{
	GENERATED_BODY()

public:
	void CreatePrefab(UFlecsWorld* InWorld);

	virtual void OnPrefabCreated(const FFlecsEntityHandle& InPrefab) {}

	UFUNCTION(BlueprintImplementableEvent, Category = "Flecs", meta = (DisplayName = "OnPrefabCreated"))
	void BP_OnPrefabCreated(const FFlecsEntityHandle& InPrefab);

	virtual UWorld* GetWorld() const override;

	UPROPERTY(EditAnywhere, Category = "Flecs")
	FString PrefabName;

	UPROPERTY()
	FFlecsEntityHandle PrefabEntityHandle;

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE FFlecsEntityHandle GetPrefabHandle() const { return PrefabEntityHandle; }

	UFUNCTION(BlueprintCallable, Category = "Flecs")
	FORCEINLINE UFlecsWorld* GetFlecsWorld() const { return FlecsWorld.Get(); }

protected:
	UPROPERTY()
	TWeakObjectPtr<UFlecsWorld> FlecsWorld;
	
}; // class UFlecsPrefabObject
