#include "GOAT.h"
#include "Pass.h"
#include "Ball.h"
#include "Utility.h"
#include "Characters/PlayerCharacter.h"


UPass::UPass() : Super()
{
    GUIName                 = "Pass";
    BaseCooldown            = 0.1f;
    KickingForceMultiplier  = 0.5f;
    BallRestitution         = 0.5f;
}


FVector UPass::CalculateDesiredDirection (const APlayerCharacter& character, const ABall& ball) const
{
    return FirstNonZeroVector
    (
        [&] () { return character.MovementAsDirection(); },
        [&] () { return DirectionTo (character, ball); },
        [&] () { return character.GetActorForwardVector(); }
    );
}