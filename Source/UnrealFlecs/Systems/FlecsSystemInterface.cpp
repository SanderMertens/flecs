// Elie Wiese-Namir © 2024. All Rights Reserved.

#include "FlecsSystemInterface.h"
#include "Logs/FlecsCategories.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsSystemInterface)

FString IFlecsSystemInterface::GetName() const
{
	UN_LOGF(LogFlecsSystem, Warning, "not implemented for %s",
		*_getUObject()->GetClass()->GetName());
	return _getUObject()->GetClass()->GetName();
}
