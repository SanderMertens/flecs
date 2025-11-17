// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "FlecsGameFrameworkVersioningTypes.h"

#include "Serialization/CustomVersion.h"
#include "Actor/FlecsEntityActorComponent.h"

IMPLEMENT_SOLID_ASSET_VERSION(UFlecsEntityActorComponent, 0xA9464252, 0x37464046, 0xBDFF76B4, 0xEBBDA15A, "FlecsEntityActorComponentVersion");

REGISTER_ASSET_MIGRATION_STEP(UFlecsEntityActorComponent, SwitchedEntityInitializationMethodToEntityInitializationType,
	{
		const bool bChanged = !(Self->EntityRecord_DEPRECATED == Archetype->EntityRecord_DEPRECATED);
		if (bChanged)
		{
			Self->EntityInitializer.Type = EFlecsEntityInitializationType::Record;
			Self->EntityInitializer.Record = Self->EntityRecord_DEPRECATED;
		}
	})