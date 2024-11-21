// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsWorldNameComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsWorldNameComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | World")
	FString WorldName;
	
}; // struct FFlecsWorldNameComponent

