// บทที่ 9 — การนิยาม ATargetDummy (บทที่ 11 เพิ่มส่วนที่กำกับว่า "บทที่ 11")

#include "TargetDummy.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
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

	// บทที่ 11: Component แหล่งสิ่งเร้า (การลงทะเบียน Sense อยู่ใน BeginPlay)
	StimuliSource =
		CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(
			TEXT("StimuliSource"));
}

void ATargetDummy::BeginPlay()
{
	Super::BeginPlay();

	// บทที่ 11: ประกาศตนเป็นแหล่งสิ่งเร้าของ Sight — Listener ที่มีอยู่ได้ Query ใหม่ทันที Listener ที่เกิดทีหลังพบหุ่นในรายการแหล่งที่ลงทะเบียนแล้ว
	if (StimuliSource != nullptr)
	{
		StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
	}
}

void ATargetDummy::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (Label != nullptr)
	{
		Label->SetText(FText::FromName(DummyName));
	}
}
