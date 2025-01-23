// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Entities/FlecsEntityRecord.h"
#include "FlecsSystemSettingsInfo.generated.h"

UENUM(BlueprintType)
enum class EFlecsSystemTimerKind : uint8
{
	None = 0,
	Interval = 1,
	Rate = 2,
}; // enum class EFlecsSystemTimerKind

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsSystemSettingsInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | System")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | System")
	FFlecsEntityRecord SystemRecord;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | System")
	FFlecsEntityHandle Kind;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | System")
	EFlecsSystemTimerKind TimerKind = EFlecsSystemTimerKind::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | System",
		meta = (ClampMin = 0.0f, EditCondition = "TimerKind == EFlecsSystemTimerKind::Interval"))
	double Interval = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | System",
		meta = (ClampMin = 0, EditCondition = "TimerKind == EFlecsSystemTimerKind::Rate"))
	int32 Rate = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | System")
	FFlecsEntityHandle TickSource;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | System")
	bool bImmediate = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | System")
	bool bMultiThreaded = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | System")
	int32 Priority = FLECS_DEFAULT_SYSTEM_PRIORITY;
	
}; // struct FFlecsSystemSettingsInfo