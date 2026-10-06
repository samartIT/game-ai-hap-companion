// บทที่ 13 — การนิยาม UDamageType_Melee

#include "DamageType_Melee.h"

UDamageType_Melee::UDamageType_Melee()
{
	// มีผู้กระทำเสมอ: ไม่ใช่ความเสียหายจากโลกของเกม
	// (ค่าเริ่มต้นของ UDamageType ก็เป็นเท็จ
	// เขียนไว้เพื่อเทียบกับ Hazard)
	bCausedByWorld = false;
}
