// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"

UCLASS()
class KRAZYKARTS_API AGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoKart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

private:
	// The mass of the car (kg).
	UPROPERTY(EditAnywhere)
	float Mass = 1000;

	// The force Applied to the car when the throttle is fully down(N).
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000;

	// 초당 최대 회전 각도

	UPROPERTY(EditAnywhere)
	float MaxDegreesPerSecond = 90;

	// 저항계수 // 높아질수록 저항이 세진다 // 단위 : Kg/m
	UPROPERTY(EditAnywhere)
	float MaxDegreesPerSecond = 16;

	void MoveForward(float Value);
	void MoveRight(float Value);

	FVector Velocity;

	float Throttle;
	float SteeringThrow;

	Fvector GetResistance();

	void UpdateLocationFromVelocity(float DeltaTime);
	void ApplyRotation(float DeltaTime);
};
