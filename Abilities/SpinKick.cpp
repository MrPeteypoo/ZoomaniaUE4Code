#include "GOAT.h"
#include "SpinKick.h"
#include "Ball.h"
#include "Utility.h"
#include "Characters/Formulae.h"
#include "Characters/PlayerCharacter.h"
#include "Environment/Goal.h"


USpinKick::USpinKick() : Super()
{
    GUIName = "Spin Kick";
    BaseCooldown = 2.f;
    KickingForceMultiplier = 0.8f;
}


void USpinKick::Trigger (APlayerCharacter* character, ABall* ball)
{
    // Ensure the object is still in range.
    if (CheckIsInRange (*character, *ball, MaximumRange))
    {
        // Set default values for the shot.
        const auto& input   = character->GetMovementInput();
        const auto goal     = character->GetTargetGoal();
        const auto curve    = character->GetCurveShotCurvature();
        const auto toGoal   = (goal->GetActorLocation() - character->GetActorLocation()).GetSafeNormal();
        auto force          = CalculateKickingForce (*character, KickingForceMultiplier);
        auto timeDelta      = 0.5f + input.X * 0.5f;
        auto curveDelta     = FMath::IsNearlyZero (curve) ? FMath::Rand() % 2 == 0 ? -0.25f : 0.25f : curve;

        // Finally shoot the ball.
        Shoot (*character, *ball, toGoal, force, timeDelta, curveDelta);
    }
}


void USpinKick::Shoot (APlayerCharacter& character, ABall& ball, 
    const FVector& direction, const float force, const float timeDelta, const float curveDelta)
{
    const auto travelTime   = FMath::Lerp (MinTravelTime, MaxTravelTime, timeDelta);
    const auto curvature    = FMath::Lerp (MinCurvature, MaxCurvature, FMath::Abs (curveDelta));
    const auto curveOffset  = curveDelta < 0.f ? -curvature : curvature;

    ball.SpinTowards (direction, force, BallRestitution, travelTime, curveOffset);
}