// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FFlecsTypeRegisteredDelegate, size_t /*entity_id*/);

namespace FlecsLibrary
{
	// is broadcast when a new type is registered in Flecs CPP API (as well as runtime API in UnrealFlecs)
	FLECSLIBRARY_API FFlecsTypeRegisteredDelegate& GetTypeRegisteredDelegate();
}