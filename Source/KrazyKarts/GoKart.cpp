// Fill out your copyright notice in the Description page of Project Settings.

#include "GoKart.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"

#include "DrawDebugHelpers.h"
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

FString GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "None";
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	case ROLE_Authority:
		return "Authority";
	default:
		return "ERROR";
	}
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 전진 적용
	FVector Force = GetActorForwardVector() * MaxDrivingForce * Throttle;

	// 공기 저항 적용
	Force += GetAirResistance();

	// 구름 저항 적용
	Force += GetRollingResistance();

	FVector Acceleration = Force / Mass;

	Velocity = Velocity + Acceleration * DeltaTime;

	ApplyRotation(DeltaTime);

	UpdateLocationFromVelocity(DeltaTime);

	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumText(GetLocalRole()), this, FColor::White, DeltaTime);
}

FVector AGoKart::GetAirResistance()
{
	return -Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}

FVector AGoKart::GetRollingResistance()
{
	// 월드에 설정된 중력값 알아내기
	float AccelerationDueToGravity = -GetWorld()->GetGravityZ() / 100;
	// 중력에 대항하는 NormalForce 계산 // NormalForce= M(질량)*G(중력)
	float NormalForce = Mass * AccelerationDueToGravity;
	return -Velocity.GetSafeNormal() * RollingResistanceCoefficient * NormalForce;
}

// 회전 반경을 고려한 회전 구현
void AGoKart::ApplyRotation(float DeltaTime)
{

	// 속력에 시간을 곱해서 이동 거리 계산
	// 속도 벡터와 액터의 전방 벡터를 내적하여 후진할 때 상황에도 대응할 수 있는 속력값 얻어내기
	// EX ) 전진이라면 +, 후진이라면 -값을 리턴하게된다.
	float DeltaLocation = FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime;
	// 실제 회전 각도 = 이동 거리/회전 반경*핸들의 회전각도
	float RotationAngle = DeltaLocation / MinTurningRadius * SteeringThrow;
	// FRotator로는 못하는 여러 축에 따른 회전이 가능
	// 현재 액터의 업 벡터에서 라디안 만큼 회전
	FQuat RotationDelta(GetActorUpVector(), RotationAngle);

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
	Server_MoveForward(Value);
}
void AGoKart::MoveRight(float Value)
{
	SteeringThrow = Value;
	Server_MoveRight(Value);
}

void AGoKart::Server_MoveForward_Implementation(float Value)
{
	Throttle = Value;
	// Velocity = GetActorForwardVector() * 20 * Value;
}

// MoveForward의 유효성 검증으로 치트 방지
bool AGoKart::Server_MoveForward_Validate(float Value)
{
	return FMath::Abs(Value) <= 1;
}

void AGoKart::Server_MoveRight_Implementation(float Value)
{
	SteeringThrow = Value;
}

bool AGoKart::Server_MoveRight_Validate(float Value)
{
	return FMath::Abs(Value) <= 1;
}
