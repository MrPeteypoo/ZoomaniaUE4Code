#pragma once

#include "Ability.h"
#include "Dive.generated.h"

/** An ability where the character will deflect the ball away from the character. */
UCLASS(abstract, Blueprintable)
class GOAT_API UDive : public UAbility
{
    GENERATED_BODY()

public:
    /** Sets the name and base cooldown of the ability. */    
    UDive();
    virtual ~UDive() override = default;

    /** Gets EAbilityType::Dive. */
    EAbilityType GetType() const override final { return EAbilityType::Dive; }

protected:
    
    /** Retrieves the movement vector for the dive. */
    virtual void Activate (APlayerCharacter* character, ABall* ball) override;

    /** Moves forward according to the ability speed. */
    virtual void Continue (APlayerCharacter* character, ABall* ball, float deltaTime) override;

public:

    /** How long should the character dive for. */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float DiveDuration = 0.5f;

    /** How fast the dive should be. */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float DiveSpeed = 750.f;

    /** How fast the dive should be when exiting. */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float ExitDiveSpeed = 75.f;

    /** How much force should be applied to the ball when hitting it. */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float CollisionForce = 1500.f;

    /** Stores the direction the character is diving in to apply. */
    FVector DiveDirection;
};
