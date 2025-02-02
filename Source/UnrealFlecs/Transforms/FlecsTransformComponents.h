// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Collections/FlecsComponentCollectionObject.h"
#include "SolidMacros/Macros.h"
#include "Standard/Hashing.h"
#include "FlecsTransformComponents.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsLocationComponent
{
	GENERATED_BODY()
	
	FORCEINLINE friend NO_DISCARD uint32 GetTypeHash(const FFlecsLocationComponent& InLocationComponent)
	{
		return GetTypeHash(InLocationComponent.Location);
	}
	
public:
	FORCEINLINE FFlecsLocationComponent() = default;

	FORCEINLINE FFlecsLocationComponent(const FVector& InLocation) : Location(InLocation) {}
	FORCEINLINE FFlecsLocationComponent(const float InX, const float InY, const float InZ)
		: Location(InX, InY, InZ)
	{
	}

	FORCEINLINE NO_DISCARD bool operator==(const FFlecsLocationComponent& Other) const
	{
		return Location == Other.Location;
	}

	FORCEINLINE NO_DISCARD bool operator!=(const FFlecsLocationComponent& Other) const
	{
		return !(*this == Other);
	}

	FORCEINLINE void SetLocation(const FVector& InLocation) { Location = InLocation; }
	FORCEINLINE void SetLocation(const float InX, const float InY, const float InZ) { Location = FVector(InX, InY, InZ); }

	FORCEINLINE NO_DISCARD FVector GetLocation() const { return Location; }
	FORCEINLINE operator FVector() const { return GetLocation(); }
	FORCEINLINE FVector* operator->() { return &Location; }
	FORCEINLINE const FVector* operator->() const { return &Location; }
	FORCEINLINE FVector& operator*() { return Location; }
	FORCEINLINE const FVector& operator*() const { return Location; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Transform")
	FVector Location = FVector::ZeroVector;
	
}; // struct FFlecsLocationComponent

DEFINE_STD_HASH(FFlecsLocationComponent)

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsRotationComponent
{
	GENERATED_BODY()
	
	FORCEINLINE friend NO_DISCARD uint32 GetTypeHash(const FFlecsRotationComponent& InRotationComponent)
	{
		return GetTypeHash(InRotationComponent.Rotation.Quaternion());
	}
	
public:
	FORCEINLINE FFlecsRotationComponent() = default;

	FORCEINLINE FFlecsRotationComponent(const FRotator& InRotation) : Rotation(InRotation) {}
	FORCEINLINE FFlecsRotationComponent(const float InPitch, const float InYaw, const float InRoll)
		: Rotation(InPitch, InYaw, InRoll)
	{
	}

	FORCEINLINE NO_DISCARD bool operator==(const FFlecsRotationComponent& Other) const
	{
		return Rotation == Other.Rotation;
	}

	FORCEINLINE NO_DISCARD bool operator!=(const FFlecsRotationComponent& Other) const
	{
		return !(*this == Other);
	}

	FORCEINLINE void SetRotation(const FRotator& InRotation) { Rotation = InRotation; }
	FORCEINLINE void SetRotation(const float InPitch, const float InYaw, const float InRoll)
	{
		Rotation = FRotator(InPitch, InYaw, InRoll);
	}

	FORCEINLINE NO_DISCARD FRotator GetRotation() const { return Rotation; }
	FORCEINLINE operator FRotator() const { return GetRotation(); }
	FORCEINLINE FRotator* operator->() { return &Rotation; }
	FORCEINLINE const FRotator* operator->() const { return &Rotation; }
	FORCEINLINE FRotator& operator*() { return Rotation; }
	FORCEINLINE const FRotator& operator*() const { return Rotation; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Transform")
	FRotator Rotation = FRotator::ZeroRotator;
	
}; // struct FFlecsRotationComponent

DEFINE_STD_HASH(FFlecsRotationComponent)

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsScaleComponent
{
	GENERATED_BODY()
	
	FORCEINLINE friend NO_DISCARD uint32 GetTypeHash(const FFlecsScaleComponent& InScaleComponent)
	{
		return GetTypeHash(InScaleComponent.Scale);
	}
	
public:
	FORCEINLINE FFlecsScaleComponent() = default;

	FORCEINLINE FFlecsScaleComponent(const FVector& InScale) : Scale(InScale) {}
	FORCEINLINE FFlecsScaleComponent(const float InX, const float InY, const float InZ) : Scale(InX, InY, InZ) {}

	FORCEINLINE NO_DISCARD bool operator==(const FFlecsScaleComponent& Other) const
	{
		return Scale == Other.Scale;
	}

	FORCEINLINE NO_DISCARD bool operator!=(const FFlecsScaleComponent& Other) const
	{
		return !(*this == Other);
	}

	FORCEINLINE void SetScale(const FVector& InScale) { Scale = InScale; }
	FORCEINLINE void SetScale(const float InX, const float InY, const float InZ) { Scale = FVector(InX, InY, InZ); }

	FORCEINLINE NO_DISCARD FVector GetScale() const { return Scale; }
	FORCEINLINE operator FVector() const { return GetScale(); }
	FORCEINLINE FVector* operator->() { return &Scale; }
	FORCEINLINE const FVector* operator->() const { return &Scale; }
	FORCEINLINE FVector& operator*() { return Scale; }
	FORCEINLINE const FVector& operator*() const { return Scale; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Transform")
	FVector Scale = FVector::OneVector;
	
}; // struct FFlecsScaleComponent

DEFINE_STD_HASH(FFlecsScaleComponent)

UCLASS(BlueprintType, EditInlineNew)
class UNREALFLECS_API UFlecsTransform3dCollection : public UFlecsComponentCollectionObject
{
	GENERATED_BODY()

public:
	UFlecsTransform3dCollection(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Transform")
	FFlecsLocationComponent Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Transform")
	FFlecsRotationComponent Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs | Transform")
	FFlecsScaleComponent Scale;

	virtual void ApplyCollectionToEntity_Implementation(FFlecsEntityHandle& Entity) override final;
	
}; // class UFlecsTransformCollection

