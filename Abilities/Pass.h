#pragma once

#include "KickingAbility.h"
#include "Pass.generated.h"

/** A kicking ability that aims straight towards the goal. */
UCLASS(abstract, Blueprintable)
class GOAT_API UPass : public UKickingAbility
{
    GENERATED_BODY()

public:
    
    /** Sets the default values for the ability. */
    UPass();
    virtual ~UPass() override = default;

    /** Gets EAbilityType::Pass */
    EAbilityType GetType() const override final { return EAbilityType::Pass; }

protected:
       
    /** Chooses a direction from the characters movement input, the direction to the ball or forward of the character. */
    FVector CalculateDesiredDirection (const APlayerCharacter& character, const ABall& ball) const override final;
};
