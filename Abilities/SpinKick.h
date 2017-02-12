#pragma once

#include "KickingAbility.h"
#include "SpinKick.generated.h"

/** A special type of shot that shoots the ball towards the goal using a quadratic bezier curve. */
UCLASS(abstract, Blueprintable)
class GOAT_API USpinKick : public UKickingAbility
{
    GENERATED_BODY()

public:
    
    /** Sets the default values of the shot. */
    USpinKick();
    virtual ~USpinKick() override = default;

    /** Gets EAbilityType::SpinKick. */
    virtual EAbilityType GetType() const override { return EAbilityType::SpinKick; }

protected:

    /** Cause a curve shot on the ball that will spin towards the goal. */
    virtual void Trigger (APlayerCharacter* character, ABall* ball) override;

    void Shoot (APlayerCharacter& character, ABall& ball, 
        const FVector& direction, const float force, const float timeDelta, const float curveDelta);

protected:

    /** 
        The minimum amount of seconds it should take to complete a curve before the ball meets the forward direction
        of the kicking character.
    */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category="Ability")
    float MinTravelTime = 1.5f;

    /** 
        The maximum amount of seconds it should take to complete a curve before the ball meets the forward direction
        of the kicking character.
    */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category="Ability")
    float MaxTravelTime = 0.75f;

    /** The minimum scalar for the curvature of the shot. This is proportionate to travel time of the curve. */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category="Ability")
    float MinCurvature = 0.1f;

    /** A maximum scalar for the curvature of the shot. This is proportionate to travel time of the curve. */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category="Ability")
    float MaxCurvature = 1.f;
};
