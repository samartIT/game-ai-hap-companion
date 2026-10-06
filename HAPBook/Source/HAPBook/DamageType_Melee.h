// บทที่ 13 — ชนิดความเสียหายของการโจมตีระยะประชิด (ศัตรูและผู้เล่นใช้ชนิดเดียวกัน)
// ป้ายกำกับเช่นเดียวกับ UDamageType_Hazard ของบทที่ 10: ไม่มีตัวเลขความเสียหายในตัว ผู้รับเป็นผู้ตัดสินความหมาย
// ต่างจาก Hazard ตรงที่มีผู้กระทำเสมอ: ผู้ส่งเติม Instigator (Controller ของผู้โจมตี) ทุกครั้ง

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "DamageType_Melee.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

UCLASS()
class HAPBOOK_API UDamageType_Melee : public UDamageType
{
	GENERATED_BODY()

public:
	UDamageType_Melee();
};
