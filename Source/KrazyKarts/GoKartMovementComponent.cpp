// Fill out your copyright notice in the Description page of Project Settings.

#include "GoKartMovementComponent.h"
#include "GameFramework/GameStateBase.h"
// Sets default values for this component's properties
UGoKartMovementComponent::UGoKartMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UGoKartMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

// Called every frame
void UGoKartMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwnerRole() == ROLE_AutonomousProxy || GetOwnerRole() == ROLE_Authority)
	{
		LastMove = CreateMove(DeltaTime);
		SimulateMove(LastMove);
	}
	// ...
}

void UGoKartMovementComponent::SimulateMove(const FGoKartMove &Move)
{
	// 전진 적용
	FVector Force = GetOwner()->GetActorForwardVector() * MaxDrivingForce * Move.Throttle;

	// 공기 저항 적용
	Force += GetAirResistance();

	// 구름 저항 적용
	Force += GetRollingResistance();

	FVector Acceleration = Force / Mass;

	Velocity = Velocity + Acceleration * Move.DeltaTime;

	ApplyRotation(Move.DeltaTime, Move.SteeringThrow);

	UpdateLocationFromVelocity(Move.DeltaTime);
}

FGoKartMove UGoKartMovementComponent::CreateMove(float DeltaTime)
{
	FGoKartMove Move;
	Move.DeltaTime = DeltaTime;
	Move.SteeringThrow = SteeringThrow;
	Move.Throttle = Throttle;
	Move.Time = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();

	return Move;
}

FVector UGoKartMovementComponent::GetAirResistance()
{
	return -Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}

FVector UGoKartMovementComponent::GetRollingResistance()
{
	// 월드에 설정된 중력값 알아내기
	float AccelerationDueToGravity = -GetWorld()->GetGravityZ() / 100;
	// 중력에 대항하는 NormalForce 계산 // NormalForce= M(질량)*G(중력)
	float NormalForce = Mass * AccelerationDueToGravity;
	return -Velocity.GetSafeNormal() * RollingResistanceCoefficient * NormalForce;
}

// 회전 반경을 고려한 회전 구현
void UGoKartMovementComponent::ApplyRotation(float DeltaTime, float MoveSteeringThrow)
{

	// 속력에 시간을 곱해서 이동 거리 계산
	// 속도 벡터와 액터의 전방 벡터를 내적하여 후진할 때 상황에도 대응할 수 있는 속력값 얻어내기
	// EX ) 전진이라면 +, 후진이라면 -값을 리턴하게된다.
	float DeltaLocation = FVector::DotProduct(GetOwner()->GetActorForwardVector(), Velocity) * DeltaTime;
	// 실제 회전 각도 = 이동 거리/회전 반경*핸들의 회전각도
	float RotationAngle = DeltaLocation / MinTurningRadius * MoveSteeringThrow;
	// FRotator로는 못하는 여러 축에 따른 회전이 가능
	// 현재 액터의 업 벡터에서 라디안 만큼 회전
	FQuat RotationDelta(GetOwner()->GetActorUpVector(), RotationAngle);

	// 현재 전진하는 벡터를 회전하는 FQuat만큼 회전시켜서 회전각도와 자동차의 전진 각도가 일치하게 하여 이동에 어색함을 없게끔한다.
	Velocity = RotationDelta.RotateVector(Velocity);

	GetOwner()->AddActorWorldRotation(RotationDelta);
}

void UGoKartMovementComponent::UpdateLocationFromVelocity(float DeltaTime)
{
	FVector Translation = Velocity * 100 * DeltaTime;

	FHitResult Hit;
	GetOwner()->AddActorWorldOffset(Translation, true, &Hit);

	if (Hit.IsValidBlockingHit())
	{
		Velocity = FVector::ZeroVector;
	}
}
