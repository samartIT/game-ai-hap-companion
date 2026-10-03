// บทที่ 9 — Decorator ใน C++ ที่ขัดจังหวะได้: สืบทอดจาก UBTDecorator_BlackboardBase เพื่อให้ได้การสังเกตคีย์
// (BlackboardKey = DistanceToTarget ที่ Service เขียนทุก Interval) เงื่อนไข: มีเป้าหมาย, ระยะ < Range, และยังไม่เคยหันหาเป้าหมายนั้น
// Observer Aborts ของ Decorator ที่สืบทอดจาก UBTDecorator ตรง ๆ จะไม่มีผล เพราะไม่มีสิ่งใดเรียก ConditionalFlowAbort (ดูบทที่ 9)

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTDecorator_TargetInRange.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

// สถานะต่อ AI: ผลครั้งก่อน เพื่อบันทึกเฉพาะเมื่อผลเปลี่ยนเป็นจริง (POD: หน่วยความจำที่เติมด้วยศูนย์ใช้ได้โดยไม่ต้อง InitializeMemory)
struct FBTTargetInRangeMemory
{
	bool bWasInRange = false;
};

UCLASS()
class HAPBOOK_API UBTDecorator_TargetInRange : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTDecorator_TargetInRange();

	// ระยะที่ถือว่า "อยู่ในระยะ" (หน่วย)
	UPROPERTY(EditAnywhere, Category = "Condition")
	float Range = 800.0f;

	// คีย์ Object(TargetDummy) ที่ Service เขียน
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector NearestTargetKey;

	// คีย์ Object(TargetDummy) ที่ Task เขียนหลังหันหา: ทำให้การหันเกิดครั้งเดียวต่อเป้าหมาย
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LastFacedTargetKey;

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FBTTargetInRangeMemory); }
	virtual FString GetStaticDescription() const override;
};
