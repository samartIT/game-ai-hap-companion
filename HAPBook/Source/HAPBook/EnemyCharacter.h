// บทที่ 7 — ศัตรูตัวแรก: ส่วนกายภาพ (Body) ของ AI
// สืบทอดจาก ACharacter โดยตรง ไม่ใช่จาก AHAPBookCharacter: คลาสผู้เล่นของ Template ผูกกล้อง (SpringArm/Camera)
// และ Enhanced Input ไว้ ซึ่งศัตรูต้องไม่สืบทอด ตามแนวทาง HAP รากฐานของศัตรูคือคลาส C++ ของตนเอง
// ส่วนโมเดล Mannequin และ Animation Blueprint เป็นชั้นตรรกะระดับสูง กำหนดใน Blueprint ลูก (BP_Enemy)
// บทที่ 10 เพิ่ม: UHealthComponent (ร่างกายที่บาดเจ็บได้) และลำดับการตาย (หยุด Behavior Tree ปิดการชนและการเคลื่อนที่ ทำลายใน 3 วินาที)
// บทที่ 11 เพิ่ม: สวิตช์ต่ออินสแตนซ์ bPerceptionEnabled ที่ AEnemyAIController อ่านตอน Possess (ร่างถือค่า เพราะ Controller ถูก Spawn ไม่ได้วาง)
//                ค่าของ Sense ทั้งหมด (รวม Detect Neutrals) อยู่ใน BP_EnemyAIController ไม่ใช่ที่นี่
// บทที่ 12 เพิ่ม: BehaviorTreeAsset ตั้งที่อินสแตนซ์ได้ (Behavior Tree ของแต่ละบทเป็น Asset ของตนเอง)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

class UBehaviorTree;
class UHealthComponent;

UCLASS(Blueprintable)
class HAPBOOK_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

	// Behavior Tree ที่ AEnemyAIController จะรันเมื่อเข้าควบคุมร่างนี้
	// กำหนดใน Class Defaults ของ BP_Enemy (C++ ไม่อ้างอิง Asset โดยตรง ตามแบบแผนของ Template)
	// บทที่ 12: EditDefaultsOnly -> EditAnywhere
	// อินสแตนซ์ที่วางในแผนที่เลือก Behavior Tree ของตนได้
	// (L_Ch12 = BT_Enemy_EQS) ค่าของคลาสยังเป็น BT_Enemy:
	// แผนที่ของบทก่อนหน้าไม่เปลี่ยน
	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	// จุดลาดตระเวน (TargetPoint ในฉาก) เรียงตามลำดับที่จะเดินไป
	// EditInstanceOnly: กำหนดที่อินสแตนซ์ที่วางในฉากเท่านั้น เพราะศัตรูแต่ละตัวมีเส้นทางของตนเอง
	UPROPERTY(EditInstanceOnly, Category = "Patrol")
	TArray<TObjectPtr<AActor>> PatrolPoints;

	// บทที่ 10: Component พลังชีวิต สร้างใน Constructor (CreateDefaultSubobject) จึงปรากฏใน BP_Enemy เป็น Component ที่สืบทอดมา
	// VisibleAnywhere: เห็น Component ในแผง Details (และปรับ MaxHealth ของ Component ได้ที่นั่น) BlueprintReadOnly: กราฟของ BP_Enemy อ้างถึงได้
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	TObjectPtr<UHealthComponent> HealthComponent;

	// บทที่ 11: เปิด Listener ของ Controller ให้อินสแตนซ์นี้ — Component AIPerception ของ BP_EnemyAIController มีอยู่ในทุกแผนที่
	// ตอน Possess Controller ผูกตัวรับและคงการลงทะเบียน Listener ไว้เมื่อจริง หรือถอน Listener ออกจากระบบกลางเมื่อเท็จ
	// EditInstanceOnly โดยเจตนา: กาที่ BP_Enemy ที่วางใน L_Ch11 เท่านั้น ค่าเริ่มต้นเท็จทำให้ L_Ch07–L_Ch10 ไม่เปลี่ยนแม้แต่บรรทัดเดียว
	UPROPERTY(EditInstanceOnly, Category = "Perception")
	bool bPerceptionEnabled = false;

protected:
	// บทที่ 10: ผูก OnDeath ของ Component (Delegate ผูกเมื่อทั้งสองฝ่ายมีตัวตนแล้ว)
	virtual void BeginPlay() override;

private:
	// บทที่ 10: ลำดับการตายของศัตรู — หยุดตรรกะ AI, ปิดการชนและการเคลื่อนที่, ไม่รับความเสียหายอีก, ทำลาย Actor หลัง CorpseLifeSpan
	// ไม่มี Animation การตายโดยเจตนา (ขอบเขตของงานเขียน) ร่างหยุดนิ่งในท่าสุดท้ายแล้วหายไป
	UFUNCTION()
	void HandleDeath();

	// เวลาที่ร่างคงอยู่หลังตายก่อนถูกทำลาย (วินาที)
	static constexpr float CorpseLifeSpan = 3.0f;
};
