#include "GOAT.h"
#include "Dive.h"
#include "Ball.h"
#include "Utility.h"
#include "Characters/PlayerCharacter.h"


UDive::UDive() : Super()
{
    GUIName		    = "Dive";
    StartupTime     = 0.f;
    AnimationTime   = 0.75f;
}


void UDive::Activate (APlayerCharacter* character, ABall* ball)
{
    Super::Activate (character, ball);

    // Models are rotated strangly so we must use the right vector if there's no input.
    const auto inputDir = character->GetMovementInput();

    // Move forward if there is no input.
    if (inputDir.IsNearlyZero())
    {
        DiveDirection = character->GetActorRightVector();
    }

    // Choose a direction from the characters input.
    else
    {
        // We need absolutes for each input direction.
        const auto x = FMath::Abs (inputDir.X);
        const auto y = FMath::Abs (inputDir.Y);

        // We need to lock the direction to up, down, left or right.
        DiveDirection = x > y ? 
            FVector (inputDir.X, 0.f, 0.f).GetSafeNormal() :
            FVector (0.f, inputDir.Y, 0.f).GetSafeNormal();
    }
}


void UDive::Continue (APlayerCharacter* character, ABall* ball, float deltaTime)
{
    // Let the base class handle timings.
    Super::Continue (character, ball, deltaTime);

    // We need to gradually decrease speed when exiting the roll.
    const auto speed = FMath::Lerp (
        DiveSpeed, 
        ExitDiveSpeed, 
        FMath::Max ((RunningTime - DiveDuration) / (AnimationTime - DiveDuration), 0.f)
    );

    // We need to track the characters initial position and teleport if we touch the ball.
    const auto initialPosition  = character->GetActorLocation();
    const auto distance         = speed * deltaTime;
    const auto movement         = DiveDirection * distance;
    auto hitResult              = FHitResult { };

    // Check if we hit an object when moving forward.
    character->SetActorLocation (initialPosition + movement, true, &hitResult, ETeleportType::TeleportPhysics);
    if (hitResult.bBlockingHit)
    {
        // Only continue moving forward if we hit the ball.
        auto ballActor = dynamic_cast<ABall*> (hitResult.Actor.Get());
        if (ballActor)
        {
            // Find out how much extra to move forward.
            const auto adjustedPosition = character->GetActorLocation();
            const auto extraDistance    = distance - hitResult.Distance;

            // Add force to the ball.
            const auto velocityDirection    = ballActor->GetPhysicsComponent()->GetPhysicsLinearVelocity().GetSafeNormal();
            const auto velocityRedirect     = velocityDirection.IsNearlyZero() ? 
                (ballActor->GetActorLocation() - adjustedPosition).GetSafeNormal() :
                FMath::GetReflectionVector (velocityDirection, hitResult.ImpactNormal);

            ballActor->HeadTowards (velocityRedirect, CollisionForce);
            character->SetActorLocation (adjustedPosition + DiveDirection * extraDistance, 
                false, nullptr, ETeleportType::TeleportPhysics);
        }

        else if (RunningTime <= DiveDuration)
        {
            RunningTime = DiveDuration;
        }
    }


    if (RunningTime <= DiveDuration)
    {
        character->SetMovementVector (movement);
    }
    
    else
    {
        character->SetMovementVector (FVector::ZeroVector);
    }
}