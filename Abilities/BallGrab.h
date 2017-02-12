#pragma once

#include "Ability.h"
#include "BallGrab.generated.h"

/** An ability where the character grabs hold of the ball and can throw it a desired direction. */
UCLASS(abstract, Blueprintable)
class GOAT_API UBallGrab : public UAbility
{
    GENERATED_BODY()
    
public:
    
    /** Sets the name and base cooldown of the ability. */
    UBallGrab();
    virtual ~UBallGrab() override = default;
    
    /** Gets EAbilityType::BallGrab. */
    EAbilityType GetType() const override final { return EAbilityType::BallGrab; }
    
    /** Normalises the grab angle so it can be used with a dot product. */
    virtual void Initialise() override;
    
    /** Attempts to grab the ball if it is in range. */
    virtual void Activate (APlayerCharacter* character, ABall* ball) override;
    
    /** Holds on to the ball unless the time runs out. */
    virtual void Continue (APlayerCharacter* character, ABall* ball, float deltaTime) override;
    
    /** Lets go of the ball. */
    virtual void Deactivate (APlayerCharacter* character, ABall* ball) override;
    
    /** Resets the state of the ability. */
    virtual void Stop (APlayerCharacter* character, ABall* ball) override;
    
protected:
    
    /** Calculates the preferred direction to throw in. */
    FVector CalculatePreferredDirection (const APlayerCharacter& character, const ABall& ball) const;
    
    /** Calculates where the ball should be held relative to the character. */
    FVector CalculateHoldingPosition (const APlayerCharacter& character, const ABall& ball) const;

public:
    
    /** The grab range in centimetres, can't be larger than the ball range. */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float GrabRange = 200.f;
    
    /** Specifies the angle of the cone in front of the character. The ball must be within this cone to be able to grab. */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float GrabAngle = 90.f;
    
    /** How long the ball can be held by the character. */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float GrabTimer = 2.f;
    
    /** How hard the ball is thrown when letting go. */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float ThrowingForce = 1000.f;

    /** How far in front of the character the ball should be held. */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float HoldingDistance = 100.f;

    /** The name of the bone to set the ball position to when being held. */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Ability")
    FName BoneToSetBallPositionTo = "Bip001-L-Hand";
    
    /** How long the ball has been held for. */
    float CurrentHeldTime = 0.f;

    /** The grab range of the ball as a cosine of the half angle in radians. */
    float NormalisedGrabAngle = 0.f;

    /** Indicates whether the ability should deactivate or not. */
    bool ShouldDeactivate = false;

    /** The position of the ball when it was grabbed. */
    FVector initialBallPosition;
};
