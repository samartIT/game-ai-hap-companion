// บทที่ 9 — การนิยาม ATargetDummy

#include "TargetDummy.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "UObject/ConstructorHelpers.h"

ATargetDummy::ATargetDummy()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));   // ไม่กระทบ NavMesh และไม่บังเส้นทาง

	// รูปทรงพื้นฐานของเอนจิ้น: ไม่ต้องสร้าง Asset เพิ่ม
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CylinderMesh.Object);
	}

	Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Label"));
	Label->SetupAttachment(Mesh);
	Label->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));
	Label->SetHorizontalAlignment(EHTA_Center);
	Label->SetWorldSize(40.0f);
}

void ATargetDummy::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (Label != nullptr)
	{
		Label->SetText(FText::FromName(DummyName));
	}
}
