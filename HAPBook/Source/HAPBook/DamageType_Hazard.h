// บทที่ 10 — ชนิดความเสียหายจากสภาพแวดล้อม (เขตอันตราย BP_HazardZone)
// UDamageType เป็นป้ายกำกับ (metadata) ของเหตุความเสียหาย: ไม่มีตัวเลขความเสียหายในตัว ผู้รับใช้ชนิดนี้ตัดสินใจ (เช่น เกราะลดครึ่ง — แบบฝึกหัด)
// เอนจิ้นใช้ CDO ของคลาสนี้ (GetDefaultObject) ไม่สร้างอินสแตนซ์ต่อเหตุ จึงประกาศเป็นคลาสว่าง ตั้งค่าใน Constructor เท่านั้น

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "DamageType_Hazard.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

UCLASS()
class HAPBOOK_API UDamageType_Hazard : public UDamageType
{
	GENERATED_BODY()

public:
	UDamageType_Hazard();
};
