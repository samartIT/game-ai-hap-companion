// บทที่ 12 — Context ของเป้าหมายที่ศัตรูรับรู้ได้ขณะนี้
// อ่านคีย์ TargetActor ใน Blackboard ของผู้ถาม (ตัวรับสิ่งเร้าของบทที่ 11
// เขียนคีย์นี้เมื่อเห็นเป้าหมายที่เป็น Hostile และล้างเมื่อหาย)
// ผู้ถามที่ไม่มี Blackboard (EQS Testing Pawn) ได้ตัวช่วยสาธิตของคลาสแม่

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
