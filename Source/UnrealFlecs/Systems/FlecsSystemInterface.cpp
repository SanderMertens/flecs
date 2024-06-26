// Fill out your copyright notice in the Description page of Project Settings.

#include "FlecsSystemInterface.h"
#include "Unlog/Unlog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlecsSystemInterface)

UNLOG_CATEGORY(LogFlecsSystemInterface);

FString IFlecsSystemInterface::GetName() const
{
	UN_LOGF(LogFlecsSystemInterface, Error,
		"not implemented for %s",
		*_getUObject()->GetClass()->GetName());
	return _getUObject()->GetClass()->GetName();
}
