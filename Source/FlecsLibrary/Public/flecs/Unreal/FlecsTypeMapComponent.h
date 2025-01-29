// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsScriptStructComponent.h"
#include "Standard/robin_hood.h"

struct FFlecsTypeMapComponent final
{
	mutable robin_hood::unordered_flat_map<FFlecsScriptStructComponent, flecs::entity> ScriptStructMap;
}; // struct FFlecsTypeMapComponent
