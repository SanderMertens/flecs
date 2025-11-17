// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsEntityRecord.h"

#include "UObject/PropertyIterator.h"
#include "Worlds/FlecsWorld.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsEntityRecord)

void FFlecsRecordPair::AddToEntity(const FFlecsEntityHandle& InEntityHandle) const
{
	switch (First.PairNodeType)
	{
		case EFlecsPairNodeType::ScriptStruct:
			{
				switch (Second.PairNodeType)
				{
					case EFlecsPairNodeType::ScriptStruct:
						{
							if (PairValueType == EFlecsValuePairType::None)
							{
								InEntityHandle.AddPair(First.PairScriptStruct.GetScriptStruct(),
													   Second.PairScriptStruct.GetScriptStruct());
							}
							else if (PairValueType == EFlecsValuePairType::First)
							{
								InEntityHandle.SetPair(First.PairScriptStruct.GetScriptStruct(),
													   First.PairScriptStruct.GetMemory(),
													   Second.PairScriptStruct.GetScriptStruct());
							}
							else if (PairValueType == EFlecsValuePairType::Second)
							{
								InEntityHandle.SetPair(First.PairScriptStruct.GetScriptStruct(),
													   Second.PairScriptStruct.GetMemory(),
													   Second.PairScriptStruct.GetScriptStruct());
							}
							break;
						}
					case EFlecsPairNodeType::EntityHandle:
						{
							InEntityHandle.AddPair(First.PairScriptStruct.GetScriptStruct(),
							                       Second.EntityHandle);
							break;
						}
					case EFlecsPairNodeType::FGameplayTag:
						{
							InEntityHandle.AddPair(First.PairScriptStruct.GetScriptStruct(),
							                       Second.GameplayTag);
							break;
						}
				}
				break;
			}
		case EFlecsPairNodeType::EntityHandle:
			{
				switch (Second.PairNodeType)
				{
					case EFlecsPairNodeType::ScriptStruct:
						{
							solid_checkf(PairValueType != EFlecsValuePairType::First,
											 TEXT("EFlecsValuePairType::First is not supported when the first pair node is an Entity Handle"));
							
							if (PairValueType == EFlecsValuePairType::None)
							{
								InEntityHandle.AddPair(First.EntityHandle,
													   Second.PairScriptStruct.GetScriptStruct());
							}
							else
							{
								InEntityHandle.SetPair(First.EntityHandle,
													Second.PairScriptStruct.GetMemory(),
														 Second.PairScriptStruct.GetScriptStruct());
							}

							break;
						}
					case EFlecsPairNodeType::EntityHandle:
						{
							InEntityHandle.AddPair(First.EntityHandle, Second.EntityHandle);

							break;
						}
					case EFlecsPairNodeType::FGameplayTag:
						{
							InEntityHandle.AddPair(First.EntityHandle, Second.GameplayTag);

							break;
						}
				}

				break;
			}
		case EFlecsPairNodeType::FGameplayTag:
			{
				switch (Second.PairNodeType)
				{
					case EFlecsPairNodeType::ScriptStruct:
						{
							solid_checkf(PairValueType != EFlecsValuePairType::First,
											 TEXT("EFlecsValuePairType::First is not supported when the first pair node is a Gameplay Tag"));
							
							if (PairValueType == EFlecsValuePairType::None)
							{
								InEntityHandle.AddPair(First.GameplayTag,
													   Second.PairScriptStruct.GetScriptStruct());
							}
							else if (PairValueType == EFlecsValuePairType::Second)
							{
								InEntityHandle.SetPair(First.GameplayTag,
													Second.PairScriptStruct.GetMemory(),
														 Second.PairScriptStruct.GetScriptStruct());
							}

							break;
						}
					case EFlecsPairNodeType::EntityHandle:
						{
							InEntityHandle.AddPair(First.GameplayTag, Second.EntityHandle);

							break;
						}
					case EFlecsPairNodeType::FGameplayTag:
						{
							InEntityHandle.AddPair(First.GameplayTag, Second.GameplayTag);

							break;
						}
				}

				break;
			}
	}
}

void FFlecsEntityRecord::ApplyRecordToEntity(const TSolidNotNull<const UFlecsWorld*> InFlecsWorld, const FFlecsEntityHandle& InEntityHandle) const
{
	solid_checkf(InEntityHandle.IsValid(), TEXT("Entity Handle is not valid"));

	for (const TInstancedStruct<FFlecsEntityRecordFragment>& Fragment : Fragments)
	{
		Fragment.Get<FFlecsEntityRecordFragment>().PreApplyRecordToEntity(InFlecsWorld, InEntityHandle);
	}

	//InFlecsWorld->Defer([this, &InEntityHandle, InFlecsWorld]()
	{
		for (const auto& [NodeType, ScriptStruct, ScriptEnum,
			 EntityHandle, GameplayTag, Pair] : Components)
		{
			switch (NodeType)
			{
				case EFlecsComponentNodeType::ScriptStruct:
					{
						// @TODO: this may be true in the case of a bool/byte member that isn't marked with UPROPERTY()
						if (ScriptStruct.GetScriptStruct()->GetStructureSize() <= 1 &&
							ScriptStruct.GetScriptStruct()->PropertyLink == nullptr)
						{
							// Empty struct, just add the tag
							InEntityHandle.Add(ScriptStruct.GetScriptStruct());
							break;
						}
							
						InEntityHandle.Set(ScriptStruct);

						break;
					}
				case EFlecsComponentNodeType::ScriptEnum:
					{
						InEntityHandle.Add(ScriptEnum.Class, ScriptEnum.Value);

						break;
					}
				case EFlecsComponentNodeType::EntityHandle:
					{
						InEntityHandle.Add(EntityHandle);

						break;
					}
				case EFlecsComponentNodeType::FGameplayTag:
					{
						InEntityHandle.Add(GameplayTag);

						break;
					}
				case EFlecsComponentNodeType::Pair:
					{
						Pair.AddToEntity(InEntityHandle);

						break;
					}
			}
		}
		
		for (const TInstancedStruct<FFlecsEntityRecord>& SubEntityRecord : SubEntities)
		{
			if UNLIKELY_IF(!ensure(SubEntityRecord.IsValid()))
			{
				continue;
			}
			
			FFlecsEntityHandle NewEntityHandle = InFlecsWorld->CreateEntity()
				.SetParent(InEntityHandle);
			
			SubEntityRecord.Get<FFlecsEntityRecord>().ApplyRecordToEntity(InFlecsWorld, NewEntityHandle);
			InEntityHandle.Add(NewEntityHandle);
		}
	}//);

	for (const TInstancedStruct<FFlecsEntityRecordFragment>& Fragment : Fragments)
	{
		Fragment.Get<FFlecsEntityRecordFragment>().PostApplyRecordToEntity(InFlecsWorld, InEntityHandle);
	}
}

void FFlecsNamedEntityRecordFragment::PreApplyRecordToEntity(const TSolidNotNull<const UFlecsWorld*> InFlecsWorld,
	const FFlecsEntityHandle& InEntityHandle) const
{
	InEntityHandle.SetName(Name);
}
