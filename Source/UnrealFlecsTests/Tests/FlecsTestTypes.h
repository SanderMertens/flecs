// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagsManager.h"
#include "Entities/FlecsDefaultEntityEngineSubsystem.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "UObject/Object.h"
#include "FlecsTestTypes.generated.h"

DECLARE_DEFAULT_ENTITY(TestEntityOption);
DECLARE_DEFAULT_ENTITY(TestEntityOption2WithTrait);

struct FFlecsTest_CPPStruct
{
}; // struct FFlecsTest_CPPStruct

struct FFlecsTest_CPPStructValue
{
	int32 Value;
}; // struct FFlecsTest_CPPStructWithNameAndValue

USTRUCT()
struct FFlecsTestStruct_Tag
{
	GENERATED_BODY()
}; // struct FFlecsTestStruct

USTRUCT(BlueprintType, BlueprintInternalUseOnly)
struct FFlecsTestStruct_Value
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs|Test")
	int32 Value = 0;
	
}; // struct FFlecsTestStructWithNameAndValue

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

USTRUCT()
struct FFlecsTestComponent_Inherited : public FFlecsTestStruct_Value
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Value2;
}; // struct FFlecsTestComponent_Inherited

USTRUCT()
struct FFlecsTestComponent_NonTagUSTRUCT_Inherited : public FFlecsTestComponent_Inherited
{
	GENERATED_BODY()
}; // struct FUSTRUCTTestComponent_NonTagUSTRUCT_Inherited

USTRUCT()
struct FFlecsTestStruct_WithPropertyTraits
{
	GENERATED_BODY()

public:
	
}; // struct FFlecsTestStruct_WithProperties

REGISTER_FLECS_COMPONENT(FFlecsTestStruct_WithPropertyTraits,
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponentHandle)
	{
		InComponentHandle.Add(flecs::Trait);
	});

USTRUCT()
struct FFlecsTestStruct_Toggleable
{
	GENERATED_BODY()

public:
	UPROPERTY()
	uint32 Value = 0;
}; // struct FFlecsTestStruct_Toggleable

REGISTER_FLECS_COMPONENT(FFlecsTestStruct_Toggleable,
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponentHandle)
	{
		InComponentHandle.Add(flecs::CanToggle);
	});

USTRUCT()
struct FFlecsTestStruct_PairIsTag
{
	GENERATED_BODY()
}; // struct FFlecsTestStruct_PairIsTag

REGISTER_FLECS_COMPONENT(FFlecsTestStruct_PairIsTag,
	[](flecs::world InWorld, const FFlecsComponentHandle& InComponentHandle)
	{
		InComponentHandle.Add(flecs::PairIsTag);
	});

enum class ETestEnum : uint8
{
	None,
	One,
	Two,
	Three
}; // enum class ETestEnum

UENUM()
enum class EFlecsTestEnum_UENUM : uint8
{
	None,
	One,
	Two,
	Three
}; // enum class EFlecsTestEnum_UENUM

UENUM()
enum class EFlecsTestEnum_SparseUENUM : uint8
{
	None,
	One,
	Two,
	Three,
	Five = 5,
	Ten = 10,
}; // enum class EFlecsTestEnum_SparseUENUM

USTRUCT()
struct FUStructTestComponent_NonTagUSTRUCT
{
	GENERATED_BODY()

	UPROPERTY()
	bool bTest = false;
	
}; // struct FUStructTestComponent_NonTagUSTRUCT

USTRUCT()
struct FUSTructTestComponent_AccidentalTag
{
	GENERATED_BODY()

	/*
	* If Registered as a static struct,
	* this will be registered as a tag as it is one byte and has no UPROPERTY saying otherwise.
	**/
	uint8 Test = 0;
	
}; // struct FUSTructTestComponent_AccidentalTag

USTRUCT()
struct FFlecsTestComponent_Vector
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Value;
	
}; // struct FFlecsTestComponent_Position

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

struct FFlecsTestNativeGameplayTags : public FGameplayTagNativeAdder
{
	static FFlecsTestNativeGameplayTags StaticInstance;
	
	FGameplayTag TestTag1;
	FGameplayTag TestTag2;
	FGameplayTag TestTag3;

	FGameplayTag TestSameSubTag1;
	FGameplayTag TestSameSubTag2;

	FGameplayTag TestSameSubGrandchildTag1;
	FGameplayTag TestSameSubGrandchildTag2;

	virtual void AddTags() override
	{
		UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
		TestTag1 = Manager.AddNativeGameplayTag(TEXT("Test.UnrealFlecs.Tag1"));
		TestTag2 = Manager.AddNativeGameplayTag(TEXT("Test.UnrealFlecs.Tag2"));
		TestTag3 = Manager.AddNativeGameplayTag(TEXT("Test.UnrealFlecs.Tag3"));

		TestSameSubTag1 = Manager.AddNativeGameplayTag(TEXT("Test.UnrealFlecs.Sub1.Tag1"));
		TestSameSubTag2 = Manager.AddNativeGameplayTag(TEXT("Test.UnrealFlecs.Sub2.Tag1"));

		TestSameSubGrandchildTag1 = Manager.AddNativeGameplayTag(TEXT("Test.UnrealFlecs.Sub1.Tag1"));
		TestSameSubGrandchildTag2 = Manager.AddNativeGameplayTag(TEXT("Test.UnrealFlecs.Sub1.Tag2"));
	}

	NO_DISCARD FORCEINLINE static const FFlecsTestNativeGameplayTags& Get()
	{
		return StaticInstance;
	}
	
}; // struct FFlecsTestNativeGameplayTags

UCLASS()
class UNREALFLECSTESTS_API UFlecsUObjectComponentTestObject : public UObject
{
	GENERATED_BODY()
}; // class UFlecsUObjectComponentTestObject





