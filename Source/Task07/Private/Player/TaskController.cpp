// Fill out your copyright notice in the Description page of Project Settings.

#include "Task07/Public/Player/TaskController.h"
#include "Task07/Public/Character/TaskPawn.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"

ATaskController::ATaskController()
{
	bShowMouseCursor = false;
}

void ATaskController::BeginPlay()
{
	Super::BeginPlay();

	// Enhanced Input — Mapping Context 등록
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (DefaultMappingContext)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ATaskController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ControlledPawn = Cast<ATaskPawn>(InPawn);
}

void ATaskController::OnUnPossess()
{
	ControlledPawn = nullptr;
	Super::OnUnPossess();
}

void ATaskController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EIC) return;

	// ── 이동 ────────────────────────────────────────────────────────
	if (MoveAction)
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATaskController::OnMove);
	}
	if (LookAction)
	{
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATaskController::OnLook);
	}
	if (MoveUpAction)
	{
		EIC->BindAction(MoveUpAction, ETriggerEvent::Triggered, this, &ATaskController::OnMoveUp);
	}

	// ── 비행 모드 전환 — Started: 키를 누르는 순간 1회만 발동 ──────
	if (ToggleFlightAction)
	{
		EIC->BindAction(ToggleFlightAction, ETriggerEvent::Started, this, &ATaskController::OnToggleFlight);
	}

	// ── 동체 회전 ───────────────────────────────────────────────────
	if (BodyYawAction)
	{
		EIC->BindAction(BodyYawAction, ETriggerEvent::Triggered, this, &ATaskController::OnBodyYaw);
	}
	if (BodyPitchAction)
	{
		EIC->BindAction(BodyPitchAction, ETriggerEvent::Triggered, this, &ATaskController::OnBodyPitch);
	}
	if (BodyRollAction)
	{
		EIC->BindAction(BodyRollAction, ETriggerEvent::Triggered, this, &ATaskController::OnBodyRoll);
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// 입력 콜백
// ─────────────────────────────────────────────────────────────────────────────

void ATaskController::OnMove(const FInputActionValue& Value)
{
	if (!ControlledPawn) return;
	ControlledPawn->AddMoveInput(Value.Get<FVector2D>());
}

void ATaskController::OnLook(const FInputActionValue& Value)
{
	// 카메라 회전은 컨트롤러 회전으로 처리 (SpringArm이 따라감)
	const FVector2D LookDelta = Value.Get<FVector2D>();
	AddYawInput(LookDelta.X);
	AddPitchInput(LookDelta.Y);
}

void ATaskController::OnMoveUp(const FInputActionValue& Value)
{
	if (!ControlledPawn) return;
	ControlledPawn->AddVerticalInput(Value.Get<float>());
}

void ATaskController::OnToggleFlight(const FInputActionValue& Value)
{
	if (!ControlledPawn) return;
	ControlledPawn->ToggleFlightMode();
}

void ATaskController::OnBodyYaw(const FInputActionValue& Value)
{
	if (!ControlledPawn) return;
	ControlledPawn->AddBodyYawInput(Value.Get<float>());
}

void ATaskController::OnBodyPitch(const FInputActionValue& Value)
{
	if (!ControlledPawn) return;
	ControlledPawn->AddBodyPitchInput(Value.Get<float>());
}

void ATaskController::OnBodyRoll(const FInputActionValue& Value)
{
	if (!ControlledPawn) return;
	ControlledPawn->AddBodyRollInput(Value.Get<float>());
}
