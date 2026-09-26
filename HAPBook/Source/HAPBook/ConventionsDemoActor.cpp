// บทที่ 2 — การนิยาม AConventionsDemoActor

#include "ConventionsDemoActor.h"
#include "ConventionsData.h"

AConventionsDemoActor::AConventionsDemoActor()
{
	PrimaryActorTick.bCanEverTick = false;
	Scores = { 80, 90, 100 };
}

int32 AConventionsDemoActor::GetScoreSum() const
{
	int32 Sum = 0;
	for (const int32 Score : Scores)
	{
		Sum += Score;
	}
	return Sum;
}

void AConventionsDemoActor::BeginPlay()
{
	Super::BeginPlay();

	// บรรทัดที่ 1: ค่าของสมาชิกพื้นฐาน (FString ใช้ *DisplayName เพื่อส่งเป็น TCHAR*)
	UE_LOG(LogTemp, Log, TEXT("[Conventions] Name: %s | Health: %d | Scores: %d items (sum %d)"),
		*DisplayName, Health, Scores.Num(), GetScoreSum());

	// สร้าง UObject ผ่าน NewObject โดยให้ Actor นี้เป็น Outer (ห้ามใช้ new กับ UObject)
	Data = NewObject<UConventionsData>(this);

	// ชื่อคลาสใน Reflection ไม่มีคำนำหน้า จึงประกอบคำนำหน้า U กลับเข้าไปด้วย GetPrefixCPP()
	const FString ClassName = FString::Printf(TEXT("%s%s"),
		Data->GetClass()->GetPrefixCPP(), *Data->GetClass()->GetName());

	// บรรทัดที่ 2: ยืนยันว่าวัตถุถูกสร้างและค่าเริ่มต้นของ UPROPERTY ถูกอ่านได้
	UE_LOG(LogTemp, Log, TEXT("[Conventions] Data object created via NewObject: %s (Level=%d)"),
		*ClassName, Data->Level);
}
