#pragma once

class APlayerCharacter;

float CalculateMovementSpeed (const APlayerCharacter& character, float multiplier = 1.f, float modifier = 0.f);

float CalculateKickingForce (const APlayerCharacter& character, float multiplier = 1.f, float modifier = 0.f);

float CalculateBuffTime (const APlayerCharacter& character, float buffTime);

float CalculateDebuffTime (const APlayerCharacter& character, float debuffTime);