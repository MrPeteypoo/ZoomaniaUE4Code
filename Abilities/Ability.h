#pragma once

#include "AbilityType.h"
#include "Ability.generated.h"

// Forward declarations.
class APlayerCharacter;
class ABall;

/** 
    A base class to all ability types in the game. Abilities have three stages, activation, continuation and
    deactivation. At each stage abilities have access to the character containing the ability and the ball in the 
    level. Using this complex abilities can be created.
*/
UCLASS(abstract)
class GOAT_API UAbility : public UObject
{
	GENERATED_BODY()
    
public:
    
    /** Synchronises the maximum and base cooldown values. */
    UAbility();
    virtual ~UAbility() = default;
    
    /** Gets the name of the ability as it should appear in the GUI. */
    UFUNCTION (BlueprintPure, Category = "Ability")
    const FString& GetGUIName() const { return GUIName; }
    
    /** Checks if the ability is currently activated. */
    UFUNCTION (BlueprintPure, Category = "Ability")
    bool IsActivated() const { return Activated; }
    
    /** Checks if the ability is ready to be activated. */
    UFUNCTION (BlueprintPure, Category = "Ability")
    bool IsReady() const { return CurrentCooldown == 0.f; }
    
    /** Gets the base ability cooldown in seconds. */
    UFUNCTION (BlueprintPure, Category = "Ability")
    float GetBaseCooldown() const { return BaseCooldown; }
    
    /** Gets the maximum cooldown of the ability after modifiers and multipliers are applied in seconds. */
    UFUNCTION (BlueprintPure, Category = "Ability")
    float GetMaxCooldown() const { return MaxCooldown; }
    
    /** Gets the current cooldown of the ability in seconds. */
    UFUNCTION (BlueprintPure, Category = "Ability")
    float GetCurrentCooldown() const { return CurrentCooldown; }
    
    /** Applies a multiplier to the base cooldown, giving a new maximum cooldown. */
    UFUNCTION (BlueprintCallable, Category = "Ability")
    void AdjustMaxCooldown (float multiplier = 1.f);    
    
    /** Sets the current cooldown to a hard value. */
    UFUNCTION (BlueprintCallable, Category = "Ability")
    void SetCurrentCooldown (float cooldown);
    
    /** Reduces the cooldown by the given number of seconds. The cooldown will be clamped to zero. */
    UFUNCTION (BlueprintCallable, Category = "Ability")
    void ReduceCooldown (float deltaTime);
    
    /** Restarts the cooldown from the current maximum cooldown value. */
    UFUNCTION (BlueprintCallable, Category = "Ability")
    void RestartCooldown();
    
    /** Immediately reduces the cooldown to zero. */
    UFUNCTION (BlueprintCallable, Category = "Ability")
    void ImmediatelyCool();
    
    UFUNCTION (BlueprintPure, Category = "Ability")
    /** Gets an enumerated representation of the abilities underlying type. */
    virtual EAbilityType GetType() const { check (0 && "UAbility::GetType() must be overriden"); return EAbilityType::None; }
    
    /** Perform any necessary variable initialisation. */
    UFUNCTION (Category = "Ability")
    virtual void Initialise();
    
    /** Activates the ability, setting the activated flag to true. */
    UFUNCTION (Category = "Ability")
    virtual void Activate (APlayerCharacter* character, ABall* ball);
    
    /** Will be called every frame of activation. Does nothing by default. */
    UFUNCTION (Category = "Ability")
    virtual void Continue (APlayerCharacter* character, ABall* ball, float deltaTime);
    
    /** Sets the activation flag to false and restarts the cooldown if activated. */
    UFUNCTION (Category = "Ability")
    virtual void Deactivate (APlayerCharacter* character, ABall* ball);
    
    /** Performs a hard stop which interrupts the ability. */
    UFUNCTION (Category = "Ability")
    virtual void Stop (APlayerCharacter* character, ABall* ball);
    
protected:

    /** Check if the ability can activate. */
    UFUNCTION (Category = "Ability")
    virtual bool CanActivate (APlayerCharacter* character, ABall* ball, FString& reason) const { return true; }

    /** Triggers the ability. */
    UFUNCTION (Category = "Ability")
    virtual void Trigger (APlayerCharacter* character, ABall* ball) { }

protected:
    
    /** The name of the Ability as it should appear in the GUI. */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Ability")
    FString GUIName = "UAbility";

    /** How long does the animation take to complete. */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float AnimationTime = 1.5f;

    /** How long before the ability actually takes effect. */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float StartupTime = 1.f;

    /** How many seconds should the ability take to cooldown. */
    UPROPERTY (EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float BaseCooldown = 1.f;

    /** How long the ability has been running. */
    float RunningTime = 0.f;

    /** Tracks whether the ability has passed the startup time and has already triggered. */
    bool HasTriggered = false;

    /** The current maximum cooldown of the ability, this can be scaled based on the base cooldown. */
    float MaxCooldown = 1.f;

    /** How long until the ability has finished cooling. */
    float CurrentCooldown = 0.f;

    /** Whether the ability is currently activated. */
    bool Activated = false;
};