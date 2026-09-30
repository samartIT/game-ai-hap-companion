// บทที่ 7 — ศัตรูตัวแรก: ส่วนสั่งการ (Mind) ของ AI
// AAIController เป็น Actor ที่ไม่มีตัวตนทางกายภาพ ทำหน้าที่เข้าควบคุม (Possess) AEnemyCharacter
// แล้วรัน Behavior Tree โดยใช้ Blackboard เป็นหน่วยความจำร่วม ใช้เฉพาะโหนดสำเร็จรูปของเอนจิน (โหนดที่เขียนเองคือบทที่ 10)

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

UCLASS()
class HAPBOOK_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	// ชื่อคีย์ชนิด Vector ใน BB_Enemy ที่โหนด Move To อ่านเป็นจุดหมาย
	static const FName PatrolLocationKey;

protected:
	// เรียกเมื่อ Controller เข้าควบคุม Pawn: ผูก Blackboard, เขียนจุดแรก, เริ่ม Behavior Tree
	virtual void OnPossess(APawn* InPawn) override;

	// เรียกเมื่อคำขอเคลื่อนที่ปัจจุบันจบลง (รวมถึงคำขอที่โหนด Move To ของ Behavior Tree เป็นผู้สั่ง)
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

private:
	// ดัชนีของจุดลาดตระเวนปัจจุบัน (0..Num-1) วนกลับเมื่อถึงจุดสุดท้าย
	int32 CurrentPatrolIndex = 0;

	// เขียนตำแหน่งของ PatrolPoints[CurrentPatrolIndex] ลง Blackboard
	void WriteCurrentPatrolLocation();
};
