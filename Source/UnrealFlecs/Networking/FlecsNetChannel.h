// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Channel.h"
#include "Worlds/FlecsWorld.h"
#include "FlecsNetChannel.generated.h"

UCLASS()
class UNREALFLECS_API UFlecsNetChannel : public UChannel
{
	GENERATED_BODY()

public:
	UFlecsNetChannel(const FObjectInitializer& ObjectInitializer);
	UFlecsNetChannel();

	UPROPERTY()
	TObjectPtr<UFlecsWorld> FlecsWorld;


}; // class UFlecsNetChannel
