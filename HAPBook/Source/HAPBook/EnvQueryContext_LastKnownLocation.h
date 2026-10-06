// บทที่ 12 — Context ของตำแหน่งที่พบล่าสุด (Last Known Location)
// อ่านคีย์ LastKnownLocation ใน Blackboard ของ Querier (บทที่ 11 เขียน
// ทั้งเมื่อเห็นและเมื่อหาย และไม่ล้าง) แล้วบันทึกเป็นตำแหน่ง (Point)
// Querier ที่ไม่มี Blackboard (EQS Testing Pawn) ได้ตัวช่วยสาธิตของคลาสแม่

#pragma once

#include "CoreMinimal.h"
#include "EnvQueryContext_Target.h"
// ต้องเป็น #include บรรทัดสุดท้ายเสมอ
#include "EnvQueryContext_LastKnownLocation.generated.h"

UCLASS()
class HAPBOOK_API UEnvQueryContext_LastKnownLocation
	: public UEnvQueryContext_Target
{
	GENERATED_BODY()

public:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance,
		FEnvQueryContextData& ContextData) const override;
};
