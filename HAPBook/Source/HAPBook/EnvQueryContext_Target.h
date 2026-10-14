// บทที่ 12 — Context ของตัวช่วยสาธิตใน Editor (Editor Teaching Aid)
// คืน Actor ตัวแรกที่ติด Tag "EQSTeachingAid" (TP_Target ใน L_Ch12)
// ใช้กับ EQS Testing Pawn เท่านั้น: เป้าหมายของศัตรูขณะเล่นคือสิ่งที่
// ศัตรูรับรู้ได้ (EnvQueryContext_PerceivedTarget) ซึ่งสืบทอดคลาสนี้
// Context ใน C++ Override ProvideContext() เพียงฟังก์ชันเดียว
// (เทียบ Variant_Combat/AI/EnvQueryContext_Player.* ของ Template)

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
// ต้องเป็น #include บรรทัดสุดท้ายเสมอ
#include "EnvQueryContext_Target.generated.h"

class UBlackboardComponent;

UCLASS()
class HAPBOOK_API UEnvQueryContext_Target
	: public UEnvQueryContext
{
	GENERATED_BODY()

public:
	// Tag ที่ TP_Target ต้องมี
	// (แผง Details หมวด Actor ส่วน Advanced ช่อง Tags)
	static const FName TeachingAidTag;

	// บันทึก Actor ที่ติด Tag เป็น Context; ไม่พบ = ไม่บันทึก
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance,
		FEnvQueryContextData& ContextData) const override;

protected:
	// Blackboard ของ AI ที่เป็น Querier (Pawn หรือ Controller)
	// nullptr = Querier ไม่ใช่ AI เช่น Testing Pawn ใน Editor
	static const UBlackboardComponent* FindQuerierBlackboard(
		const FEnvQueryInstance& QueryInstance);
};
