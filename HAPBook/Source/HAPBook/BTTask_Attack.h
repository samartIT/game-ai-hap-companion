// บทที่ 13 — Task การโจมตีระยะประชิดของศัตรู (C++): จบในเฟรมเดียว ไม่มีสถานะต่อ AI จึงไม่ต้องการหน่วยความจำจำเพาะ
// เป้าหมายอยู่ในระยะโจมตี: หันร่างหาเป้าหมาย แล้วส่งความเสียหายผ่านท่อความเสียหายของบทที่ 10
// (Instigator = Controller ของศัตรู, Damage Causer = ร่างของศัตรู) แล้วคืน Succeeded
// นอกระยะหรือไม่มีเป้าหมาย: คืน Failed (Selector ไปยัง Branch Chase ทางขวา)
// ช่วงพักระหว่างการโจมตีเป็นของ Decorator Cooldown บน Branch Attack ของ BT_Enemy_Combat ไม่ใช่ของ Task นี้

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Attack.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

class UDamageType;

UCLASS()
class HAPBOOK_API UBTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Attack();

	// เป้าหมาย: ค่าเริ่มต้นคือคีย์ TargetActor ของบทที่ 11
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	// ระยะโจมตี = ช่องว่างระหว่างแคปซูลของทั้งสอง
	// วัดแบบเดียวกับ Acceptable Radius ของ Move To
	UPROPERTY(EditAnywhere, Category = "Attack",
		meta = (ClampMin = "0.0", Units = "cm"))
	float AttackRange = 200.0f;

	// ความเสียหายต่อการโจมตีหนึ่งครั้ง
	UPROPERTY(EditAnywhere, Category = "Attack",
		meta = (ClampMin = "0.0"))
	float Damage = 10.0f;

	// ป้ายกำกับของเหตุ (ค่าเริ่มต้น UDamageType_Melee)
	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<UDamageType> DamageTypeClass;

protected:
	virtual void InitializeFromAsset(
		UBehaviorTree& Asset) override;
	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
