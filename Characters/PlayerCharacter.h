#pragma once

#include "GameFramework/Character.h"
#include "Characters/CharacterState.h"
#include "Characters/Team.h"
#include "Roles/Role.h"
#include "PlayerCharacter.generated.h"

// Forward declarations.
class AGoal;
class UInputComponent;

/** A playable character in the game, characters can have multiple roles which can be swapped at will. */
UCLASS(abstract)
class GOAT_API APlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:

    /** Sets default values for the character. */
    APlayerCharacter();
    
    /** Attempts to search for the ball in the scene. */
    virtual void BeginPlay() override;
    
    /** Updates the position of the character. */
    virtual void Tick (float deltaSeconds) override;
    
    /** Binds each required input. */
    virtual void SetupPlayerInputComponent (UInputComponent* inputComponent) override;

    /** Sets the movement vector of the character to the given value.*/
    void SetMovementVector (const FVector& movement) { PreviousMovement = movement; }

    /** Gets the current role of the character.. */
    UFUNCTION (BlueprintPure, Category = "Character")
    URole* GetRole() const { return Role; }

    /** Gets the current state of the character. */
    UFUNCTION (BlueprintPure, Category = "Character")
    ECharacterState GetState() const { return State; }

    /** Gets the current state of the character. */
    UFUNCTION (BlueprintPure, Category = "Character")
    EAbilityType GetCurrentAbilityType() const { return Ability; }

    /** Gets the team the character is currently on. */
    UFUNCTION (BlueprintPure, Category = "Character")
    ETeam GetTeam() const { return Team; }

    /** Retrieves the target that the character is aiming at. */
    UFUNCTION (BlueprintPure, Category = "Character")
    AGoal* GetTargetGoal() const { return TargetGoal; }

    /** Gets the desired curvature of the curve shot ability. */
    UFUNCTION (BlueprintPure, Category = "Character")
    float GetCurveShotCurvature() const { return CurveShotCurvature; }

    /** Gets the current movement input. */
    UFUNCTION (BlueprintPure, Category = "Character")
    FVector GetMovementInput() const { return Input; }

    /** Turns the current movement input of the player into a direction. */
    UFUNCTION (BlueprintPure, Category = "Character")
    FVector MovementAsDirection() const { return Input.GetSafeNormal(); }

    /** Gets the most recently applied movement. */
    UFUNCTION (BlueprintPure, Category = "Character")
    FVector GetMovement() const { return PreviousMovement; }

    /** Sets the team the of the character, this will cause the character to search for a goal to target. */
    UFUNCTION (BlueprintCallable, Category = "Character")
    void SetTeam (ETeam team);

    /** Sets the role of the character to a role matching the given string name. */
    UFUNCTION (BlueprintCallable, Category = "Character")
    void SetRole (const FString& roleName);
    
    /** Informs the player to perform no action for the given number of seconds. */
    UFUNCTION (BlueprintCallable, Category = "Character")
    void AddCooldown (float seconds);
    
    /** Stuns the player for the given number of seconds (before modifiers). */
    UFUNCTION (BlueprintCallable, Category = "Character")
    void Stun (float seconds);
    
    /** Notifies the character that the ability has finished so the state should be updated. */
    UFUNCTION (BlueprintCallable, Category = "Character")
    void NotifyAbilityHasEnded (EAbilityType ability);

    /** Overrides the current state with the given value. */
    void SetState (ECharacterState state) { State = state; }

private:

    /** An action that has been requested to be performed. */
    enum class ERequestedAction : uint8
    {
        RoleChange,
        ActivateAbility,
        DeactivateAbility
    };

    /** Instantiates roles from the blueprints specified in AvailableRoles. */
    void InstantiateRoles();

    /** Sets the role of the character to the role stored at the specified index. */
    void SwitchToRole (int32 index);

    /** Searches the currently open level for a ball to watch. */
    ABall* FindBall() const;

    /** Searches the currently open level for a goal to target. */
    AGoal* FindGoal() const;

    /** Updates every instantiated role. */
    void UpdateRoles (float deltaTime);

    /** Checks which abilities have finished and abilities the state accordingly. */
    void ProcessCompletedAbilities();

    /** Manages the state of the character, processing movement input if possible. */
    void UpdateState (float deltaTime);

    /** Attempts to move the character if input has been provided. */
    void Move (float deltaTime);

    /** Processes requested input. */
    void ProcessRequests (float deltaTime);

    /** Manages the cooldown of the character. */
    void ManageCooldown (float deltaTime);

    /** Manages the ability currently being used. */
    void ManageAbility();

    /** Requests the given action is performed, if the action is an ability then an index is required. */
    void RequestAction (ERequestedAction action, int32 abilityIndex = 0);

    /** Moves forward by the given value if possible. */
    void RequestMoveUp (float value) { Input.Y = value; }

    /** Moves right by the given value if possible. */
    void RequestMoveRight (float value) { Input.X = value; }

    /** Requests that the spin kick use the following curvature. */
    void RequestCurveShotCurvature (float curvature) { CurveShotCurvature = FMath::Clamp (curvature, -1.f, 1.f); }

    /** Requests that the character increment its role. */
    void RequestRoleChange() { RequestAction (ERequestedAction::RoleChange); }

    /** Requests the ability at the given index is activated. */
    template <int32 n>
    void RequestActivateAbility() { RequestAction (ERequestedAction::ActivateAbility, n); }

    /** Requests the ability at the given index is deactivated. */
    template <int32 n>
    void RequestDeactivateAbility() { RequestAction (ERequestedAction::DeactivateAbility, n); }
    
protected:
    
    /** Which roles are accessible to the character. */
    UPROPERTY (EditAnywhere, BlueprintReadOnly, Category = "Character")
    TArray<TSubclassOf<URole>> AvailableRoles;

private:

    /** A reference to the goal being targetted by the character. */
    UCharacterMovementComponent* Movement = nullptr;
    
    /** Contains instantiated URole objects. */
    UPROPERTY() 
    TArray<URole*> Roles;

    /** The index of the current role. */
    int32 RoleIndex = 0;
    
    /** The current role of the player. */
    URole* Role = nullptr;

    /** The current state of the character. */
    ECharacterState State = ECharacterState::Idle;

    /** The ability currently being used. */
    EAbilityType Ability = EAbilityType::None;

    /** Which team the character is on. */
    ETeam Team = ETeam::Red;

    /** The current cooldown applied to the character, this prevents input having an effect. */
    float Cooldown = 0.f;

    /** A cached copy of the most recent movement input from the player. */
    FVector Input;

    /** A left/right curvature scalar for the curve shot. */
    float CurveShotCurvature = 0.f;

    /** A queue of actions that have been requested to be performed. */
    TQueue<TPair<ERequestedAction, int32>> ActionQueue;

    /** A queue of abilities which have sent a notification that they've finished what they were doing. */
    TQueue<EAbilityType> CompletedAbilities;
    
    /** A reference to the ball in the world. */
    ABall* Ball = nullptr;

    /** A reference to the goal being targetted by the character. */
    AGoal* TargetGoal = nullptr;

    /** The previous movement that occurred. */
    FVector PreviousMovement;
};
