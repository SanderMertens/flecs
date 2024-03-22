// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlecsEntityActorComponent.generated.h"


UCLASS(BlueprintType, Blueprintable, ClassGroup=(Flecs), meta=(BlueprintSpawnableComponent))
class UNREALFLECS_API UFlecsEntityActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFlecsEntityActorComponent(const FObjectInitializer& ObjectInitializer);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;
	
};
