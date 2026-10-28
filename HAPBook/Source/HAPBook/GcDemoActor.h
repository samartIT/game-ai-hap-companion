// บทที่ 4 — Actor สาธิต Garbage Collection: การอ้างอิงที่ GC ติดตาม (UPROPERTY) กับที่ไม่ติดตาม (TWeakObjectPtr)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GcDemoActor.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

class UGcDemoData;

UCLASS()
class HAPBOOK_API AGcDemoActor : public AActor
{
	GENERATED_BODY()

public:
	AGcDemoActor();

	// Strong Reference: UPROPERTY + TObjectPtr (บทที่ 2 หัวข้อ TObjectPtr) ทำให้ GC เห็นและรักษาวัตถุไว้
	UPROPERTY(VisibleAnywhere, Category = "GC")
	TObjectPtr<UGcDemoData> TrackedData;

	// Weak Reference: ไม่มี UPROPERTY และไม่รั้งวัตถุไว้ แต่ตรวจสอบได้อย่างปลอดภัยด้วย IsValid()
	// ไม่มีที่ใดถือ Strong Reference ไปยังวัตถุนี้ GC จึงเก็บกวาดได้ในรอบถัดไป
	TWeakObjectPtr<UGcDemoData> UntrackedWatcher;

	// หน่วงเวลาก่อนรายงานผลหลังการเก็บกวาด (GC ทำงานในเฟรมถัดไปหลัง ForceGarbageCollection)
	UPROPERTY(EditAnywhere, Category = "GC")
	float ReportDelaySeconds = 2.0f;

protected:
	virtual void BeginPlay() override;

private:
	void ReportAfterCollection();

	FTimerHandle ReportTimerHandle;
};
