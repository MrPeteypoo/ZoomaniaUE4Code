#pragma once

#include "Ability.h"
#include "Deflect.generated.h"

/** An ability where the character will deflect the ball away from the character. */
UCLASS(abstract, Blueprintable)
class GOAT_API UDeflect : public UAbility
{
    GENERATED_BODY()

public:
    /** Sets the name and base cooldown of the ability. */    
    UDeflect();
    virtual ~UDeflect() override = default;

    /** Gets EAbilityType::Deflect. */
    EAbilityType GetType() const override final { return EAbilityType::Deflect; }

protected:

    /** Check if the ball is in range. */
    virtual bool CanActivate (APlayerCharacter* character, ABall* ball, FString& reason) const override;

    /** Deflects the ball in the preferred direction. */
    virtual void Trigger (APlayerCharacter* character, ABall* ball) override;

    /** Calculates the preferred direction to deflect to. */
    FVector CalculatePreferredDirection (const APlayerCharacter& character, const ABall& ball) const;

public:

    /** How close the ball must be to be deflected, in centimetres. */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float DeflectionRange = 150.f;

    /** How much force is added to the ball on deflection. */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float DeflectionForce = 400.f;

    /** How much velocity the ball should maintain. */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float BallResitution = 0.3f;
};
