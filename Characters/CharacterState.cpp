#include "GOAT.h"
#include "CharacterState.h"


FString EnumAsString (const ECharacterState value)
{
    const auto uenum = FindObject<UEnum> (ANY_PACKAGE, TEXT("ECharacterState"), true);
    check (uenum && "This should never happen.");  

    return uenum->GetEnumName (static_cast<uint8> (value));
}