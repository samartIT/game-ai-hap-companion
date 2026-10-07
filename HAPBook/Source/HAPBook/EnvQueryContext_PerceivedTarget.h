// บทที่ 12 — Context ของเป้าหมายที่ศัตรูรับรู้ได้ขณะนี้
// ใช้เฉพาะในแบบฝึกหัดออกแบบของบทที่ 12 (ย้ายจุดยืนเทียบกับเป้าหมายที่เคลื่อนที่)
// ไม่มี Query หรือ Behavior Tree ใดในชุดโค้ดประกอบอ้างถึงคลาสนี้
// อ่านคีย์ TargetActor ใน Blackboard ของ Querier (ตัวรับสิ่งเร้าของบทที่ 11
// เขียนคีย์นี้เมื่อเห็นเป้าหมายที่เป็น Hostile และล้างเมื่อหาย)
// Querier ที่ไม่มี Blackboard (EQS Testing Pawn) ได้ตัวช่วยสาธิตของคลาสแม่

#pragma once

#include "CoreMinimal.h"
#include "EnvQueryContext_Target.h"
// ต้องเป็น #include บรรทัดสุดท้ายเสมอ
#include "EnvQueryContext_PerceivedTarget.generated.h"

UCLASS()
class HAPBOOK_API UEnvQueryContext_PerceivedTarget
	: public UEnvQueryContext_Target
{
	GENERATED_BODY()

public:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance,
		FEnvQueryContextData& ContextData) const override;
};
