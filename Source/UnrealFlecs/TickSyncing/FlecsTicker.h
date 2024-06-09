// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Modules/FlecsModuleInterface.h"
#include "FlecsTicker.generated.h"

UCLASS(BlueprintType)
class UNREALFLECS_API UFlecsTicker final : public UObject, public IFlecsModuleInterface, public FTickableGameObject
{
	GENERATED_BODY()

public:

	static constexpr const TCHAR* ModuleName = TEXT("FlecsTickerModule");

	virtual FString GetModuleName_Implementation() const override
	{
		return ModuleName;
	}

	FORCEINLINE virtual void Tick(float DeltaTime) override
	{
		
	}
	
	FORCEINLINE virtual bool IsTickable() const override
	{
		return true;
	}

	FORCEINLINE virtual TStatId GetStatId() const override
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UFlecsTicker, STATGROUP_Tickables);
	}
	
	FORCEINLINE virtual bool IsTickableWhenPaused() const override
	{
		return true;
	}
	
	FORCEINLINE virtual bool IsTickableInEditor() const override
	{
		return false;
	}

	
	
}; // class UFlecsTicker


