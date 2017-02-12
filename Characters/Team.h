#pragma once

#include "Team.generated.h"


UENUM(BlueprintType)
enum class ETeam : uint8
{
    None    UMETA (DisplayName = "None"),
    Red     UMETA (DisplayName = "Red Team"),
    Blue    UMETA (DisplayName = "Blue Team")
};

FString EnumAsString (ETeam value);