// Elie Wiese-Namir © 2025. All Rights Reserved.

#include "flecs/Unreal/FlecsTypeRegisteredDelegate.h"

FFlecsTypeRegisteredDelegate& FlecsLibrary::GetTypeRegisteredDelegate()
{
	static FFlecsTypeRegisteredDelegate Delegate;
	return Delegate;
}
