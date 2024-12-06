

#pragma once

#include "CoreMinimal.h"
#include "Standard/Hashing.h"
#include "FlecsGlobalTransformComponent.generated.h"

USTRUCT(BlueprintType)
struct UNREALFLECS_API FFlecsGlobalTransformComponent
{
	GENERATED_BODY()

	FORCEINLINE friend NO_DISCARD uint32 GetTypeHash(const FFlecsGlobalTransformComponent& InComponent)
	{
		return GetTypeHash(InComponent.Transform);
	}

	FORCEINLINE FFlecsGlobalTransformComponent() = default;

	FORCEINLINE FFlecsGlobalTransformComponent(const FTransform& InTransform)
		: Transform(InTransform)
	{
	}

	FORCEINLINE NO_DISCARD FTransform& GetTransform() { return Transform; }
	FORCEINLINE NO_DISCARD const FTransform& GetTransform() const { return Transform; }
	
	FORCEINLINE operator FTransform&() { return GetTransform(); }
	FORCEINLINE operator const FTransform&() const { return GetTransform(); }

	FORCEINLINE FFlecsGlobalTransformComponent& operator=(const FTransform& InTransform)
	{
		Transform = InTransform;
		return *this;
	}

	FORCEINLINE FFlecsGlobalTransformComponent& operator=(const FFlecsGlobalTransformComponent& InComponent)
	{
		Transform = InComponent.Transform;
		return *this;
	}

	FORCEINLINE bool operator==(const FFlecsGlobalTransformComponent& InComponent) const
	{
		return Transform.Equals(InComponent.Transform);
	}

	FORCEINLINE bool operator!=(const FFlecsGlobalTransformComponent& InComponent) const
	{
		return !(*this == InComponent);
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flecs")
	FTransform Transform;
	
}; // struct FFlecsGlobalTransformComponent

DEFINE_STD_HASH(FFlecsGlobalTransformComponent)
