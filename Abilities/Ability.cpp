#include "GOAT.h"
#include "Ability.h"
#include "Characters/PlayerCharacter.h"


UAbility::UAbility() : Super()
{
    // Ensure the max cooldown is synchronised with the base cooldown.
    MaxCooldown = BaseCooldown;
}


void UAbility::AdjustMaxCooldown (float multiplier)
{
    // Don't go below zero cooldown and scale the current cooldown accordingly.
    MaxCooldown = FMath::Max (BaseCooldown * multiplier, 0.f);
    SetCurrentCooldown (CurrentCooldown * multiplier);
}


void UAbility::SetCurrentCooldown (float cooldown)
{
    CurrentCooldown = FMath::Clamp (cooldown, 0.f, MaxCooldown);
}


void UAbility::ReduceCooldown (float DeltaTime)
{
    SetCurrentCooldown (CurrentCooldown - DeltaTime);
}


void UAbility::RestartCooldown()
{
    SetCurrentCooldown (MaxCooldown);
    //GEngine->AddOnScreenDebugMessage (-1, 15.0f, FColor::Green, FString::Printf (TEXT ("%s cooldown: %f)"), *GUIName, CurrentCooldown));
}


void UAbility::ImmediatelyCool()
{
    SetCurrentCooldown (0.f);
}


void UAbility::Initialise()
{
    AdjustMaxCooldown (1.f);
}


void UAbility::Activate (APlayerCharacter* character, ABall* ball)
{
    // Activate if possible.
    auto reason = FString { };
    if (CanActivate (character, ball, reason))
    {
        Activated       = true;
        HasTriggered    = false;
        RunningTime     = 0.f;
    }

    // Inform the character that the ability failed.
    else
    {
        /*GEngine->AddOnScreenDebugMessage (-1, 15.f, FColor::Blue, 
            FString::Printf (TEXT ("%s::Activate(): %s."), *GUIName, *reason)
        );*/

        character->NotifyAbilityHasEnded (GetType());
    }
}


void UAbility::Continue (APlayerCharacter* character, ABall* ball, float deltaTime)
{
    // Increment the running time.
    RunningTime += deltaTime;

    if (!HasTriggered && RunningTime >= StartupTime)
    {
        Trigger (character, ball);
        HasTriggered = true;
    }

    else if (RunningTime >= AnimationTime)
    {
        Stop (character, ball);
    }
}


void UAbility::Deactivate (APlayerCharacter* character, ABall* ball)
{
    if (RunningTime >= AnimationTime)
    {
        Stop (character, ball);
    }
}


void UAbility::Stop (APlayerCharacter* character, ABall*)
{
    // Force the ability to stop uncleanly.
    Activated       = false;
    HasTriggered    = false;
    RunningTime     = 0.f;
    RestartCooldown();
    character->NotifyAbilityHasEnded (GetType());
}