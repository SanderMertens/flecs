// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsEntityHandleTypes.h"

#include "Logging/StructuredLog.h"

#include "Logs/FlecsCategories.h"
#include "FlecsEntityHandle.h"
#include "Worlds/FlecsWorld.h"

DEFINE_FLECS_ENTITY_NET_SERIALIZE_FUNCTION(Unreal::Flecs::EmptyNetSerializeFunction,
	[](FFlecsEntityHandle& InEntity, TSolidNotNull<UFlecsWorld*> FlecsWorld, FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
	{
		if UNLIKELY_IF(!InEntity.IsValid())
		{
			UE_LOGFMT(LogFlecsEntity, Warning, "Trying to net serialize an invalid entity");
			bOutSuccess = true;
			return true;
		}
	    
		if (InEntity.HasName())
		{
			UE_LOGFMT(LogFlecsEntity, Warning,
				"Trying to net serialize entity {EntityId} with a name ({EntityName}) without a valid NetSerialize function",
				InEntity.GetFlecsId().ToString(), InEntity.GetName());
		}
		else
		{
			UE_LOGFMT(LogFlecsEntity, Warning,
			"Trying to net serialize entity {EntityId} without a valid NetSerialize function",
					 InEntity.GetFlecsId().ToString());
		}
	        
		bOutSuccess = true;
		return true;
	});

Unreal::Flecs::FEntityNetSerializeFunction* Unreal::Flecs::GNetSerializeFunctionPtr = &EmptyNetSerializeFunction; 