// Fill out your copyright notice in the Description page of Project Settings.

#include "GOAT.h"
#include "Ball.h"
#include "Characters/PlayerCharacter.h"


// Sets default values
ABall::ABall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetUpActor();
	SetUpMesh();
	SetUpCollider();
	RootComponent = Collider;
    Mesh->SetupAttachment (RootComponent);
}

void ABall::SetUpActor()
{
	Tags.Add ("Ball");
}

void ABall::SetUpMesh()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	auto visualMesh = ConstructorHelpers::FObjectFinder<UStaticMesh> (TEXT ("/Engine/EngineMeshes/Sphere")).Object;
	if (visualMesh)
	{
		Mesh->SetStaticMesh (visualMesh);
	}

	Mesh->SetCollisionProfileName ("NoCollision");
}

void ABall::SetUpCollider()
{
	Collider = CreateDefaultSubobject<USphereComponent> (TEXT ("Collider"));
    
    Collider->SetSimulatePhysics (true);
    Collider->SetNotifyRigidBodyCollision (true);
	Collider->SetCollisionProfileName ("PhysicsActor");
	Collider->SetConstraintMode (EDOFMode::XYPlane);
	Collider->OnComponentHit.AddDynamic (this, &ABall::OnHit);
}

// Called when the game starts or when spawned
void ABall::BeginPlay()
{
	Super::BeginPlay();
	
	Collider->SetPhysicsLinearVelocity (initialVelocity);
	ClampVelocity();
    EnablePhysics();
    Collider->SetMassOverrideInKg (NAME_None, 1.f);
}

// Called every frame
void ABall::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

    if (CurveMode)
    {
        CurvePhysics (DeltaTime);
    }

    else
    {
        ClampVelocity();
    }
	
    cachedVelocity = Collider->GetPhysicsLinearVelocity();
}

void ABall::OnHit (UPrimitiveComponent* comp, AActor* other, UPrimitiveComponent* otherComp, FVector normalImpulse, const FHitResult& hit)
{
    if (CurveMode)
    {
        DisableCurvePhysics();
    }

	if (other && other->ActorHasTag("Player"))
	{
		auto player = static_cast<APlayerCharacter*>(other);
		if (cachedVelocity.SizeSquared() > FMath::Square (StunVelocityThreshold) &&
            (player->GetState() != ECharacterState::UsingAbility))
		{
			player->Stun (StunTime);
			//GEngine->AddOnScreenDebugMessage (-1, 15.f, FColor::White, TEXT ("ABall::OnHit() stunning player."));
		}
	}
}

void ABall::HeadTowards (const FVector& direction, const float force, const float restitution)
{
    if (CurveMode)
    {
        CurveMode = false;
    }

	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::White, FString::Printf(TEXT("ABall::Kick(), %s, %f, %f"), *direction.ToCompactString(), force, restitution));
	const auto currentSpeed = Collider->GetPhysicsLinearVelocity().Size();

	// Ensure force and restitution aren't below zero.
	const auto additionalForce = direction * FMath::Max(force, 0.f);
	const auto maintainedVelocity = direction * (FMath::Max(restitution, 0.f) * currentSpeed);

	const auto kickVelocity = additionalForce + maintainedVelocity;
	Collider->SetPhysicsLinearVelocity (kickVelocity);
}

void ABall::SpinTowards (const FVector& direction, const float force, const float restitution, 
        const float travelTime, const float curvature)
{
    const auto distance = (Collider->GetPhysicsLinearVelocity().Size() * restitution + force) * travelTime;
    const auto curveOffset = distance * curvature;

    const auto start = GetActorLocation();
    const auto end = start + direction * distance;
    const auto mid = FMath::Lerp (start, end, 0.5f);

    const auto angle = curvature > 0.f ? 90.f : -90.f;
    const auto rotatedDirection = direction.RotateAngleAxis (angle, FVector::UpVector);
    const auto second = mid + rotatedDirection * FMath::Abs (curveOffset);

    CurveMode = true;

    BezierPath = BezierValues { };
    BezierPath.Points[0] = start;
    BezierPath.Points[1] = second;
    BezierPath.Points[2] = end;
    BezierPath.TravelTime = travelTime;
    BezierPath.CurrentTime = 0.f;

    BezierPath.Length = CalculateCurveLength();
    BezierPath.PreviousTangent = CurveTangent (0.f);
}

