// Fill out your copyright notice in the Description page of Project Settings.


#include "Collider.h"
#include "Components/SphereComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ColliderMovementComponent.h"

// Sets default values
ACollider::ACollider()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(SphereComponent);

	SphereComponent->InitSphereRadius(40.0f);
	SphereComponent->SetCollisionProfileName(TEXT("Pawn"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(GetRootComponent());

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshComponentAsset(TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere'"));
	if (MeshComponentAsset.Succeeded())
	{
		MeshComponent->SetStaticMesh(MeshComponentAsset.Object);
		MeshComponent->SetRelativeLocation(FVector(0.f, 0.f, -40.f));
		MeshComponent->SetWorldScale3D(FVector(0.8f, 0.8f, 0.8f));
	}

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
	SpringArmComponent->TargetArmLength = 400.f;
	SpringArmComponent->bEnableCameraLag = true;
	SpringArmComponent->CameraLagSpeed = 3.0f;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	//CameraComponent->SetRelativeLocation(FVector(-300.f, 0.f, 300.f));
	//CameraComponent->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
	OurMovementComponent = CreateDefaultSubobject<UColliderMovementComponent>(TEXT("OurMovementComponent"));
	OurMovementComponent->UpdatedComponent = RootComponent;

	CameraInput = FVector2D(0.f, 0.f);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ACollider::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACollider::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += CameraInput.X;
	//NewRotation.Pitch += CameraInput.Y;
	SetActorRotation(NewRotation);

	FRotator NewSpringArmRotation = SpringArmComponent->GetComponentRotation();
	NewSpringArmRotation.Pitch = FMath::Clamp(NewSpringArmRotation.Pitch += CameraInput.Y, -80.f, -15.f);
	SpringArmComponent->SetWorldRotation(NewSpringArmRotation);

}

// Called to bind functionality to input
void ACollider::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACollider::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACollider::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("CameraPitch"), this, &ACollider::PitchCamera);
	PlayerInputComponent->BindAxis(TEXT("CameraYaw"), this, &ACollider::YawCamera);

}

UPawnMovementComponent* ACollider::GetMovementComponent() const
{
	return OurMovementComponent;
}

void ACollider::MoveForward(float InputValue)
{
	FVector Forward = GetActorForwardVector();
	//AddMovementInput(InputValue * Forward);
	if (OurMovementComponent)
	{
		OurMovementComponent->AddInputVector(Forward * InputValue * 10);
	}

}
void ACollider::MoveRight(float InputValue) 
{
	FVector Right = GetActorRightVector();
	if (OurMovementComponent)
	{
		OurMovementComponent->AddInputVector(Right * InputValue);
	}
}

void ACollider::YawCamera(float AxisValue)
{
	CameraInput.X = AxisValue;
}

void ACollider::PitchCamera(float AxisValue)
{
	CameraInput.Y = AxisValue;
}


