// บทที่ 9 — Service ใน C++ ที่ยกระดับมาจากต้นแบบ Blueprint (BTService_FindTarget_BP) ด้วยอัลกอริทึมเดียวกัน:
// รวบรวม TargetDummy ครั้งเดียวเมื่อ Branch เริ่มทำงาน แล้วทุก Interval หาตัวที่ใกล้ที่สุด (ไม่กรองด้วยทิศหน้า: ทิศเป็นงานของ Decorator/Task)
// เขียนคีย์ NearestTarget (Object) และ DistanceToTarget (Float)
// สถานะต่อ AI (เป้าหมายที่เป็นไปได้, เป้าหมายล่าสุด) อยู่ในหน่วยความจำจำเพาะ ไม่ใช่ตัวแปรสมาชิก (บทเรียน Node ไร้สถานะของบทที่ 8)

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FindTarget.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

class ATargetDummy;

// สถานะต่อ AI หนึ่งตัว: มี TArray จึงต้องสร้างด้วย Placement New ใน InitializeMemory และทำลายใน CleanupMemory
struct FBTFindTargetMemory
{
	TArray<TWeakObjectPtr<ATargetDummy>> Candidates;
	TWeakObjectPtr<AActor> LastNearest;
};

UCLASS()
class HAPBOOK_API UBTService_FindTarget : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_FindTarget();

	// คีย์ปลายทาง: เลือกใน Editor ตัวกรองจำกัดให้เหลือคีย์ชนิด Object(TargetDummy) และ Float
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector NearestTargetKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector DistanceToTargetKey;

	// เครื่องมือวัดเท่านั้น (ไม่มีผลต่อพฤติกรรม): บรรทัด [Nodes] Service เมื่อเป้าหมายเปลี่ยน ปิดเมื่อวัดประสิทธิภาพ
	UPROPERTY(EditAnywhere, Category = "Instrumentation")
	bool bLogTargetChanges = true;

protected:
	// แปลงตัวเลือกคีย์เป็นดัชนีจริงเมื่อโหลด Behavior Tree (ลืม = อ่านคีย์ไม่ได้โดยไม่มีข้อผิดพลาด)
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	// เมื่อ Branch เริ่มทำงาน: รวบรวมเป้าหมายที่เป็นไปได้ครั้งเดียว (เท่ากับ Receive Activation AI ของต้นแบบ)
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// ทุก Interval: วนเป้าหมายที่เป็นไปได้ เขียนสองคีย์ (เท่ากับ Receive Tick AI ของต้นแบบ)
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FBTFindTargetMemory); }
	virtual void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const override;
	virtual void CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const override;

	virtual FString GetStaticDescription() const override;
};
