#include "GOAT.h"
#include "BallGrab.h"
#include "Ball.h"
#include "Utility.h"
#include "Characters/PlayerCharacter.h"


UBallGrab::UBallGrab() : Super()
{
    GUIName         = "Ball Grab";
    BaseCooldown    = 5.f;
}


void UBallGrab::Initialise()
{
    Super::Initialise();
    
    // Ensure the grab range is clamped between 0 and 360 degrees.
    if (GrabAngle > 360.f)
    {
        GrabAngle = FMath::Fmod (GrabRange, 360.f);
    }
    
    if (GrabAngle < 0.f)
    {
        GrabAngle += 360.f;
    }
    
    // Normalise the grab range to -1 to 1.
    NormalisedGrabAngle = FMath::Cos (FMath::DegreesToRadians (GrabAngle));
    
    // Ensure the holding distance isn't larger than the range of the ability.
    HoldingDistance = FMath::Clamp (HoldingDistance, -GrabRange, GrabRange);
}


void UBallGrab::Activate (APlayerCharacter* character, ABall* ball)
{
    // Ensure the ball is both in range and we're facing the correct direction.
    if (CheckIsReachable (*character, *ball, GrabRange, NormalisedGrabAngle))
    {
        // Ensure the ball doesn't collide with the character and just place it at the same spot for now.
        ball->DisablePhysics();
        initialBallPosition = ball->GetActorLocation();
        
        // We need to set ourselves to being activated and reset the held time.
        Activated           = true;
        HasTriggered        = false;
        ShouldDeactivate    = false;
        RunningTime         = 0.f;
        CurrentHeldTime     = 0.f;
    }

    // Inform the character that the ability failed.
    else
    {
        /*GEngine->AddOnScreenDebugMessage (-1, 15.f, FColor::Blue, 
            FString::Printf (TEXT ("%s::Activate(): the ball isn't reachable."), *GUIName)
        );*/

        character->NotifyAbilityHasEnded (GetType());
    }
}


void UBallGrab::Continue (APlayerCharacter* character, ABall* ball, float deltaTime)
{
    // Ensure we set the ball position correctly.
    ball->SetActorLocation (CalculateHoldingPosition (*character, *ball));

    // We're going to grab the ball.
    if (RunningTime < StartupTime)
    {
        RunningTime += deltaTime;
    }

    // The user has let go of the grab button.
    else if (ShouldDeactivate)
    {
        Deactivate (character, ball);
        ball->SetActorLocation (CalculateHoldingPosition (*character, *ball));
    }

    // We're still allowed to hold the ball.
    else if (CurrentHeldTime < GrabTimer)
    {
        CurrentHeldTime += deltaTime;
    }

    // Ensure we don't go to throw the ball multiple times.
    else if (!HasTriggered)
    {
        ShouldDeactivate = true;
        RunningTime += deltaTime;
        Deactivate (character, ball);
    }

    // We haven't completed finished the animation.
    else if (RunningTime < AnimationTime)
    {
        RunningTime += deltaTime;
    }

    // We are finally free to move again.
    else
    {
        Stop (character, ball);
        
        // Ensure the ball can move again and is reset back to its original height.
        const auto ballPosition = ball->GetActorLocation();
        ball->SetActorLocation ({ ballPosition.X, ballPosition.Y, initialBallPosition.Z }, 
            false, nullptr, ETeleportType::TeleportPhysics);
        ball->EnablePhysics();

		const auto throwDirection = CalculatePreferredDirection (*character, *ball);
		ball->HeadTowards (throwDirection, ThrowingForce, 0.f);
    }
}


void UBallGrab::Deactivate (APlayerCharacter* character, ABall* ball)
{
    if (Activated)
    {        
        if (RunningTime < StartupTime)
        {
            ShouldDeactivate = true;
        }

        else if (ShouldDeactivate || CurrentHeldTime < GrabTimer)
        {
            character->SetState (ECharacterState::Throwing);

            HasTriggered        = true;
            ShouldDeactivate    = false;
            CurrentHeldTime     = GrabTimer;
        }
    }
}


void UBallGrab::Stop (APlayerCharacter* character, ABall* ball)
{
    Super::Stop (character, ball);
    CurrentHeldTime     = 0.f;
    ShouldDeactivate    = false;

    if (Activated)
    {
        ball->EnablePhysics();
    }
}


FVector UBallGrab::CalculatePreferredDirection (const APlayerCharacter& character, const ABall& ball) const
{
    return FirstNonZeroVector
    (
        [&] () { return character.MovementAsDirection(); },
        [&] () { return character.GetActorRightVector(); }
    );
}


FVector UBallGrab::CalculateHoldingPosition (const APlayerCharacter& character, const ABall& ball) const
{
    // Use the position of a bone if one has been set.
    const auto bonePosition = character.GetMesh()->GetBoneLocation (BoneToSetBallPositionTo);

    // Also move the ball forward slightly, keep in mind the meshes are rotated incorrectly.
    return bonePosition + character.GetActorRightVector() * HoldingDistance;
}