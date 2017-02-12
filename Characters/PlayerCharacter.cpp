#include "GOAT.h"
#include "Ball.h"
#include "Environment/Goal.h"
#include "Formulae.h"
#include "PlayerCharacter.h"


APlayerCharacter::APlayerCharacter() : Super()
{
    // Set this character to call Tick() every frame.
    PrimaryActorTick.bCanEverTick = true;
    Tags.Add ("Player");
    bCanBeDamaged = false;

    // Set capsule presets.
    auto capsule = GetCapsuleComponent();
    capsule->bGenerateOverlapEvents = true;
    capsule->SetSimulatePhysics (false);
    capsule->SetCollisionProfileName ("PhysicsActor");
    capsule->SetLinearDamping (0.f);
    capsule->SetPhysicsMaxAngularVelocity (0.f);

    auto movement = CastChecked<UCharacterMovementComponent> (GetMovementComponent());
    movement->bEnablePhysicsInteraction = true;
    movement->InitialPushForceFactor    = 350.f;
    movement->PushForceFactor           = 350.f;
}


void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    // Set Movement.
    Movement = CastChecked<UCharacterMovementComponent> (GetMovementComponent());

    // Prepare the roles.
    InstantiateRoles();

    // Allow for incorrect configurations by ensuring a role is always set.
    if (!Role)
    {
        SwitchToRole (0);
    }

    // Attempt to find the ball and goal.
    SetTeam (Team);
    Ball = FindBall();

    if (!Ball)
    {
        GEngine->AddOnScreenDebugMessage (-1, 5.f, FColor::Red, 
            TEXT ("APlayerCharacter::BeingPlay(): Unable to find ball in loaded level.")
        );
    }
}


void APlayerCharacter::Tick (float deltaTime)
{
    Super::Tick (deltaTime);

    UpdateRoles (deltaTime);
    ProcessCompletedAbilities();
    UpdateState (deltaTime);
}


void APlayerCharacter::SetupPlayerInputComponent (UInputComponent* inputComponent)
{
    Super::SetupPlayerInputComponent (inputComponent);
    
    // Bind the axes.
    inputComponent->BindAxis ("MoveForward",        this, &APlayerCharacter::RequestMoveUp);
    inputComponent->BindAxis ("MoveRight",          this, &APlayerCharacter::RequestMoveRight);
    inputComponent->BindAxis ("CurveShotCurvature", this, &APlayerCharacter::RequestCurveShotCurvature);

    // Bind actions.
    inputComponent->BindAction ("SwitchClass",      IE_Pressed,     this, &APlayerCharacter::RequestRoleChange);
    inputComponent->BindAction ("UseAbilityOne",    IE_Pressed,     this, &APlayerCharacter::RequestActivateAbility<0>);
    inputComponent->BindAction ("UseAbilityOne",    IE_Released,    this, &APlayerCharacter::RequestDeactivateAbility<0>);
    inputComponent->BindAction ("UseAbilityTwo",    IE_Pressed,     this, &APlayerCharacter::RequestActivateAbility<1>);
    inputComponent->BindAction ("UseAbilityTwo",    IE_Released,    this, &APlayerCharacter::RequestDeactivateAbility<1>);
    inputComponent->BindAction ("Pass",             IE_Pressed,     this, &APlayerCharacter::RequestActivateAbility<2>);
    inputComponent->BindAction ("Pass",             IE_Released,    this, &APlayerCharacter::RequestDeactivateAbility<2>);
    inputComponent->BindAction ("Shoot",            IE_Pressed,     this, &APlayerCharacter::RequestActivateAbility<3>);
    inputComponent->BindAction ("Shoot",            IE_Released,    this, &APlayerCharacter::RequestDeactivateAbility<3>);
    inputComponent->BindAction ("CurveShot",        IE_Pressed,     this, &APlayerCharacter::RequestActivateAbility<4>);
    inputComponent->BindAction ("CurveShot",        IE_Released,    this, &APlayerCharacter::RequestDeactivateAbility<4>);
}


void APlayerCharacter::AddCooldown (float seconds)
{
    Cooldown += seconds;
}


void APlayerCharacter::SetTeam (ETeam team)
{
    Team        = team;
    TargetGoal  = FindGoal();
}


