// บทที่ 2 — คลาสข้อมูลประกอบสำหรับสาธิตการสร้าง UObject ด้วย NewObject
// คำนำหน้า U: คลาสที่สืบทอดจาก UObject แต่ไม่ใช่ Actor (วางในฉากโดยตรงไม่ได้)

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ConventionsData.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

UCLASS()
class HAPBOOK_API UConventionsData : public UObject
{
	GENERATED_BODY()

public:
	// int32 แทน int เพื่อให้ขนาดข้อมูลคงที่ทุกแพลตฟอร์ม (Platform Agnostic)
	UPROPERTY(EditAnywhere, Category = "Conventions")
	int32 Level = 2;
};
