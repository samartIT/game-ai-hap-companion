// บทที่ 14 — การนิยาม ABenchmarkActorBase

#include "BenchmarkActorBase.h"
#include "Components/SceneComponent.h"

ABenchmarkActorBase::ABenchmarkActorBase()
{
	// ทุกสถาปัตยกรรมต้อง Tick: Event Tick ของ Blueprint
	// อาศัยกลไก Tick เดียวกันนี้
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

void ABenchmarkActorBase::RunWork()
{
	FVector Acc = FVector::ZeroVector;
	for (int32 i = 0; i < Iterations; ++i)
	{
		const FVector V(i * 1.0, i * 2.0, i * 3.0);
		Acc = Acc + V.GetSafeNormal() * 2.0;
	}
	// เก็บในสมาชิก ไม่ใช่ตัวแปรเฉพาะที่:
	// Compiler จึงตัดลูปทิ้งไม่ได้
	Result = Acc.Size();
}

void ABenchmarkActorBase::Tick(float DeltaSeconds)
{
	// AActor::Tick เรียก Event Tick ของคลาสลูกแบบ Blueprint
	Super::Tick(DeltaSeconds);
	// ว่างโดยเจตนา: หากเรียก RunWork() ที่นี่ ทุกแบบจะรัน
	// ภาระงานใน C++ และแบบ Blueprint จะรันซ้ำอีกรอบ
}