void APlayerCharacter::SetRole (const FString& roleName)
{
    // Ensure we have valid roles before doing anything.
    InstantiateRoles();

    // Now find the correct role to apply.
    for (size_t i { 0 }; i < Roles.Num(); ++i)
    {
        const auto role = Roles[i];
        
        if (role->GetGUIName() == roleName)
        {
            SwitchToRole (i);
            RoleIndex = i;
            return;
        }
    }
}


void APlayerCharacter::InstantiateRoles()
{
    // Ensure we haven't already instantiated the roles.
    if (Roles.Num() != 0)
    {
        return;
    }

    // We need to convert from TSubclassOf wrappers to actual objects.
    for (auto wrapper : AvailableRoles)
    {
        Roles.Add (NewObject<URole> (this, wrapper));
    }

    for (auto role : Roles)
    {
        role->Initialise();
    }
}


void APlayerCharacter::Stun (float seconds)
{
    Cooldown = CalculateDebuffTime (*this, seconds);
    Role->StopAbilities (this, Ball);
    
    // Update state and ability accordingly.
    State   = ECharacterState::Stunned;
    Ability = EAbilityType::None;
}


void APlayerCharacter::NotifyAbilityHasEnded (EAbilityType ability)
{
    CompletedAbilities.Enqueue (ability);
}


void APlayerCharacter::SwitchToRole (int32 index)
{
    if (Roles.Num() != 0)
    {
        // Change the current role.
        Role = Roles[index];

        // Update the mesh and animations.
        auto mesh       = GetMesh();
        auto animations = Role->GetAnimations();
        auto dimensions = Role->GetCapsuleDimensions();
        auto radius     = dimensions.X / 2.f;
        auto halfHeight = dimensions.Y / 2.f;

        mesh->SetSkeletalMesh (Role->GetMesh (Team));
        mesh->RelativeLocation.Z = -halfHeight;

        if (animations)
        {
            mesh->SetAnimInstanceClass (animations->GetClass());
        }

        else
        {
            GEngine->AddOnScreenDebugMessage (-1, 5.f, FColor::Red, 
                FString::Printf (
                    TEXT ("APlayerCharacter::SwitchToRole(): No animations assigned to role %s."),
                    *Role->GetGUIName()
                )
            );
        }

        // Update the capsule.
        auto capsule            = GetCapsuleComponent();
        const auto oldHeight    = capsule->GetScaledCapsuleHalfHeight();
        capsule->RelativeLocation.Z += halfHeight - capsule->GetUnscaledCapsuleHalfHeight();
        capsule->SetCapsuleSize (radius, halfHeight);

        // Finally set the actor scale.
        const auto newScale = FVector { Role->GetActorScale() };
        SetActorScale3D (newScale);

        // Force the character to the ground.
        const auto position     = GetActorLocation();
        const auto newHeight    = capsule->GetScaledCapsuleHalfHeight();

        SetActorLocation ({ position.X, position.Y, position.Z + (newHeight - oldHeight) },
            true, nullptr, ETeleportType::TeleportPhysics);
    }

    else
    {
        GEngine->AddOnScreenDebugMessage (-1, 5.f, FColor::Red, 
            TEXT ("APlayerCharacter::SwitchToRole(): No roles set for the character.")
        );
    }
}


ABall* APlayerCharacter::FindBall() const
{
    for (TActorIterator<ABall> iterator (GetWorld()); iterator; ++iterator)
    {
        return *iterator;
    }

    GEngine->AddOnScreenDebugMessage (-1, 5.f, FColor::Red, 
        TEXT ("APlayerCharacter::FindBall(): Unable to find ball in loaded level.")
    );

    return nullptr;
}


AGoal* APlayerCharacter::FindGoal() const
{
    for (TActorIterator<AGoal> iterator { GetWorld() }; iterator; ++iterator)
    {
        auto goal = *iterator;
        if (goal->GetTeam() != Team)
        {
            return goal;
        }
    }

    // We hath failed my lord!
    auto enumAsString = EnumAsString (Team);
    GEngine->AddOnScreenDebugMessage (-1, 5.f, FColor::Red, 
        FString::Printf (
            TEXT ("APlayerCharacter::FindGoal(): Unable to find a target goal in loaded level for team %s."),
            *enumAsString
        )
    );

    return nullptr;
}


