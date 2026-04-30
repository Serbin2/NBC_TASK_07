// Fill out your copyright notice in the Description page of Project Settings.

#include "Task07/Public/Character/TaskPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"

ATaskPawn::ATaskPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->InitCapsuleSize(42.f, 96.f);
	SetRootComponent(CapsuleComponent);

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	SkeletalMeshComponent->SetupAttachment(CapsuleComponent);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 600.f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	SkeletalMeshComponent->SetSimulatePhysics(false);
}

void ATaskPawn::BeginPlay()
{
	Super::BeginPlay();
}

void ATaskPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateMovement(DeltaTime);
}

void ATaskPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

//	입력 누적
void ATaskPawn::AddMoveInput(FVector2D MoveInput)
{
	PendingMoveInput += MoveInput;
}

void ATaskPawn::AddVerticalInput(float VerticalInput)
{
	PendingVerticalInput += VerticalInput;
}

void ATaskPawn::ToggleFlightMode()
{
	bFlightMode = !bFlightMode;

	// 비행 모드 진입 시 낙하 중이면 수직 속도를 0으로 정지
	if (bFlightMode && Velocity.Z < 0.f)
	{
		Velocity.Z = 0.f;
	}
}

void ATaskPawn::AddBodyYawInput(float YawInput)
{
	PendingYawInput += YawInput;
}

void ATaskPawn::AddBodyPitchInput(float PitchInput)
{
	PendingPitchInput += PitchInput;
}

void ATaskPawn::AddBodyRollInput(float RollInput)
{
	PendingRollInput += RollInput;
}

//	바닥 감지
bool ATaskPawn::CheckGrounded() const
{
	const float HalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();

	// 캡슐 중심 → (HalfHeight + GroundCheckDistance) 아래로 LineTrace
	const FVector TraceStart = GetActorLocation();
	const FVector TraceEnd   = TraceStart - FVector(0.f, 0.f, HalfHeight + GroundCheckDistance);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_WorldStatic, Params))
	{
		// Hit.Distance: TraceStart 기준 거리, 캡슐 하단까지는 HalfHeight
		const float DistFromBottom = Hit.Distance - HalfHeight;
		return DistFromBottom <= GroundCheckDistance;
	}
	return false;
}


void ATaskPawn::UpdateMovement(float DeltaTime)
{
	/// 회전 
	// Yaw: 월드 Z축 기준 회전 (기울어진 상태에서도 좌우 회전이 직관적)
	if (!FMath::IsNearlyZero(PendingYawInput))
	{
		AddActorWorldRotation(FRotator(0.f, PendingYawInput * YawRotationSpeed * DeltaTime, 0.f));
	}

	// Pitch, Roll: 로컬 축 기준 회전
	if (!FMath::IsNearlyZero(PendingPitchInput) || !FMath::IsNearlyZero(PendingRollInput))
	{
		AddActorLocalRotation(FRotator(
			PendingPitchInput * PitchRotationSpeed * DeltaTime,
			0.f,
			PendingRollInput  * RollRotationSpeed  * DeltaTime
		));
	}

	// 바닥 확인
	bIsGrounded = CheckGrounded();

	/// 이동 
	// 수평 이동은 항상 수평면 위에서
	const float   PawnYaw = GetActorRotation().Yaw;
	const FRotator YawOnly(0.f, PawnYaw, 0.f);
	const FVector  Forward = FRotationMatrix(YawOnly).GetUnitAxis(EAxis::X);
	const FVector  Right   = FRotationMatrix(YawOnly).GetUnitAxis(EAxis::Y);

	// 수평 가속 / 감속
	const bool bHasHorizInput = !PendingMoveInput.IsNearlyZero();
	if (bHasHorizInput)
	{
		const FVector DesiredDir = (Forward * PendingMoveInput.X + Right * PendingMoveInput.Y).GetSafeNormal2D();
		Velocity += DesiredDir * HorizontalAcceleration * DeltaTime;
	}

	//	속도 계산
	
	const float HorizSpeed = FVector2D(Velocity.X, Velocity.Y).Size();
	if (HorizSpeed > KINDA_SMALL_NUMBER)	//	KINDA_SMALL_NUMBER = 부동소수점에서 충분히 작은 값 ( Near 0 )
	{
		float DragAmount = 0.f;
		if (!bHasHorizInput)	DragAmount = HorizontalDeceleration;
		float NewSpeed   = FMath::Max(0.f, HorizSpeed - DragAmount * DeltaTime);
		if ( NewSpeed > MaxHorizontalSpeed )	NewSpeed = MaxHorizontalSpeed;
		const float Scale      = NewSpeed / HorizSpeed;
		Velocity.X *= Scale;
		Velocity.Y *= Scale;
	}
	

	// 수직 이동 + 중력 
	// 비행 모드: 중력 없음
	const bool bApplyGravity = !bFlightMode && (!bIsGrounded || Velocity.Z > 0.f);
	if (bApplyGravity)
	{
		Velocity.Z -= GravityStrength * DeltaTime;
	}
	
	if (!FMath::IsNearlyZero(PendingVerticalInput))
	{
		Velocity.Z += PendingVerticalInput * VerticalAcceleration * DeltaTime;
	}
	
	if (bIsGrounded && Velocity.Z < 0.f)
	{
		Velocity.Z = 0.f;
	}

	// 수직 속도 클램프
	Velocity.Z = FMath::Clamp(Velocity.Z, -MaxVerticalSpeed, MaxVerticalSpeed);

	FVector GroundVelocity = Velocity;
	if (bIsGrounded)
	{
		GroundVelocity = Velocity * 0.7f;
	}
	
	// 결과
	FHitResult Hit;
	AddActorWorldOffset(GroundVelocity * DeltaTime, true, &Hit);

	if (Hit.IsValidBlockingHit())
	{	//	기능이 원하는대로 동작하진 않았지만 시간관계상 다음에 다시 도전하는걸로
		// 충돌 법선 방향 속도 성분 제거
		const float VDotN = FVector::DotProduct(Velocity, Hit.Normal);
		if (VDotN < 0.f)
		{
			Velocity -= VDotN * Hit.Normal;
		}

		// 바닥 충돌 (법선 Z > 0.7 ≈ 45° 이내 경사)
		if (Hit.Normal.Z > 0.7f)
		{
			bIsGrounded = true;
			Velocity.Z  = FMath::Max(0.f, Velocity.Z);
		}
	}

	// 입력 초기화
	PendingMoveInput     = FVector2D::ZeroVector;
	PendingVerticalInput = 0.f;
	PendingYawInput      = 0.f;
	PendingPitchInput    = 0.f;
	PendingRollInput     = 0.f;
}
