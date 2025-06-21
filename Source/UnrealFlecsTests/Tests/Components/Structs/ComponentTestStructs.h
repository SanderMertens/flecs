// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ComponentTestStructs.generated.h"

struct FTestComponent_RegisterComponentTest
{
	int32 Value;
}; // struct FTestComponent

USTRUCT()
struct FUStructTestComponent_RegisterComponentTest
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Value;
}; // struct FUStructTestComponent

USTRUCT()
struct FUStructTestComponent_Inherited : public FUStructTestComponent_RegisterComponentTest
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Value2;
}; // struct FUStructTestComponent_Inherited

USTRUCT()
struct FUStructTestComponent_TagUSTRUCT
{
	GENERATED_BODY()
}; // struct FUStructTestComponent_TagUSTRUCT

USTRUCT()
struct FUStructTestComponent_NonTagUSTRUCT
{
	GENERATED_BODY()

	UPROPERTY()
	bool bTest = false;
}; // struct FUStructTestComponent_NonTagUSTRUCT

USTRUCT()
struct alignas(16) FUStructTestComponent_CustomAlignedUSTRUCT_SixteenBytes
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Value1;

	UPROPERTY()
	int32 Value2;

}; // struct FUStructTestComponent_CustomAlignedUSTRUCT_SixteenBytes

USTRUCT()
struct alignas(32) FUStructTestComponent_CustomAlignedUSTRUCT_ThirtyTwoBytes
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Value1;

	UPROPERTY()
	int32 Value2;

	UPROPERTY()
	int32 Value3;

	UPROPERTY()
	int32 Value4;

}; // struct FUStructTestComponent_CustomAlignedUSTRUCT_ThirtyTwoBytes

USTRUCT()
struct alignas(64) FUStructTestComponent_CustomAlignedUSTRUCT_SixtyFourBytes
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Value1;

	UPROPERTY()
	int32 Value2;

	UPROPERTY()
	int32 Value3;

}; // struct FUStructTestComponent_CustomAlignedUSTRUCT_SixtyFourBytes

USTRUCT()
struct FUSTRUCTPairTestComponent
{
	GENERATED_BODY()

public:
}; // struct FUSTRUCTPairTestComponent

USTRUCT()
struct FUSTRUCTPairTestComponent_Second
{
	GENERATED_BODY()
}; // struct FUSTRUCTPairTestComponent_Second

USTRUCT()
struct FUSTRUCTPairTestComponent_Data
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Value = 0;

	UPROPERTY()
	float FloatValue = 0.0f;

	UPROPERTY()
	FString StringValue;
	
}; // struct FUSTRUCTPairTestComponent_Data



