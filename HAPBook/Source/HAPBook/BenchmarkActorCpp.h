// บทที่ 14 — สถาปัตยกรรม C++ ล้วน (Pure C++): Tick() เรียก RunWork() โดยตรง
// คลาส Native นี้คือ Arm Class ของ C++ ล้วนโดยตรง ไม่มีคลาสลูกแบบ Blueprint โดยเจตนา:
// Actor ที่เป็นคลาส Blueprint จ่ายต้นทุนของ Event Tick ทุกเฟรม (AActor::Tick) ซึ่งคือสิ่งที่การเปรียบเทียบต้องแยกออก

#pragma once

#include "CoreMinimal.h"
#include "BenchmarkActorBase.h"
#include "BenchmarkActorCpp.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

UCLASS()
class HAPBOOK_API ABenchmarkActorCpp : public ABenchmarkActorBase
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaSeconds) override;
};
