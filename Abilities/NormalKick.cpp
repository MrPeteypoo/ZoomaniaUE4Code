#include "GOAT.h"
#include "NormalKick.h"
#include "Utility.h"
#include "Characters/PlayerCharacter.h"
#include "Environment/Goal.h"


UNormalKick::UNormalKick() : Super()
{
    GUIName                 = "Normal Kick";
    BaseCooldown            = 0.1f;
    KickingForceMultiplier  = 1.f;
}


FVector UNormalKick::CalculateDesiredDirection (const APlayerCharacter& character, const ABall& ball) const
{
    // We need to aim towards the goal with the input acting as an offset.
    const auto goal     = character.GetTargetGoal();
    const auto input    = character.MovementAsDirection();
    const auto toGoal   = goal->GetActorLocation() - character.GetActorLocation();

    // Move the target point based on the Y axis input.
    const auto aim = FVector { toGoal.X, toGoal.Y + input.Y, toGoal.Z }.GetSafeNormal();

    return aim;
}