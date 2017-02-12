#include "GOAT.h"
#include "Abilities.h"


TSubclassOf<UAbility> UAbilities::Get (EAbilityType type) const
{
    check (type != EAbilityType::None && TEXT ("UAbilities::Get() received EAbilityType::None."));
    
    for (auto wrapper : Abilities)
    {
        auto ability = wrapper->GetDefaultObject<UAbility>();
        check (ability && "Abilities list contains data invalid data, ensure each element has a value.")
        
        if (ability->GetType() == type)
        {
            return wrapper;
        }
    }
    
    return nullptr;
}