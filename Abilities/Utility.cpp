#include "GOAT.h"
#include "Utility.h"


bool CheckIsInRange (const AActor& primary, const AActor& secondary, float range)
{
    const auto difference = primary.GetActorLocation() - secondary.GetActorLocation();
    
    // Compare the square magnitude for the sake of efficiency.
    return difference.SizeSquared() <= FMath::Square (range);
}


bool CheckIsReachable (const AActor& primary, const AActor& secondary, float range, float perpendicularity)
{
    // Firstly check if the actors are in range.
    const auto difference = secondary.GetActorLocation() - primary.GetActorLocation();
    const auto sizeSquared = difference.SizeSquared();

    if (sizeSquared > FMath::Square (range))
    {
        return false;
    }

    // Firstly we need to normalise the difference between the actor locations to receive the direction.
    const auto toSecondary = difference / FGenericPlatformMath::Sqrt (sizeSquared);

    // Next check the perpendicularity. Unfortunately characters are rotated incorrectly we need must use the right vector.
    const auto forward = primary.GetActorRightVector();

    return FVector::DotProduct (forward, toSecondary) >= perpendicularity;
}


bool CheckPerpendicularity (const AActor& primary, const AActor& secondary, float perpendicularity)
{
    const auto forward = primary.GetActorForwardVector();
    const auto toSecondary = (secondary.GetActorLocation() - primary.GetActorLocation()).GetSafeNormal();

    return FVector::DotProduct (forward, toSecondary) >= perpendicularity;
}


FVector DirectionTo (const AActor& primary, const AActor& secondary)
{
    const auto toSecondary = secondary.GetActorLocation() - primary.GetActorLocation();

    return FVector { toSecondary.X, toSecondary.Y, 0.f }.GetSafeNormal();
}