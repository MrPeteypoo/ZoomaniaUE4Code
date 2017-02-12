// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Ability.h"
#include "KickingAbility.generated.h"

/** A base class for kicking abilities. */
UCLASS(abstract)
class GOAT_API UKickingAbility : public UAbility
{
	GENERATED_BODY()

public:
	
    /** Sets the default name. */
    UKickingAbility();
	virtual ~UKickingAbility() override = default;

protected:

    /** Check if the ball is in range. */
    virtual bool CanActivate (APlayerCharacter* character, ABall* ball, FString& reason) const override;

    /** Kicks the ball towards the desired direction. */
    virtual void Trigger (APlayerCharacter* character, ABall* ball) override;
    
    /** Gets the desired direction to kick the ball in. */
    virtual FVector CalculateDesiredDirection (const APlayerCharacter& character, const ABall& ball) const { return { }; }

public:

    /** The maximum kicking range from the ball for the ability to activate. */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float MaximumRange = 150.f;

    /** A multiplier to apply to the characters kicking force. */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float KickingForceMultiplier = 1.f;
    
    /** How much of the balls current velocity should be maintained, this is a multiplier. */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float BallRestitution = 1.f;
};
