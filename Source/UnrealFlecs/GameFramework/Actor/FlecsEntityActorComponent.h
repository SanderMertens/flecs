// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"

#include "SolidMacros.h"

#include "Entities/FlecsEntityHandle.h"
#include "Interfaces/FlecsEntityInterface.h"
#include "Entities/FlecsEntityRecord.h"
#include "Entities/FlecsEntityInitializationType.h"

#include "FlecsEntityActorComponent.generated.h"

UCLASS(BlueprintType, Blueprintable, ClassGroup=(Flecs),
	meta = (BlueprintSpawnableComponent, DisplayName = "Flecs Entity Actor Component"))
class UNREALFLECS_API UFlecsEntityActorComponent : public UActorComponent, public IFlecsEntityInterface
{
	GENERATED_BODY()

public:
	UFlecsEntityActorComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PostLoad() override;

	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	virtual void InitializeEntity();

	UFUNCTION(BlueprintCallable, Category = "Flecs | Entity")
	virtual void DestroyEntity(const bool bDestroyOwningActor = false);

	virtual void OnEntitySpawned(const FFlecsEntityHandle& InEntityHandle);

	UFUNCTION(BlueprintImplementableEvent, Category = "Flecs | Entity", meta = (DisplayName = "On Entity Spawned"))
	void BP_OnEntitySpawned(const FFlecsEntityHandle& InEntityHandle);

	FORCEINLINE virtual FFlecsEntityHandle GetEntityHandle() const override final
	{
		return EntityHandle;
	}

	// Currently unimplemented and will crash if called
	UFUNCTION()
	void SetEntityHandle(const FFlecsEntityHandle& InEntityHandle);

	UPROPERTY(Replicated)
	FFlecsEntityHandle EntityHandle = FFlecsEntityHandle::GetNullHandle();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//UPROPERTY(EditAnywhere, Category = "Flecs | Entity")
	//FString EntityName;

	UPROPERTY(meta=(DeprecatedProperty))
	FFlecsEntityRecord EntityRecord_DEPRECATED;

	UPROPERTY(EditAnywhere, Category = "Flecs | Entity")
	FFlecsEntityInitializationType EntityInitializer;

#if WITH_EDITOR

	virtual bool CanEditChange(const FProperty* InProperty) const override;

#endif // WITH_EDITOR

	UFUNCTION()
	void OnWorldCreated(UFlecsWorld* InWorld);

private:
	void CreateActorEntity(const TSolidNotNull<const UFlecsWorld*> InWorld);

	NO_DISCARD FFlecsEntityHandle InitializeComponentEntity(const TSolidNotNull<const UFlecsWorld*> InWorld) const;
	
	NO_DISCARD FFlecsEntityHandle InitializeUsingEntityRecord(const TSolidNotNull<const UFlecsWorld*> InWorld,
		const FFlecsEntityRecord& InRecord) const;
	
	NO_DISCARD FFlecsEntityHandle InitializeUsingCollection(const TSolidNotNull<const UFlecsWorld*> InWorld,
		const FFlecsCollectionInstancedReference& InCollectionRef) const;
	
}; // class UFlecsEntityActorComponent
