// บทที่ 9 — Task ใน C++ ที่จบในเฟรมเดียว: ตั้งจุดสนใจของ Controller ที่ NearestTarget, หมุนร่างให้หันหา,
// เขียน LastFacedTarget (เพื่อให้ Decorator Target In Range เป็นเท็จสำหรับเป้าหมายนี้) แล้วคืน Succeeded
// ไม่มีสถานะต่อ AI จึงไม่ต้องการหน่วยความจำจำเพาะ

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FaceTarget.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

UCLASS()
class HAPBOOK_API UBTTask_FaceTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FaceTarget();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector NearestTargetKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LastFacedTargetKey;

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
