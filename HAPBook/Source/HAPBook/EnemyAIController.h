// บทที่ 7 — ศัตรูตัวแรก: ส่วนสั่งการ (Mind) ของ AI
// AAIController เป็น Actor ที่ไม่มีตัวตนทางกายภาพ ทำหน้าที่เข้าควบคุม (Possess) AEnemyCharacter
// แล้วรัน Behavior Tree โดยใช้ Blackboard เป็นหน่วยความจำร่วม ใช้เฉพาะ Node สำเร็จรูปของเอนจิ้น (Node ที่เขียนเองคือบทที่ 9)
// บทที่ 8 เพิ่ม: คีย์ Alerted/AlertLocation, RaiseAlert() ที่ Blueprint เรียกได้, ผู้สังเกตคีย์ Alerted,
//               การจำแนกการมาถึงจากจุดหมายของคำขอ และการล้าง Alerted เมื่อการตรวจสอบเสร็จ

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeTypes.h" // FBlackboard::FKey, EBlackboardNotificationResult, FOnBlackboardChangeNotification
#include "EnemyAIController.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

UCLASS()
class HAPBOOK_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	// ชื่อคีย์ชนิด Vector ใน BB_Enemy ที่ Node Move To ของกิ่งลาดตระเวนอ่านเป็นจุดหมาย (บทที่ 7)
	static const FName PatrolLocationKey;

	// บทที่ 8: คีย์ชนิด Bool ที่ Decorator Blackboard (Is Set, Observer Aborts: Lower Priority) ของกิ่ง Investigate สังเกต
	static const FName AlertedKey;

	// บทที่ 8: คีย์ชนิด Vector ที่ Node Move To ของกิ่ง Investigate อ่าน
	static const FName AlertLocationKey;

	// บทที่ 8: เรียกจาก Blueprint (BP_AlertZone เมื่อผู้เล่นทับซ้อน ส่งตำแหน่งของตนเอง): เขียน AlertLocation ก่อน แล้วตั้ง Alerted = true
	// SourceName ใช้ในบรรทัดบันทึกเท่านั้น (BP_AlertZone ส่ง "AlertZone")
	UFUNCTION(BlueprintCallable, Category = "AI")
	void RaiseAlert(const FVector& Location, FName SourceName);

protected:
	// เรียกเมื่อ Controller เข้าควบคุม Pawn: ผูก Blackboard, เขียนจุดแรก, ลงทะเบียนผู้สังเกต Alerted, เริ่ม Behavior Tree
	virtual void OnPossess(APawn* InPawn) override;

	// เรียกเมื่อคำขอเคลื่อนที่ปัจจุบันจบลง (รวมถึงคำขอที่ Node Move To ของ Behavior Tree เป็นผู้สั่ง)
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

private:
	// บทที่ 8: ผู้สังเกตคีย์ Alerted ถูกเรียกใน SetValue เดียวกับที่ Decorator ถูกเรียก (ลงทะเบียนก่อน RunBehaviorTree จึงถูกเรียกก่อน)
	EBlackboardNotificationResult OnAlertedChanged(const UBlackboardComponent& ChangedBlackboard, FBlackboard::FKey ChangedKey);

	// บทที่ 8: ตรวจหลัง 0.1 วินาทีว่า Behavior Tree ยุติการลาดตระเวนแล้วหรือไม่ โดยดูจากจุดหมายของคำขอเคลื่อนที่ปัจจุบัน
	// (การยุติที่ Decorator ร้องขอถูกประมวลผลในรอบ Tick ของ Behavior Tree ไม่ใช่ทันทีใน SetValue)
	void CheckAbortAfterAlert();
	FTimerHandle AbortCheckTimer;

	// บทที่ 8: จริงเมื่อคำขอเคลื่อนที่ที่เพิ่งจบคือการไปยัง AlertLocation (คีย์ Alerted ถูกตั้ง และ Pawn อยู่ภายในระยะยอมรับ)
	// จำแนกจาก "จุดหมายของคำขอ" ไม่ใช่จากสวิตช์โหมด (บทเรียนจากการถดถอยที่บันทึกใน OnMoveCompleted)
	bool WasInvestigateArrival() const;

	// ระยะที่ถือว่า "มาถึง": Acceptable Radius 50 ของ Node Move To + รัศมีแคปซูล 34 + ส่วนเผื่อ
	static constexpr float ArrivalRadius = 150.0f;

	// ดัชนีของจุดลาดตระเวนปัจจุบัน (0..Num-1) วนกลับเมื่อถึงจุดสุดท้าย
	int32 CurrentPatrolIndex = 0;

	// เขียนตำแหน่งของ PatrolPoints[CurrentPatrolIndex] ลง Blackboard
	void WriteCurrentPatrolLocation();
};
