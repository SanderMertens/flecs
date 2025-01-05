// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Unlog/Unlog.h"

UNLOG_CATEGORY(LogFlecsEditor);

using FFlecsEditorLog
	= TUnlog<>::WithCategory<LogFlecsEditor>::WithTargets<Target::Default, Target::MessageLog>;
