// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"

/* 이동에 관련된 변수들을 구조체로 만들어서 클라이언트 자체적으로 이동을 처리하다가
서버에서 이 구조체에 대한 정보가 오면 기존 구조체들을 버리고 서버에서 온 정보로 업데이트하여
자연스러운 동기화 구현 */
/*
서버에서 온 정보를 정확히 파악하는 방법은 서버에서 보내는 정보에 일련번호(sequence Number)를 사용하는 방법등이 있다.
이번에는 Time 변수를 통해서 서버에서 실행된 움직임 처리의 시간을 통해서 클라이언트에서 동기화 작업을 진행한다.
즉 Time보다 오래된 클라이언트측의 Move는 지우면서 동기화한다.
*/
USTRUCT()
struct FGoKartMove
{
	GENERATED_BODY()

	UPROPERTY()
	float Throttle;
	UPROPERTY()
	float SteeringThrow;

	UPROPERTY()
	float DeltaTime;

	UPROPERTY()
	float Time;
};

USTRUCT()
struct FGoKartState
{
	GENERATED_BODY()

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	FVector Velocity;

	UPROPERTY()
	FGoKartMove LastMove;
};

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
	void SimulateMove(const FGoKartMove &Move);

	FGoKartMove CreateMove(float DeltaTime);

	void ClearAcknowledgedMoves(FGoKartMove LastMove);

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
	void Server_SendMove(FGoKartMove Move);

	FVector GetAirResistance();
	FVector GetRollingResistance();

	void UpdateLocationFromVelocity(float DeltaTime);
	void ApplyRotation(float DeltaTime, float MoveSteeringThrow);

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FGoKartState ServerState;

	FVector Velocity;

	UFUNCTION()
	void OnRep_ServerState();

	float Throttle;
	float SteeringThrow;

	TArray<FGoKartMove> UnacknowledgedMoves;
};
