// บทที่ 11 — ผู้เล่นที่มีฝ่าย: คลาสลูกของ AHAPBookCharacter (คลาสผู้เล่นของ Template ซึ่งงานเขียนนี้ไม่แก้)
// ขยาย Template ด้วยการสืบทอด ไม่ใช่การแก้ไฟล์ของ Epic: ทุกอย่างของ Template (กล้อง, Enhanced Input, DoMove/DoLook) คงเดิม
// สิ่งเดียวที่เพิ่มคือ IGenericTeamAgentInterface ซึ่ง AI Perception อ่านจาก Pawn ของผู้เล่นโดยตรง
// (FGenericTeamId::GetTeamIdentifier Cast Actor เป็น Interface — ไม่ไล่ไปที่ Controller) ผู้เล่นจึงต้องถือฝ่ายไว้ที่ร่างของตนเอง
// BP_ThirdPersonCharacter ถูก Reparent มายังคลาสนี้ (File -> Reparent Blueprint) ในขั้นที่ 2 ของตัวอย่างประกอบ (ฝ่ายในฐานะตัวกรองของการรับรู้)
// บทที่ 13 เพิ่ม: LogFromBlueprint() — เครื่องมือวัดที่ Blueprint ของผู้เล่นเรียกเพื่อบันทึกบรรทัด [Combat] ลง LogTemp
//                (นโยบายการตายของผู้เล่นอยู่ใน Event Graph ของ BP_ThirdPersonCharacter ไม่ใช่ที่นี่)

#pragma once

#include "CoreMinimal.h"
#include "HAPBookCharacter.h"
#include "GenericTeamAgentInterface.h"
#include "HAPPlayerCharacter.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

UCLASS()
class HAPBOOK_API AHAPPlayerCharacter : public AHAPBookCharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// ฝ่ายของผู้เล่น (ค่าเริ่มต้น 1; ศัตรูคือ 2 — ดู AEnemyAIController::EnemyTeamId) ปรับได้ใน Class Defaults ของ BP_ThirdPersonCharacter
	// 255 คือ FGenericTeamId::NoTeam ซึ่ง Attitude Solver ของงานเขียนนี้ตีความเป็น Neutral เสมอ
	UPROPERTY(EditDefaultsOnly, Category = "Team", meta = (ClampMin = "0", ClampMax = "255"))
	uint8 TeamId = 1;

	// IGenericTeamAgentInterface: ฟังก์ชันเสมือนของ C++ ล้วน (ไม่ใช่ UFUNCTION) Blueprint จึงทั้ง Implement และเรียกไม่ได้
	virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(TeamId); }
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamId) override { TeamId = NewTeamId.GetId(); }

	// บทที่ 13 (เครื่องมือวัด): บรรทัด [Combat] ใน LogTemp
	// Print String เขียนลง LogBlueprintUserMessages
	// ไม่ใช่ LogTemp
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void LogFromBlueprint(const FString& Message) const;
};
