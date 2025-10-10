// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsCollectionWorldSubsystem.h"

#include "Engine/World.h"
#include "Misc/AutomationTest.h"

#include "Logs/FlecsCategories.h"

#include "Worlds/FlecsWorldSubsystem.h"
#include "FlecsCollectionTypes.h"
#include "FlecsCollectionDataAsset.h"
#include "FlecsCollectionInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsCollectionWorldSubsystem)

void UFlecsCollectionWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Automation Testing specific initialization flow
#if WITH_AUTOMATION_TESTS
	if (GIsAutomationTesting)
	{
		const TSolidNotNull<const UFlecsWorldSubsystem*> WorldSubsystem = GetWorld()->GetSubsystemChecked<UFlecsWorldSubsystem>();
		
		if (WorldSubsystem->HasValidFlecsWorld())
		{
			const TSolidNotNull<UFlecsWorld*> FlecsWorld = GetFlecsWorld();

			FlecsWorld->RegisterComponentType<FFlecsCollectionPrefabTag>();
			FlecsWorld->RegisterComponentType<FFlecsCollectionReferenceComponent>();
			FlecsWorld->RegisterComponentType<FFlecsCollectionSlotTag>();

			CollectionScopeEntity = FlecsWorld->CreateEntity("CollectionScope")
				.Add(flecs::Module);
	
			UE_LOG(LogFlecsWorld, Verbose, TEXT("UCollectionsModule registered"));
		}
		else
		{
			Unreal::Flecs::GOnFlecsWorldInitialized.AddLambda([this](const TSolidNotNull<const UFlecsWorld*> InWorld)
			{
				InWorld->RegisterComponentType<FFlecsCollectionPrefabTag>();
				InWorld->RegisterComponentType<FFlecsCollectionReferenceComponent>();
				InWorld->RegisterComponentType<FFlecsCollectionSlotTag>();

				CollectionScopeEntity = InWorld->CreateEntity("CollectionScope")
					.Add(flecs::Module);
	
				UE_LOG(LogFlecsWorld, Verbose, TEXT("UCollectionsModule registered"));
			});
		}
		
		return;
	}
#endif // WITH_AUTOMATION_TESTS

	const TSolidNotNull<const UFlecsWorld*> FlecsWorld = GetFlecsWorldChecked();

	FlecsWorld->RegisterComponentType<FFlecsCollectionPrefabTag>();
	FlecsWorld->RegisterComponentType<FFlecsCollectionReferenceComponent>();
	FlecsWorld->RegisterComponentType<FFlecsCollectionSlotTag>();

	CollectionScopeEntity = FlecsWorld->CreateEntity("CollectionScope")
		.Add(flecs::Module);
	
	UE_LOG(LogFlecsWorld, Verbose, TEXT("UCollectionsModule registered"));
}

