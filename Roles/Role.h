#pragma once

#include "Abilities/Abilities.h"
#include "Abilities/Pass.h"
#include "Abilities/NormalKick.h"
#include "Abilities/SpinKick.h"
#include "Characters/CharacterStats.h"
#include "Characters/Team.h"
#include "Containers/Map.h"
#include "Role.generated.h"

/** The role of a character, this determines the appearance and what abilities are available to the character. */
UCLASS(abstract, Blueprintable)
class GOAT_API URole : public UObject
{
	GENERATED_BODY()
    
public:	
    
    /** Sets default stats. */
    URole();
    
    /** Instantiates each available ability. */
    void Initialise();
    
    /** Updates each ability, reducing cooldowns and allowing abilities to continue. */
    void UpdateAbilities (APlayerCharacter* character, ABall* ball, float deltaTime);
    
    /** Forces all abilities to stop what they're doing. */
    UFUNCTION (BlueprintCallable, Category = "Role")
    void StopAbilities (APlayerCharacter* character, ABall* ball);
    
    /** Returns the name of the Role as it should be displayed in the GUI. */
    UFUNCTION (BlueprintPure, Category = "Role")
    const FString& GetGUIName() const { return GUIName; }
    
    /** Returns the total number of available special abilities to the role. */
    UFUNCTION (BlueprintPure, Category = "Role")
    int32 GetSpecialAbilityCount() const { return SpecialAbilities.Num(); }
    
    /** Gets the base statistics and multipliers of the role. */
    UFUNCTION (BlueprintPure, Category = "Role")
    const FCharacterStats& GetStats() const { return Stats; }
    
    /** Gets the desired scale of the scale. */
    UFUNCTION (BlueprintPure, Category = "Role")
    float GetActorScale() const { return ActorScale; }
    
    /** Gets the animations to use for animating the character. */
    UFUNCTION (BlueprintPure, Category = "Role")
    UAnimInstance* GetAnimations() const { return Animations; }
    
    /** Gets the desired dimensions of the capsule collider. */
    UFUNCTION (BlueprintPure, Category = "Role")
    const FVector2D& GetCapsuleDimensions() const { return CapsuleDimensions; }
    
    /** Gets the index of the pass ability. */
    UFUNCTION (BlueprintPure, Category = "Role")
    int32 GetPassIndex() const { return PassIndex; }
    
    /** Gets the index of the normal kick ability. */
    UFUNCTION (BlueprintPure, Category = "Role")
    int32 GetNormalKickIndex() const { return NormalKickIndex; }
    
    /** Gets the index of the spin kick ability. */
    UFUNCTION (BlueprintPure, Category = "Role")
    int32 GetSpinKickIndex() const { return SpinKickIndex; }
    
    /** Gets the skeletal mesh used to represent the role. */
    UFUNCTION (BlueprintPure, Category = "Role")
    USkeletalMesh* GetMesh (ETeam team) const;
    
    /** Gets the EAbilityType of the ability at the given index. */
    UFUNCTION (BlueprintPure, Category = "Role")
    EAbilityType GetAbilityType (int32 index) const;
    
    /** Gets the ability at the given index. This can be used to get base abilities and special abilities. */
    UFUNCTION (BlueprintPure, Category = "Role")
    UAbility* GetAbility (int32 index);
    
    /** 
        Attempts to activate the chosen ability. The ability will not be activated if already active,
        index must begin at 0 and be less than GetAbilityCount(). Character and ball parameters are optional.
        Returns whether the ability activated or not.
    */
    UFUNCTION (BlueprintCallable, Category = "Role")
    bool ActivateAbility (int32 abilityIndex, APlayerCharacter* character, ABall* ball);
    
    /** 
        Attempts to deactivate the chosen ability. Index must begin at 0 and be less than GetAbilityCount().
        Character and ball parameters are optional.
    */
    UFUNCTION (BlueprintCallable, Category = "Role")
    void DeactivateAbility (int32 abilityIndex, APlayerCharacter* character, ABall* ball);
    
protected:
    
    /** Validates the given index, prints a message if invalid. */
    bool ValidateIndex (int32 abilityIndex) const;
    
    /** Checks the validity of the given index and performs the given action on the ability at the given index. */
    bool PerformActionOnAbility (int32 abilityIndex, TFunctionRef<bool (UAbility*)> action);
    
protected:

    /** The display name of the Role in the GUI. */
    UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Role")
    FString GUIName = "Role";
    
    /** The base statistics and multipliers of the role. */
    UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Role")
    FCharacterStats Stats;
    
    /** 
        When the role is created it will initialise its abilities with the values stored within this container. The
        container should hold all required abilities specified in Available Ability Types otherwise the game won't
        run.
    */
    UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Role")
    TSubclassOf<UAbilities> AbilityContainer = nullptr;
    
    /** Which special abilities should be available to this particular role. */
    UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Role")
    TArray<EAbilityType> SpecialAbilities;

    /** The freely-aimed kicking ability. */
    UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Role")
    TSubclassOf<UPass> PassAbility;

    /** The goal-targetted kicking ability. */
    UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Role")
    TSubclassOf<UNormalKick> NormalKickAbility;

    /** The goal-targetted spin kicking ability. */
    UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Role")
    TSubclassOf<USpinKick> SpinKickAbility;
    
    /** The animation blueprint to apply to the character. */
    UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Role")
    TSubclassOf<UAnimInstance> AnimationBlueprint;
    
    /** The visual representation of the Role to apply to characters. */
    UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Role")
    USkeletalMesh* RedTeamMesh = nullptr;
    
    /** The visual representation of the Role to apply to characters. */
    UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Role")
    USkeletalMesh* BlueTeamMesh = nullptr;
    
    /** The scale to apply to the character when switching to this role. */
    UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Role")
    float ActorScale = 1.f;
    
    /** The width and height of the character to be covered by the capsule, dimensions are in centimetres. */
    UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Role")
    FVector2D CapsuleDimensions;
    
    /** Contains instantiated copies of each ability specified in AvailableAbilityTypes. */
    UPROPERTY()
    TArray<UAbility*> Abilities;

    /** The index to the pass ability. */
    int32 PassIndex = 0;

    /** The index to the normal kick ability. */
    int32 NormalKickIndex = 0;

    /** The index to the spin kick ability. */
    int32 SpinKickIndex = 0;
    
    /** The instantiated object of the given animation blueprint. */
    UPROPERTY()
    UAnimInstance* Animations;
};
