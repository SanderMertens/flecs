// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsScriptClassComponent.h"
#include "FlecsScriptEnumComponent.h"
#include "FlecsScriptStructComponent.h"
#include "Entities/FlecsEntityHandle.h"
#include "Standard/robin_hood.h"
#include "FlecsTypeMapComponent.generated.h"

USTRUCT()
struct FFlecsTypeMapComponent final
{
	GENERATED_BODY()
	
	mutable robin_hood::unordered_flat_map<FFlecsScriptStructComponent, FFlecsEntityHandle> ScriptStructMap;
	mutable robin_hood::unordered_flat_map<FFlecsScriptClassComponent, FFlecsEntityHandle> ScriptClassMap;
}; // struct FFlecsTypeMapComponent

REGISTER_FLECS_PROPERTIES(FFlecsTypeMapComponent, { flecs::Sparse }, {});
