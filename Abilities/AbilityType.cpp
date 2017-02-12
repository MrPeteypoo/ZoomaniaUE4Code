#include "GOAT.h"
#include "AbilityType.h"


FString EnumAsString (const EAbilityType value)
{
    const auto uenum = FindObject<UEnum> (ANY_PACKAGE, TEXT("EAbilityType"), true);
    check (uenum && "This should never happen.");  

    return uenum->GetEnumName (static_cast<uint8> (value));
}