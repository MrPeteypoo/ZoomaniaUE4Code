#include "GOAT.h"
#include "Team.h"


FString EnumAsString (ETeam value)
{
    const auto uenum = FindObject<UEnum> (ANY_PACKAGE, TEXT("ETeam"), true);
    check (uenum && "This should never happen.");  

    return uenum->GetEnumName (static_cast<uint8> (value));
}