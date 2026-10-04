// บทที่ 10 — การนิยาม UDamageType_Hazard

#include "DamageType_Hazard.h"

UDamageType_Hazard::UDamageType_Hazard()
{
	// ความเสียหายจากโลก (ลาวา ตกจากฉาก เขตอันตราย) ไม่ใช่จากผู้โจมตี: เหตุความเสียหายชนิดนี้จึงไม่มี Instigator โดยธรรมชาติ
	bCausedByWorld = true;

	// ไม่มีแรงกระแทกทางฟิสิกส์ (ค่าเริ่มต้นของเอนจิ้นคือ 800): เขตอันตรายไม่ผลักตัวละคร
	DamageImpulse = 0.0f;
}
