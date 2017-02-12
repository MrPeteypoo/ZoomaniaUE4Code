// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Ball.generated.h"

UCLASS()
class GOAT_API ABall : public AActor
{
    GENERATED_BODY()
    
public:	

    ABall();

    UFUNCTION (BlueprintCallable, Category="Ball")
    void HeadTowards (const FVector& direction, const float force = 0.f, const float restitution = 1.f);

    UFUNCTION (BlueprintCallable, Category="Ball")
    void SpinTowards (const FVector& direction, const float force, const float restitution, 
        const float travelTime, const float curvature);

    virtual void BeginPlay() override final;
    
    virtual void Tick(float DeltaSeconds) override final;

    void EnablePhysics();

    void DisablePhysics();

    UFUNCTION (BlueprintCallable, Category="Ball")
    void DisableCurvePhysics();

    UPrimitiveComponent* const GetPhysicsComponent() const { return Collider; }

protected:

    UFUNCTION()
    void OnHit (UPrimitiveComponent* comp, AActor* other, UPrimitiveComponent* otherComp, FVector normalImpulse, const FHitResult& hit);

    void SetUpActor();

    void SetUpMesh();

    void SetUpCollider();

    void ClampVelocity();

    float CalculateCurveLength() const;

    void CurvePhysics (const float delta);

    FVector CurvePosition (const float delta) const;

    FVector CurveTangent (const float delta) const;

    void SetPhysicsProperties (bool enabled);

    UPROPERTY (EditAnywhere, BlueprintReadOnly, Category="Ball")
    UStaticMeshComponent* Mesh = nullptr;

    UPROPERTY (EditAnywhere, BlueprintReadOnly, Category="Ball")
    USphereComponent* Collider = nullptr;

    UPROPERTY (EditAnywhere, BlueprintReadOnly, Category="Ball")
    FVector initialVelocity = { 0.f, 0.f, 0.f };

    UPROPERTY (EditAnywhere, Category="Ball")
    float VelocityLimit = 3000.f;

    UPROPERTY (EditAnywhere, Category="Ball")
    float StunVelocityThreshold = 400.f;

    UPROPERTY (EditAnywhere, Category="Ball")
    float StunTime = 3.f;

    FVector cachedVelocity;

    const int32 CurveSamples = 10;
    bool CurveMode = false;

    struct BezierValues
    {
        const static int32 PointCount = 3;
        FVector Points[PointCount];
        float TravelTime = 0.f;
        float Length = 0.f;
        float CurrentTime = 0.f;
        FVector PreviousTangent;

    } BezierPath;
};