void ABall::EnablePhysics()
{
    SetPhysicsProperties (true);
}

void ABall::DisablePhysics()
{
    SetPhysicsProperties (false);
}

void ABall::ClampVelocity()
{
    const auto velocity = Collider->GetPhysicsLinearVelocity();
	const auto speedSqr = velocity.SizeSquared();
	
	// Only clamp the velocity if we need to.
	if (speedSqr > FMath::Square (VelocityLimit))
	{
		const auto direction = velocity.GetSafeNormal();
		const auto clampedVelocity = direction * VelocityLimit;
	
        Collider->SetPhysicsLinearVelocity (clampedVelocity);
	}
}

float ABall::CalculateCurveLength() const
{
    auto accumulator = 0.f;
    auto previous = BezierPath.Points[0];

    // Begin sampling at the desired frequency.
    for (int32 i { 1 }; i <= CurveSamples; ++i)
    {
        // Obtain the current position on the curve.
        const auto current = CurvePosition (i / static_cast<float> (CurveSamples));

        // Add the magnitude onto the accumulator.
        accumulator += (current - previous).Size();

        // Move onto the next sample.
        previous = current;
    }

    return accumulator;
}

void ABall::CurvePhysics (const float delta)
{
    const auto arcDistance = BezierPath.Length / BezierPath.TravelTime * delta;
    const auto tangentSpeed = BezierPath.PreviousTangent.Size();
    const auto inverseTangentalSpeed = 1.f / tangentSpeed;

    BezierPath.CurrentTime += inverseTangentalSpeed * arcDistance;

    if (BezierPath.CurrentTime >= 1.f)
    {
        Collider->SetPhysicsLinearVelocity (BezierPath.PreviousTangent);
        DisableCurvePhysics();
    }

    else
    {
        const auto tangent = CurveTangent (BezierPath.CurrentTime);

        Collider->SetPhysicsLinearVelocity (tangent);
        BezierPath.PreviousTangent = tangent;
    }
}

FVector ABall::CurvePosition (const float delta) const
{
    const auto& p0 = BezierPath.Points[0];
    const auto& p1 = BezierPath.Points[1];
    const auto& p2 = BezierPath.Points[2];

    const auto inverseDelta = 1.f - delta;
    const auto inverseDeltaSqr = FMath::Square (inverseDelta);
    const auto deltaSqr = FMath::Square (delta);

    const auto first = inverseDeltaSqr * p0;
    const auto second = 2.f * inverseDelta * delta * p1;
    const auto third = deltaSqr * p2;

    return first + second + third;
}

FVector ABall::CurveTangent (const float delta) const
{
    const auto& p0 = BezierPath.Points[0];
    const auto& p1 = BezierPath.Points[1];
    const auto& p2 = BezierPath.Points[2];

    const auto inverseDelta = 1.f - delta;

    const auto first = 2.f * inverseDelta * (p1 - p0);
    const auto second = 2.f * delta * (p2 - p1);

    return first + second;
}

void ABall::DisableCurvePhysics()
{
    CurveMode = false;
    //GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::White, TEXT("ABall::DisableCurvePhysics()."));
}

void ABall::SetPhysicsProperties (bool enabled)
{
    Collider->SetMassOverrideInKg (NAME_None, 1.f);
    Collider->SetSimulatePhysics (enabled);
    Collider->SetNotifyRigidBodyCollision (enabled);

    if (enabled)
    {
        Collider->SetCollisionProfileName ("PhysicsActor");
    }
    else
    {
        Collider->SetCollisionProfileName ("NoCollision");
    }
}
