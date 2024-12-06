// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "Entities/FlecsDefaultEntityEngineSubsystem.h"
//#include "FlecsTransformDefaultEntities.generated.h"

// Gets the closest Entity with FGlobalTransform
DEFINE_DEFAULT_ENTITY_OPTION(FlecsLocalTrait);

// Maybe deprecated
DEFINE_DEFAULT_ENTITY_OPTION(FlecsGlobalTrait);

/*USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsRelativeTrait
{
	GENERATED_BODY()
	
	/* How far up the hierarchy to look for the relative entity, 0 is the Parent(Which is FlecsLocalTrait),
	 * 1 is the Parent's Parent, etc.#1#
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Transform")
	int32 RelativeToIndex = 0;
	
}; // struct FFlecsRelativeComponent

DEFINE_DEFAULT_ENTITY_OPTION(FFlecsRelativeRelationTrait);*/