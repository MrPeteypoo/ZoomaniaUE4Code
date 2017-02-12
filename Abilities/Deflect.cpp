#include "GOAT.h"
#include "Deflect.h"
#include "Ball.h"
#include "Utility.h"
#include "Characters/PlayerCharacter.h"


UDeflect::UDeflect() : Super()
{
    GUIName = "Deflect";
    BaseCooldown = 3.f;
}


bool UDeflect::CanActivate (APlayerCharacter* character, ABall* ball, FString& reason) const
{
    // Activate if possible.
    if (CheckIsInRange (*character, *ball, DeflectionRange))
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


void UDeflect::Trigger (APlayerCharacter* character, ABall* ball)
{
    // Ensure the object is still in range.
    if (CheckIsInRange (*character, *ball, DeflectionRange))
    {
        ball->HeadTowards (CalculatePreferredDirection (*character, *ball), DeflectionForce, BallResitution);
    }
}


FVector UDeflect::CalculatePreferredDirection (const APlayerCharacter& character, const ABall& ball) const
{
    return FirstNonZeroVector 
    (
        [&] () { return DirectionTo (character, ball); },
        [&] () { return -ball.GetPhysicsComponent()->GetPhysicsLinearVelocity().GetSafeNormal(); },
        [&] () { return character.GetActorForwardVector(); }
    );
}