void APlayerCharacter::UpdateRoles (float deltaTime)
{
    for (auto role : Roles)
    {
        role->UpdateAbilities (this, Ball, deltaTime);
    }
}

void APlayerCharacter::ProcessCompletedAbilities()
{
    while (!CompletedAbilities.IsEmpty())
    {
        // Get the current ability.
        auto ability = EAbilityType::None;
        CompletedAbilities.Dequeue (ability);

        // An ability should never be set to an invalid type.
        check (ability != EAbilityType::None && "An ability contains an invalid EAbilityType.");

        if (ability == Ability)
        {
            Ability = EAbilityType::None;
            State   = ECharacterState::Idle;
            CompletedAbilities.Empty();
        }
    }
}


void APlayerCharacter::UpdateState (float deltaTime)
{
    switch (State)
    {
        case ECharacterState::Idle:
        case ECharacterState::Moving:
            Move (deltaTime);
            ProcessRequests (deltaTime);
            break;

        case ECharacterState::Stunned:
            ManageCooldown (deltaTime);
            ActionQueue.Empty();
            break;

        case ECharacterState::UsingAbility:
        case ECharacterState::Throwing:
            ProcessRequests (deltaTime);
            break;

        default:

            GEngine->AddOnScreenDebugMessage (-1, 5.f, FColor::Red,
                FString::Printf (
                    TEXT ("APlayerCharacter::UpdateState(): Not implemented state %s"),
                    *EnumAsString (State)
                )
            );
            break;
    }
}


void APlayerCharacter::Move (float deltaTime)
{
    if (!Input.IsZero())
    {
        // Calculate the direction we want to move. Keep in mind that all characters are axis aligned.
        const auto up           = FVector::RightVector * Input.Y;
        const auto right        = FVector::ForwardVector * Input.X;
        const auto direction    = (up + right).GetClampedToMaxSize (1.f);
        const auto speed        = CalculateMovementSpeed (*this);

        // Indicate that we're moving.
        PreviousMovement = direction * speed;
        Movement->MoveSmooth (PreviousMovement, deltaTime);
        State = ECharacterState::Moving;
    }

    else
    {
        PreviousMovement = FVector::ZeroVector;
        State = ECharacterState::Idle;
    }
}


void APlayerCharacter::ProcessRequests (float deltaTime)
{
    while (!ActionQueue.IsEmpty())
    {
        TPair<ERequestedAction, int32> request;
        ActionQueue.Dequeue (request);

        // We have to process requests differently if we're using an ability.
        if (State == ECharacterState::UsingAbility || State == ECharacterState::Throwing)
        {
            if (request.Key == ERequestedAction::DeactivateAbility)
            {
                Role->DeactivateAbility (request.Value, this, Ball);
            }
        }

        // Process all requests as normal otherwise.
        else
        {
            switch (request.Key)
            {
                case ERequestedAction::RoleChange:

                    // Simply switch the role and clamp the RoleIndex to the total number of available roles.
                    SwitchToRole (++RoleIndex % Roles.Num());
                    break;

                case ERequestedAction::ActivateAbility:

                    // Then activate the ability.
                    if (Role->ActivateAbility (request.Value, this, Ball))
                    {
                        // Set the state to using an ability and abandon all other requests.
                        State = ECharacterState::UsingAbility;

                        // Assign the ability type first.
                        Ability = Role->GetAbilityType (request.Value);
                    }

                    break;

                    case ERequestedAction::DeactivateAbility:

                        // Nothing special needs to happen here.
                        Role->DeactivateAbility (request.Value, this, Ball);
                        break;

                    default:
                        GEngine->AddOnScreenDebugMessage (-1, 5.f, FColor::Red, TEXT ("Request not handled."));
                        break;
            }
        }
    }
}


void  APlayerCharacter::ManageCooldown (float deltaTime)
{
    Cooldown = FMath::Max (0.f, Cooldown - deltaTime);

    if (Cooldown == 0.f)
    {
        State = ECharacterState::Idle;
    }
}


void APlayerCharacter::RequestAction (ERequestedAction action, int32 abilityIndex)
{
    ActionQueue.Enqueue ({ TPairInitializer<ERequestedAction, int32> { action, abilityIndex } });
}