// บทที่ 11 — การนิยาม AHAPPlayerCharacter
// ทั้งสองฟังก์ชันของ Interface เป็น inline ใน Header (บทที่ 11)
// บทที่ 13 เพิ่ม: LogFromBlueprint() ซึ่งเป็นนิยามเดียวในไฟล์นี้

#include "HAPPlayerCharacter.h"

void AHAPPlayerCharacter::LogFromBlueprint(
	const FString& Message) const
{
	// ผลที่คาด: LogTemp: [Combat] Player died
	UE_LOG(LogTemp, Log, TEXT("[Combat] %s"), *Message);
}
