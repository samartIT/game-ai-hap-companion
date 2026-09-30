// บทที่ 7 — ศัตรูตัวแรก: ส่วนกายภาพ (Body) ของ AI
// สืบทอดจาก ACharacter โดยตรง ไม่ใช่จาก AHAPBookCharacter: คลาสผู้เล่นของเทมเพลตผูกกล้อง (SpringArm/Camera)
// และ Enhanced Input ไว้ ซึ่งศัตรูต้องไม่สืบทอด ตามแนวทาง HAP รากฐานของศัตรูคือคลาส C++ ของตนเอง
// ส่วนโมเดล Mannequin และ Animation Blueprint เป็นชั้นตรรกะระดับสูง กำหนดใน Blueprint ลูก (BP_Enemy)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

class UBehaviorTree;

UCLASS(Blueprintable)
class HAPBOOK_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

	// Behavior Tree ที่ AEnemyAIController จะรันเมื่อเข้าควบคุมร่างนี้
	// EditDefaultsOnly: กำหนดใน Class Defaults ของ BP_Enemy (C++ ไม่อ้างอิงแอสเซ็ตโดยตรง ตามแบบแผนของเทมเพลต)
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	// จุดลาดตระเวน (TargetPoint ในฉาก) เรียงตามลำดับที่จะเดินไป
	// EditInstanceOnly: กำหนดที่อินสแตนซ์ที่วางในฉากเท่านั้น เพราะศัตรูแต่ละตัวมีเส้นทางของตนเอง
	UPROPERTY(EditInstanceOnly, Category = "Patrol")
	TArray<TObjectPtr<AActor>> PatrolPoints;
};
