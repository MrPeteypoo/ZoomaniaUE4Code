#pragma once

#include "Ability.h"
#include "Abilities.generated.h"

/** A container of references to ability blueprints. */
UCLASS(abstract, Blueprintable)
class GOAT_API UAbilities : public UObject
{
    GENERATED_BODY()
    
public:
    
    UAbilities() {}
    ~UAbilities() = default;
    
    /** Gets a copy of a wrapper around an ability with the given underlying type enum. */
    TSubclassOf<UAbility> Get (EAbilityType type) const;
    
protected:
    
    /** Contains references to all ability types that can be instantiated from this container. */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Abilities")
    TArray<TSubclassOf<UAbility>> Abilities;
};
