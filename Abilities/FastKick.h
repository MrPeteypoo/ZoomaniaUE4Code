#pragma once

#include "NormalKick.h"
#include "FastKick.generated.h"

/** A kicking ability that aims straight towards the goal. */
UCLASS(abstract, Blueprintable)
class GOAT_API UFastKick : public UNormalKick
{
    GENERATED_BODY()

public:

    UFastKick();

    /** Gets EAbilityType::FastKick */
    EAbilityType GetType() const override final { return EAbilityType::FastKick; }
};
