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

	// 최대로 꺾은 자동차의 회전 반경의 최소 반지름 // 단위 : m
	UPROPERTY(EditAnywhere)
	float MinTurningRadius = 10;

	// 공기 저항계수 // 높아질수록 저항이 세진다 // 단위 : Kg/m
	UPROPERTY(EditAnywhere)
	float DragCoefficient = 16;

	// 구름 저항 계수// 높아질수록 저항이 더 커진다.
	UPROPERTY(EditAnywhere)
	float RollingResistanceCoefficient = 0.015;

	void MoveForward(float Value);
	void MoveRight(float Value);

	// ServerRPC임을 명시
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveForward(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveRight(float Value);

	FVector Velocity;

	float Throttle;
	float SteeringThrow;

	FVector GetAirResistance();
	FVector GetRollingResistance();

	void UpdateLocationFromVelocity(float DeltaTime);
	void ApplyRotation(float DeltaTime);
};
