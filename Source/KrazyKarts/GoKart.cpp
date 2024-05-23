// Fill out your copyright notice in the Description page of Project Settings.

#include "GoKart.h"
#include "Components/InputComponent.h"

// Sets default values
AGoKart::AGoKart()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 전진 적용
	FVector Force = GetActorForwardVector() * MaxDrivingForce * Throttle;

	// 공기 저항 적용
	Force += GetResistance();

	FVector Acceleration = Force / Mass;

	Velocity = Velocity + Acceleration * DeltaTime;

	ApplyRotation(DeltaTime);

	UpdateLocationFromVelocity(DeltaTime);
}

Fvector AGoKart::GetResistance()
{
	return -Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}

void AGoKart::ApplyRotation(float DeltaTime)
{
	// 회전 적용
	float RotationAngle = MaxDegreesPerSecond * DeltaTime * SteeringThrow;
	// FRotator로는 못하는 여러 축에 따른 회전이 가능
	// 현재 액터의 업 벡터에서 라디안 만큼 회전
	// 따라서 RotationAngle을 라디안으로 변환
	FQuat RotationDelta(GetActorUpVector(), FMath::DegreesToRadians(RotationAngle));

	// 현재 전진하는 벡터를 회전하는 FQuat만큼 회전시켜서 회전각도와 자동차의 전진 각도가 일치하게 하여 이동에 어색함을 없게끔한다.
	Velocity = RotationDelta.RotateVector(Velocity);

	AddActorWorldRotation(RotationDelta);
}

void AGoKart::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Translation = Velocity * 100 * DeltaTime;

	FHitResult Hit;
	AddActorWorldOffset(Translation, true, &Hit);

	if (Hit.IsValidBlockingHit())
	{
		Velocity = FVector::ZeroVector;
	}
}

// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGoKart::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGoKart::MoveRight);
}

void AGoKart::MoveForward(float Value)
{
	Throttle = Value;
	// Velocity = GetActorForwardVector() * 20 * Value;
}

void AGoKart::MoveRight(float Value)
{
	SteeringThrow = Value;
}
