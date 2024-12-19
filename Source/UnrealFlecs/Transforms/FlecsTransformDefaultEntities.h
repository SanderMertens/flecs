// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "flecs.h"
#include "Entities/FlecsDefaultEntityEngineSubsystem.h"
//#include "FlecsTransformDefaultEntities.generated.h"

// Gets the closest Entity with FGlobalTransform
DEFINE_DEFAULT_ENTITY_OPTION(FlecsLocalTrait);
DEFINE_DEFAULT_ENTITY_OPTION(FlecsGlobalTrait);

// Doesn't have own Transform, but is relative to another Entity
DEFINE_DEFAULT_ENTITY_OPTION(FlecsRelativeTrait);