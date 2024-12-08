// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlecsTranslationSettingTraits.generated.h"

UENUM(BlueprintType)
enum class EFlecsTranslationSetting : uint8
{
	ToUnreal = 0,
	ToFlecs,
	ToBoth
}; // enum class ETranslationSetting
