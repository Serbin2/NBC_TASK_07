// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TaskController.generated.h"

class UInputMappingContext;
class UInputAction;
class ATaskPawn;
struct FInputActionValue;

UCLASS()
class TASK07_API ATaskController : public APlayerController
{
	GENERATED_BODY()

public:
	ATaskController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void SetupInputComponent() override;

	// 에디터에서 할당할 Input Mapping Context
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	// 수평 이동 (FVector2D: X=Forward, Y=Right)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	// 카메라 회전 (FVector2D: X=Yaw, Y=Pitch)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	// 수직 이동 (float: 1=Up, -1=Down)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveUpAction;

	// 비행 모드 전환 (Digital — 키를 누르는 순간 1회 발동)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ToggleFlightAction;

	// 동체 Yaw 회전 (float: 1=Right, -1=Left)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> BodyYawAction;

	// 동체 Pitch 회전 (float: 1=NoseUp, -1=NoseDown)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> BodyPitchAction;

	// 동체 Roll 회전 (float: 1=RightWingDown, -1=LeftWingDown)
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> BodyRollAction;

private:
	void OnMove(const FInputActionValue& Value);
	void OnLook(const FInputActionValue& Value);
	void OnMoveUp(const FInputActionValue& Value);
	void OnToggleFlight(const FInputActionValue& Value);
	void OnBodyYaw(const FInputActionValue& Value);
	void OnBodyPitch(const FInputActionValue& Value);
	void OnBodyRoll(const FInputActionValue& Value);

	UPROPERTY()
	TObjectPtr<ATaskPawn> ControlledPawn;
};
