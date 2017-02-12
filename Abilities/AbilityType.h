#pragma once

#include "AbilityType.generated.h"


UENUM(BlueprintType)
enum class EAbilityType : uint8
{
    None        UMETA (DisplayName = "None"),
    Pass        UMETA (DisplayName = "Pass"),
    NormalKick  UMETA (DisplayName = "Normal Kick"),
    SpinKick    UMETA (DisplayName = "Spin Kick"),
    FastKick    UMETA (DisplayName = "Fast Kick"),
    BallGrab    UMETA (DisplayName = "Ball Grab"),
    Deflect     UMETA (DisplayName = "Deflect"),
    Dive        UMETA (DisplayName = "Dive")
};

FString EnumAsString (const EAbilityType value);