#include "GOAT.h"
#include "Formulae.h"
#include "PlayerCharacter.h"
#include "Roles/Role.h"


float CalculateMovementSpeed (const APlayerCharacter& character, float multiplier, float modifier)
{
    return character.GetRole()->GetStats().MovementSpeed * multiplier + modifier;
}


float CalculateKickingForce (const APlayerCharacter& character, float multiplier, float modifier)
{
    return character.GetRole()->GetStats().KickingForce * multiplier + modifier;
}


float CalculateBuffTime (const APlayerCharacter& character, float buffTime)
{
    return character.GetRole()->GetStats().BuffTimeMultiplier * buffTime;
}


float CalculateDebuffTime (const APlayerCharacter& character, float debuffTime)
{
    return character.GetRole()->GetStats().DebuffTimeMultiplier * debuffTime;
}