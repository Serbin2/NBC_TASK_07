// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TaskPawn.generated.h"

UCLASS()
class TASK07_API ATaskPawn : public APawn
{
	GENERATED_BODY()

public:
	ATaskPawn();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 수평 이동 입력 누적 (Controller 호출)
	void AddMoveInput(FVector2D MoveInput);
	// 수직 이동 입력 누적 (Controller 호출)
	void AddVerticalInput(float VerticalInput);
	// 비행 모드 전환 (Controller 호출)
	void ToggleFlightMode();
	// 동체 Yaw 회전 입력 누적 (Controller 호출)
	void AddBodyYawInput(float YawInput);
	// 동체 Pitch 회전 입력 누적 (Controller 호출)
	void AddBodyPitchInput(float PitchInput);
	// 동체 Roll 회전 입력 누적 (Controller 호출)
	void AddBodyRollInput(float RollInput);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Task")
	TObjectPtr<class UCapsuleComponent> CapsuleComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Task")
	TObjectPtr<class USkeletalMeshComponent> SkeletalMeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Task")
	TObjectPtr<class USpringArmComponent> SpringArm;

	UPROPERTY(EditDefaultsOnly, Category = "Task")
	TObjectPtr<class UCameraComponent> Camera;

	// 비행 모드 활성 여부 (비행 중 중력 없음)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bFlightMode = false;

	// 수평 최대 속도
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float MaxHorizontalSpeed = 600.f;

	// 수직 최대 속도
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float MaxVerticalSpeed = 400.f;

	// 수평 가속도
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float HorizontalAcceleration = 1500.f;

	// 수직 가속도
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float VerticalAcceleration = 1200.f;

	// 입력 없을 때 수평 감속도
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float HorizontalDeceleration = 1000.f;

	// 중력 강도
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float GravityStrength = 980.f;

	// 바닥 감지 거리 (캡슐 하단 기준)
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float GroundCheckDistance = 10.f;

	// Yaw 회전 속도 (deg/s)
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Rotation")
	float YawRotationSpeed = 90.f;

	// Pitch 회전 속도 (deg/s)
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Rotation")
	float PitchRotationSpeed = 60.f;

	// Roll 회전 속도 (deg/s)
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Rotation")
	float RollRotationSpeed = 60.f;

private:
	FVector    Velocity             = FVector::ZeroVector;
	FVector2D  PendingMoveInput     = FVector2D::ZeroVector;
	float      PendingVerticalInput = 0.f;
	float      PendingYawInput      = 0.f;
	float      PendingPitchInput    = 0.f;
	float      PendingRollInput     = 0.f;
	bool       bIsGrounded          = false;

	void UpdateMovement(float DeltaTime);
	bool CheckGrounded() const;
};
