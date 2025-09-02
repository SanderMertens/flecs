// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsEntityRecord.h"

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
							if (PairValueType == EFlecsValuePairType::First)
							{
								InEntityHandle.SetPairFirst(First.PairScriptStruct.GetScriptStruct(),
								                            First.PairScriptStruct.GetMemory(),
								                            Second.PairScriptStruct.GetScriptStruct());
							}
							else
							{
								if UNLIKELY_IF(!ensure(First.PairScriptStruct.GetScriptStruct()->GetStructureSize() <= 1))
								{
									return;
								}
							}
						}
						break;
					case EFlecsPairNodeType::EntityHandle:
						{
							InEntityHandle.AddPair(First.PairScriptStruct.GetScriptStruct(),
							                       Second.EntityHandle);
						}
						break;
					case EFlecsPairNodeType::FGameplayTag:
						{
							InEntityHandle.AddPair(First.PairScriptStruct.GetScriptStruct(),
							                       Second.GameplayTag);
						}
						break;
				}
			}
			break;
		case EFlecsPairNodeType::EntityHandle:
			{
				switch (Second.PairNodeType)
				{
					case EFlecsPairNodeType::ScriptStruct:
						{
							InEntityHandle.SetPairSecond(First.EntityHandle,
							                             Second.PairScriptStruct.GetScriptStruct(),
							                             Second.PairScriptStruct.GetMemory());
						}
						break;
					case EFlecsPairNodeType::EntityHandle:
						{
							InEntityHandle.AddPair(First.EntityHandle, Second.EntityHandle);
						}
						break;
					case EFlecsPairNodeType::FGameplayTag:
						{
							InEntityHandle.AddPair(First.EntityHandle, Second.GameplayTag);
						}
						break;
				}
			}
			break;
		case EFlecsPairNodeType::FGameplayTag:
			{
				switch (Second.PairNodeType)
				{
					case EFlecsPairNodeType::ScriptStruct:
						{
							InEntityHandle.SetPairSecond(First.GameplayTag,
							                             Second.PairScriptStruct.GetScriptStruct(),
							                             Second.PairScriptStruct.GetMemory());
						}
						break;
					case EFlecsPairNodeType::EntityHandle:
						{
							InEntityHandle.AddPair(First.GameplayTag, Second.EntityHandle);
						}
						break;
					case EFlecsPairNodeType::FGameplayTag:
						{
							InEntityHandle.AddPair(First.GameplayTag, Second.GameplayTag);
						}
						break;
				}
			}
			break;
	}
}

void FFlecsRecordSubEntity::ApplyRecordToEntity(const FFlecsEntityHandle& InEntityHandle) const
{
	solid_checkf(InEntityHandle.IsValid(), TEXT("Entity Handle is not valid"));

	if (!Name.IsEmpty() && !InEntityHandle.HasName())
	{
		InEntityHandle.SetName(Name);
	}

	for (const auto&
	     [NodeType, ScriptStruct, ScriptEnum,
		     EntityHandle, GameplayTag, Pair] : Components)
	{
		switch (NodeType)
		{
			case EFlecsComponentNodeType::ScriptStruct:
				{
					InEntityHandle.Set(ScriptStruct);
				}
				break;
			case EFlecsComponentNodeType::ScriptEnum:
				{
					InEntityHandle.Add(ScriptEnum.Class, ScriptEnum.Value);
				}
			case EFlecsComponentNodeType::EntityHandle:
				{
					InEntityHandle.Add(EntityHandle);
				}
				break;
			case EFlecsComponentNodeType::FGameplayTag:
				{
					InEntityHandle.Add(GameplayTag);	
				}
				break;
			case EFlecsComponentNodeType::Pair:
				{
					Pair.AddToEntity(InEntityHandle);
				}
				break;
		}
	}
}

void FFlecsEntityRecord::ApplyRecordToEntity(const FFlecsEntityHandle& InEntityHandle) const
{
	solid_checkf(InEntityHandle.IsValid(), TEXT("Entity Handle is not valid"));

	for (const auto& [NodeType, ScriptStruct, ScriptEnum,
		     EntityHandle, GameplayTag, Pair] : Components)
	{
		switch (NodeType)
		{
			case EFlecsComponentNodeType::ScriptStruct:
				{
					InEntityHandle.Set(ScriptStruct);
				}
				break;
			case EFlecsComponentNodeType::ScriptEnum:
				{
					InEntityHandle.Add(ScriptEnum.Class, ScriptEnum.Value);
				}
			case EFlecsComponentNodeType::EntityHandle:
				{
					InEntityHandle.Add(EntityHandle);
				}
				break;
			case EFlecsComponentNodeType::FGameplayTag:
				{
					InEntityHandle.Add(GameplayTag);	
				}
				break;
			case EFlecsComponentNodeType::Pair:
				{
					Pair.AddToEntity(InEntityHandle);
				}
				break;
		}
	}

	for (const FFlecsRecordSubEntity& SubEntity : SubEntities)
	{
		FFlecsEntityHandle NewEntityHandle = InEntityHandle
		                                     .GetEntity().world().entity(Unreal::Flecs::ToCString(SubEntity.Name));
			
		NewEntityHandle.SetParent(InEntityHandle);
			
		SubEntity.ApplyRecordToEntity(NewEntityHandle);
		InEntityHandle.Add(NewEntityHandle);
	}
}
