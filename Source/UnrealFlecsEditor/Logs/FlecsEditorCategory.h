// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Unlog/Unlog.h"

UNLOG_CATEGORY(LogFlecsEditor);

using FFlecsEditorLog
	= TUnlog<>::WithCategory<LogFlecsEditor>::WithTargets<Target::Default, Target::MessageLog>;
