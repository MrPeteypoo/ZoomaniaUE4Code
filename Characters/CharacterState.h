#pragma once

#include "CharacterState.generated.h"


UENUM(BlueprintType)
enum class ECharacterState : uint8
{
    Idle            UMETA (DisplayName = "Idle"),
    Moving          UMETA (DisplayName = "Moving"),
    Stunned         UMETA (DisplayName = "Stunned"),
    UsingAbility    UMETA (DisplayName = "Using Ability"),
    Throwing        UMETA (DisplayName = "Throwing Ball")
};

FString EnumAsString (const ECharacterState value);