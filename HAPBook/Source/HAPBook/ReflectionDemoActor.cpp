// บทที่ 4 — การนิยาม AReflectionDemoActor

#include "ReflectionDemoActor.h"
#include "UObject/UnrealType.h" // TFieldIterator, FProperty

AReflectionDemoActor::AReflectionDemoActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Constructor ห้ามแตะ World: ครั้งแรกที่ทำงานคือตอนสร้าง CDO ขณะโหลดโมดูล ซึ่งยังไม่มี World
	// IsTemplate() เป็น true สำหรับ CDO และเป็น false สำหรับอินสแตนซ์ที่วางในฉาก
	UE_LOG(LogTemp, Log, TEXT("[Reflection] Constructor: %s (IsTemplate=%s)"),
		*GetClass()->GetName(), IsTemplate() ? TEXT("true") : TEXT("false"));
}

void AReflectionDemoActor::BeginPlay()
{
	Super::BeginPlay();

	// ExcludeSuper: เฉพาะ UPROPERTY ที่ประกาศในคลาสนี้ ไม่รวมของ AActor
	// HiddenCounter ไม่ปรากฏ เพราะไม่มี UPROPERTY จึงไม่มีอยู่ในข้อมูล Reflection
	for (TFieldIterator<FProperty> It(GetClass(), EFieldIteratorFlags::ExcludeSuper); It; ++It)
	{
		const FProperty* Property = *It;
		UE_LOG(LogTemp, Log, TEXT("[Reflection] Property: %s (%s)"),
			*Property->GetName(), *Property->GetClass()->GetName());
	}
}
