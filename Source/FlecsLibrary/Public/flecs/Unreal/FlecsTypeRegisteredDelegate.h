// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FFlecsTypeRegisteredDelegate, size_t /*entity_id*/);

namespace FlecsLibrary
{
	// Get the delegate that is broadcast when a new type is registered in Flecs CPP API
	FLECSLIBRARY_API FFlecsTypeRegisteredDelegate& GetTypeRegisteredDelegate();
}