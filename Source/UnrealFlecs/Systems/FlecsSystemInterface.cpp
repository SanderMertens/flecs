// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsSystemInterface.h"
#include "Logs/FlecsCategories.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsSystemInterface)

FString IFlecsSystemInterface::GetName() const
{
	UN_LOGF(LogFlecsSystem, Warning, "not implemented for %s",
		*_getUObject()->GetClass()->GetName());
	return _getUObject()->GetClass()->GetName();
}
