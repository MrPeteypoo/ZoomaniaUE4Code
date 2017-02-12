#include "GOAT.h"
#include "Role.h"


URole::URole() : Super()
{
    Stats.MovementSpeed         = 500.f;
    Stats.KickingForce          = 750.f;
    Stats.BuffTimeMultiplier    = 1.f;
    Stats.DebuffTimeMultiplier  = 1.f;
    CapsuleDimensions           = FVector2D { 68.f, 176.f };
}


void URole::Initialise()
{
    // Instantiate the ability container.
    checkf (AbilityContainer, TEXT ("%s role doesn't contain an Ability Container as required."), *GUIName);
    auto abilityContainer = NewObject<UAbilities> (this, AbilityContainer);
    
    // Instantiate extra abilities.
    for (auto type : SpecialAbilities)
    {
        check (type != EAbilityType::None && "URole::ExtraAbilities contains invalid data, ensure None is not selected.");

        const auto wrapper = abilityContainer->Get (type);
        checkf (wrapper, TEXT ("Unable to duplicate ability %s"), EnumAsString (type));

        auto ability = NewObject<UAbility> (this, wrapper);
        ability->Initialise();

        Abilities.Add (ability);
    }

    // Instantiate base abilities.
    if (PassAbility && NormalKickAbility && SpinKickAbility)
    {
        PassIndex       = Abilities.Num();
        NormalKickIndex = PassIndex + 1;
        SpinKickIndex   = NormalKickIndex + 1;

        Abilities.Add (NewObject<UAbility> (this, PassAbility));
        Abilities.Add (NewObject<UAbility> (this, NormalKickAbility));
        Abilities.Add (NewObject<UAbility> (this, SpinKickAbility));
    }

    else
    {
        GEngine->AddOnScreenDebugMessage (-1, 5.f, FColor::Red, 
            FString::Printf (TEXT ("Base abilities not set for role: %s"), *GUIName)
        );
    }

    // Instantiate the animations object.
    Animations = NewObject<UAnimInstance> (this, AnimationBlueprint);
}


void URole::UpdateAbilities (APlayerCharacter* character, ABall* ball, float deltaTime)
{
    for (auto ability : Abilities)
    {
        checkf (ability, TEXT ("%s role has a nullptr in URole::Abilities."), *GUIName);

        // Cooldowns must be reduced and if the ability is activated then it must continue.
        ability->ReduceCooldown (deltaTime);
        
        if (ability->IsActivated())
        {
            ability->Continue (character, ball, deltaTime);
        }
    }
}

void URole::StopAbilities (APlayerCharacter* character, ABall* ball)
{
    for (auto ability : Abilities)
    {
        if (ability->IsActivated())
        {
            ability->Deactivate (character, ball);
        }
    }
}


USkeletalMesh* URole::GetMesh (ETeam team) const
{
    return team == ETeam::Blue ? BlueTeamMesh : RedTeamMesh;
}


EAbilityType URole::GetAbilityType (int32 index) const
{
    if (ValidateIndex (index))
    {
        return Abilities[index]->GetType();
    }

    return EAbilityType::None;
}


UAbility* URole::GetAbility (int32 index)
{
    if (ValidateIndex (index))
    {
        return Abilities[index];
    }

    return nullptr;
}


bool URole::ActivateAbility (int32 abilityIndex, APlayerCharacter* character, ABall* ball)
{
    return PerformActionOnAbility (abilityIndex, [=] (UAbility* ability)
    {
        if (!ability->IsActivated() && ability->IsReady())
        {
            ability->Activate (character, ball);

            return ability->IsActivated();
        }

        return false;
    });
}


void URole::DeactivateAbility (int32 abilityIndex, APlayerCharacter* character, ABall* ball)
{
    PerformActionOnAbility (abilityIndex, [=] (UAbility* ability)
    {
        if (ability->IsActivated())
        {
            ability->Deactivate (character, ball);
        }

        return true;
    });
}


bool URole::ValidateIndex (int32 abilityIndex) const
{
    if (abilityIndex >= Abilities.Num() || abilityIndex < 0)
    {
        GEngine->AddOnScreenDebugMessage 
        (
            -1, 
            15.0f, 
            FColor::Red, 
            FString::Printf 
            (
                TEXT ("URole::ValidateIndex(): Attempt to access ability %i on %s when only %i abilities exist."), 
                abilityIndex, *GUIName, Abilities.Num()
            )
        );
        
        return false;
    }

    return true;
}


bool URole::PerformActionOnAbility (int32 abilityIndex, TFunctionRef<bool (UAbility*)> action)
{
    if (ValidateIndex (abilityIndex))
    {
        auto ability = Abilities[abilityIndex];
        return action (ability);
    }

    return false;
}