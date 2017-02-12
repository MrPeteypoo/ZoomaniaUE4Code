#include "GOAT.h"
#include "KickingAbility.h"
#include "Ball.h"
#include "Utility.h"
#include "Characters/PlayerCharacter.h"
#include "Characters/Formulae.h"


UKickingAbility::UKickingAbility() : Super()
{
    GUIName = "UKickingAbility";
}


bool UKickingAbility::CanActivate (APlayerCharacter* character, ABall* ball, FString& reason) const
{
    // Activate if possible.
    if (CheckIsInRange (*character, *ball, MaximumRange))
    {
        return true;
    }

    // Inform the character that the ability failed.
    else
    {
        reason = "Needs to be closer to the ball.";
    }

    return false;
}


void UKickingAbility::Trigger (APlayerCharacter* character, ABall* ball)
{
    // Ensure the object is still in range.
    if (CheckIsInRange (*character, *ball, MaximumRange))
    {
        const auto direction = CalculateDesiredDirection (*character, *ball);
        const auto force = CalculateKickingForce (*character, KickingForceMultiplier);
        ball->HeadTowards (direction, force, BallRestitution);
    }
}