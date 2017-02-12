#pragma once

#include "KickingAbility.h"
#include "NormalKick.generated.h"

/** A kicking ability that aims straight towards the goal. */
UCLASS(abstract, Blueprintable)
class GOAT_API UNormalKick : public UKickingAbility
{
    GENERATED_BODY()

public:
    
    /** Sets the default values for the ability. */
    UNormalKick();
    virtual ~UNormalKick() override = default;

    /** Gets EAbilityType::NormalKick */
    EAbilityType GetType() const override { return EAbilityType::NormalKick; }

protected:
       
    /** Aims towards the goal, using the Y axis input as an offset. */
    FVector CalculateDesiredDirection (const APlayerCharacter& character, const ABall& ball) const override final;
};
