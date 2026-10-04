// บทที่ 7 — ศัตรูตัวแรก: ส่วนกายภาพ (Body) ของ AI
// สืบทอดจาก ACharacter โดยตรง ไม่ใช่จาก AHAPBookCharacter: คลาสผู้เล่นของ Template ผูกกล้อง (SpringArm/Camera)
// และ Enhanced Input ไว้ ซึ่งศัตรูต้องไม่สืบทอด ตามแนวทาง HAP รากฐานของศัตรูคือคลาส C++ ของตนเอง
// ส่วนโมเดล Mannequin และ Animation Blueprint เป็นชั้นตรรกะระดับสูง กำหนดใน Blueprint ลูก (BP_Enemy)
// บทที่ 10 เพิ่ม: UHealthComponent (ร่างกายที่บาดเจ็บได้) และลำดับการตาย (หยุด Behavior Tree ปิดการชนและการเคลื่อนที่ ทำลายใน 3 วินาที)

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
	// EditDefaultsOnly: กำหนดใน Class Defaults ของ BP_Enemy (C++ ไม่อ้างอิง Asset โดยตรง ตามแบบแผนของ Template)
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	// จุดลาดตระเวน (TargetPoint ในฉาก) เรียงตามลำดับที่จะเดินไป
	// EditInstanceOnly: กำหนดที่อินสแตนซ์ที่วางในฉากเท่านั้น เพราะศัตรูแต่ละตัวมีเส้นทางของตนเอง
	UPROPERTY(EditInstanceOnly, Category = "Patrol")
	TArray<TObjectPtr<AActor>> PatrolPoints;

	// บทที่ 10: Component พลังชีวิต สร้างใน Constructor (CreateDefaultSubobject) จึงปรากฏใน BP_Enemy เป็น Component ที่สืบทอดมา
	// VisibleAnywhere: เห็น Component ในแผง Details (และปรับ MaxHealth ของมันได้ที่นั่น) BlueprintReadOnly: กราฟของ BP_Enemy อ้างถึงได้
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	TObjectPtr<UHealthComponent> HealthComponent;

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