void UFlecsCollectionWorldSubsystem::Deinitialize()
{
	if (MAYBE_UNUSED const UFlecsWorld* FlecsWorld = GetFlecsWorld())
	{
		if LIKELY_IF(CollectionScopeEntity.IsValid())
		{
			CollectionScopeEntity.Destroy();
		}
	}
		
	Super::Deinitialize();
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::RegisterCollectionAsset(const UFlecsCollectionDataAsset* InAsset)
{
	solid_check(InAsset);

	const FFlecsCollectionId Id = FFlecsCollectionId(InAsset->GetFName());
	if (const FFlecsEntityHandle* Existing = RegisteredCollections.Find(Id))
	{
		return *Existing;
	}

	const FFlecsEntityHandle Prefab = CreatePrefabEntity(InAsset->GetName(), InAsset->Record);
	RegisteredCollections.Add(Id, Prefab);

	for (const FFlecsCollectionReference& Reference : InAsset->Collections)
	{
		const FFlecsEntityHandle Resolved = ResolveCollectionReference(Reference);
		
		if UNLIKELY_IF(!Resolved.IsValid())
		{
			UE_LOGFMT(LogFlecsWorld, Error,
				"UFlecsCollectionWorldSubsystem::RegisterCollectionAsset: Failed to resolve collection reference on asset '{Asset}'",
				InAsset->GetName());
			continue;
		}

		Prefab.AddPair(flecs::IsA, Resolved);
	}
	
	ExpandChildCollectionReferences(Prefab);

	return Prefab;
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::RegisterCollectionDefinition(
	const FName& InName, 
	const FFlecsCollectionDefinition& InDefinition)
{
	const FFlecsCollectionId Id = FFlecsCollectionId(InName);
	if (const FFlecsEntityHandle* Existing = RegisteredCollections.Find(Id))
	{
		return *Existing;
	}
	
	const FFlecsEntityHandle Prefab = CreatePrefabEntity(Id.NameId.ToString(), InDefinition.Record);
	RegisteredCollections.Add(Id, Prefab);
	
	for (const FFlecsCollectionReference& Reference : InDefinition.Collections)
	{
		const FFlecsEntityHandle ResolvedEntityHandle = ResolveCollectionReference(Reference);
		
		if UNLIKELY_IF(!ResolvedEntityHandle.IsValid())
		{
			UE_LOGFMT(LogFlecsWorld, Error,
				"UFlecsCollectionWorldSubsystem::RegisterCollectionDefinition: Failed to resolve collection reference on definition '{NameId}'",
				Id.NameId.ToString());
			continue;
		}

		Prefab.AddPair(flecs::IsA, ResolvedEntityHandle);
	}
	
	ExpandChildCollectionReferences(Prefab);
	
	return Prefab;
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::RegisterCollectionClass(const TSolidNotNull<UClass*> InClass,
	const FFlecsCollectionBuilder& InBuilder)
{
	const FFlecsCollectionId Id = FFlecsCollectionId(InClass->GetFName());
	if (const FFlecsEntityHandle* Existing = RegisteredCollections.Find(Id))
	{
		return *Existing;
	}
	
	const FFlecsEntityHandle Prefab
		= CreatePrefabEntity(InClass->GetName(), InBuilder.GetCollectionDefinition().Record);
	
	RegisteredCollections.Add(Id, Prefab);
	
	for (const FFlecsCollectionReference& Reference : InBuilder.GetCollectionDefinition().Collections)
	{
		const FFlecsEntityHandle ResolvedEntityHandle = ResolveCollectionReference(Reference);
		
		if UNLIKELY_IF(!ResolvedEntityHandle.IsValid())
		{
			UE_LOGFMT(LogFlecsWorld, Error,
				"UFlecsCollectionWorldSubsystem::RegisterCollectionClass: Failed to resolve collection reference on class '{Class}'",
				InClass->GetName());
			continue;
		}

		Prefab.AddPair(flecs::IsA, ResolvedEntityHandle);
	}
	
	ExpandChildCollectionReferences(Prefab);
	
	return Prefab;
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::RegisterCollectionClass(
	const TSolidNotNull<TScriptInterface<IFlecsCollectionInterface>*> InInterfaceObject)
{
	FFlecsCollectionDefinition Definition;
	
	FFlecsCollectionBuilder CollectionBuilder = FFlecsCollectionBuilder::Create(Definition);
	
	// @TODO: how safe is this to be called on a CDO?
	InInterfaceObject->GetInterface()->BuildCollection(CollectionBuilder);

	return RegisterCollectionClass(InInterfaceObject->GetObject()->GetClass(), CollectionBuilder);
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::GetPrefabByAsset(const UFlecsCollectionDataAsset* Asset) const
{
	solid_check(Asset);
	
	const FFlecsCollectionId Id = FFlecsCollectionId(Asset->GetFName());
	return GetPrefabById(Id);
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::GetPrefabById(const FFlecsCollectionId& Id) const
{
	if LIKELY_IF(const FFlecsEntityHandle* Found = RegisteredCollections.Find(Id))
	{
		return *Found;
	}
	
	return FFlecsEntityHandle();
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::GetPrefabByClass(const TSubclassOf<UObject> InClass) const
{
	solid_check(InClass);
	
	const FFlecsCollectionId Id = FFlecsCollectionId(InClass->GetFName());
	return GetPrefabById(Id);
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::EnsurePrefabShell(const FFlecsCollectionId& Id)
{
	if (const FFlecsEntityHandle* Existing = RegisteredCollections.Find(Id))
	{
		return *Existing;
	}
	
	const TSolidNotNull<const UFlecsWorld*> FlecsWorld = GetFlecsWorldChecked();
	
	const FFlecsEntityHandle Prefab = FlecsWorld->CreateEntity(Id.NameId.ToString())
		.SetParent(CollectionScopeEntity)
		.Add(flecs::Prefab)
		.Add<FFlecsCollectionPrefabTag>();
	
	RegisteredCollections.Add(Id, Prefab);
	
	return Prefab;
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::ResolveCollectionReference(const FFlecsCollectionReference& Reference)
{
	switch (Reference.Mode)
	{
		case EFlecsCollectionReferenceMode::Asset:
			{
				if LIKELY_IF(ensureMsgf(IsValid(Reference.Asset),
					TEXT("UFlecsCollectionWorldSubsystem::ResolveReference: Asset is Invalid")))
				{
					return RegisterCollectionAsset(Reference.Asset);
				}

				return FFlecsEntityHandle();
			}
		
		case EFlecsCollectionReferenceMode::Id:
			{
				if LIKELY_IF(const FFlecsEntityHandle* Found = RegisteredCollections.Find(Reference.Id))
				{
					return *Found;
				}
				
				UE_LOGFMT(LogFlecsWorld, Error,
					"UFlecsCollectionWorldSubsystem::ResolveReference: Id '{NameId}' not registered",
					Reference.Id.NameId);
				
				return FFlecsEntityHandle();
			}
		
		case EFlecsCollectionReferenceMode::UClass:
			{
				if LIKELY_IF(ensureMsgf(Reference.Class,
					TEXT("UFlecsCollectionWorldSubsystem::ResolveReference: Class is null")))
				{
					const FFlecsEntityHandle ClassPrefab = GetPrefabByClass(Reference.Class);
					
					if LIKELY_IF(ensureMsgf(ClassPrefab.IsValid(),
						TEXT("UFlecsCollectionWorldSubsystem::ResolveReference: Class '{Class}' not registered"),
							*Reference.Class->GetName()))
					{
						return ClassPrefab;
					}
				}
				
				return FFlecsEntityHandle();
			}
	}

	return FFlecsEntityHandle();
}

void UFlecsCollectionWorldSubsystem::ExpandChildCollectionReferences(const FFlecsEntityHandle& InCollectionEntity)
{
	const TSolidNotNull<const UFlecsWorld*> FlecsWorld = GetFlecsWorldChecked();

	FlecsWorld->Defer([this, FlecsWorld, &InCollectionEntity]()
	{
		FlecsWorld->World.query_builder<FFlecsCollectionReferenceComponent*>() // 0 (FFlecsCollectionReferenceComponent)
			.with(flecs::ChildOf, InCollectionEntity) // 1
			.with<FFlecsCollectionSlotTag>().optional() // 2
			.each([&](flecs::iter& Iter, size_t Index, FFlecsCollectionReferenceComponent* ReferenceComponent) // 3
			{
				const FFlecsEntityHandle ChildEntityHandle = Iter.entity(Index);

				for (const FFlecsCollectionReference& CollectionReference : ReferenceComponent->Collections)
				{
					if (ReferenceComponent)
					{
						const FFlecsEntityHandle Resolved = ResolveCollectionReference(CollectionReference);
					
						if UNLIKELY_IF(!Resolved.IsValid())
						{
							UE_LOGFMT(LogFlecsWorld, Error,
								"UFlecsCollectionWorldSubsystem::ExpandChildCollectionReferences: Failed to resolve collection reference on entity '{Entity}'",
								ChildEntityHandle.GetPath());
							return;
						}

						ChildEntityHandle.AddPair(flecs::IsA, Resolved);
					}
				}

				if (Iter.is_set(2))
				{
					ChildEntityHandle.MarkSlot();
					ChildEntityHandle.Remove<FFlecsCollectionSlotTag>();
				}

				ExpandChildCollectionReferences(ChildEntityHandle);
			});
	});
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::CreatePrefabEntity(const FString& Name, const FFlecsEntityRecord& Record) const
{
	const TSolidNotNull<const UFlecsWorld*> FlecsWorld = GetFlecsWorldChecked();
	
	const FFlecsEntityHandle Prefab = FlecsWorld->CreatePrefabWithRecord(Record, Name)
		.Add<FFlecsCollectionPrefabTag>();

	return Prefab;
}

FFlecsEntityHandle UFlecsCollectionWorldSubsystem::CreatePrefabEntity(const TSolidNotNull<UClass*> InClass,
	const FFlecsEntityRecord& Record) const
{
	const TSolidNotNull<const UFlecsWorld*> FlecsWorld = GetFlecsWorldChecked();
	
	const FFlecsEntityHandle Prefab = FlecsWorld->CreatePrefabWithRecord(Record, InClass)
		.Add<FFlecsCollectionPrefabTag>();

	return Prefab;
}

bool UFlecsCollectionWorldSubsystem::ClassImplementsCollectionInterface(const TSolidNotNull<const UClass*> InClass) const
{
	return InClass->ImplementsInterface(UFlecsCollectionInterface::StaticClass());
}
