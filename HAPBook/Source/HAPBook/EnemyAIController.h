// บทที่ 7 — ศัตรูตัวแรก: ส่วนสั่งการ (Mind) ของ AI
// AAIController เป็น Actor ที่ไม่มีตัวตนทางกายภาพ ทำหน้าที่เข้าควบคุม (Possess) AEnemyCharacter
// แล้วรัน Behavior Tree โดยใช้ Blackboard เป็นหน่วยความจำร่วม ใช้เฉพาะ Node สำเร็จรูปของเอนจิ้น (Node ที่เขียนเองคือบทที่ 9)
// บทที่ 8 เพิ่ม: คีย์ Alerted/AlertLocation, RaiseAlert() ที่ Blueprint เรียกได้, ผู้สังเกตคีย์ Alerted,
//               การจำแนกการมาถึงจากจุดหมายของคำขอ และการล้าง Alerted เมื่อการตรวจสอบเสร็จ
// บทที่ 11 เพิ่ม (เฉพาะสิ่งที่ Blueprint ทำไม่ได้หรือสิ่งที่ถือนโยบาย): ฝ่าย (Team 2 + Attitude Solver),
//                ตัวรับสิ่งเร้า OnTargetPerceptionUpdated ที่เขียนความรู้ลง Blackboard เฉพาะเมื่อความสัมพันธ์เป็น Hostile
//                และสวิตช์เปิด/ปิด Listener ตาม bPerceptionEnabled ของร่าง
//                ระบบรับรู้เอง (Component + Sense Sight/Hearing + ค่าของ Sense) อยู่ใน BP_EnemyAIController ซึ่งเป็น Blueprint ลูกของคลาสนี้
// บทที่ 12 เพิ่ม: คีย์ SearchLocation (ผลของ EQS_SearchHidden), ผู้สังเกตคีย์นี้พร้อมทรงกลม Debug (เครื่องมือวัด)
//                และการจำแนกการมาถึงจุดซ่อน — Query เองรันโดย Run EQS Query ใน BT_Enemy_EQS ไม่ใช่โดยคลาสนี้
// บทที่ 13 เพิ่ม (ใน OnTargetPerceptionUpdated เท่านั้น): สาขาของ Sense Damage (ผู้กระทำที่เป็น Hostile กลายเป็นเป้าหมาย)
//                และการเพิกเฉยเมื่อร่างของตนหรือ Actor ที่รับรู้ตายแล้ว — การโจมตีเองเป็นของ BTTask_Attack ใน BT_Enemy_Combat

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeTypes.h" // FBlackboard::FKey, EBlackboardNotificationResult, FOnBlackboardChangeNotification
#include "Perception/AIPerceptionTypes.h"   // FAIStimulus (พารามิเตอร์ของ OnTargetPerceptionUpdated)
#include "EnemyAIController.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

class AEnemyCharacter;

UCLASS()
class HAPBOOK_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

	// ชื่อคีย์ชนิด Vector ใน BB_Enemy ที่ Node Move To ของ Branch ลาดตระเวนอ่านเป็นจุดหมาย (บทที่ 7)
	static const FName PatrolLocationKey;

	// บทที่ 8: คีย์ชนิด Bool ที่ Decorator Blackboard (Is Set, Observer Aborts: Lower Priority) ของ Branch Investigate สังเกต
	static const FName AlertedKey;

	// บทที่ 8: คีย์ชนิด Vector ที่ Node Move To ของ Branch Investigate อ่าน
	static const FName AlertLocationKey;

	// บทที่ 11: คีย์ชนิด Object (Actor) ที่ Decorator Blackboard (Is Set, Observer Aborts: Both) ของ Branch Chase สังเกต และ Move To ของ Branch นั้นอ่าน
	static const FName TargetActorKey;

	// บทที่ 11: คีย์ชนิด Vector = ตำแหน่งสิ่งเร้าล่าสุดของการมองเห็นเป้าหมายที่เป็น Hostile (ความจำของเป้าหมาย) เขียนทั้งเมื่อเห็นและเมื่อหาย
	static const FName LastKnownLocationKey;

	// บทที่ 11: ฝ่ายของศัตรู (ผู้เล่นคือ 1 — ดู AHAPPlayerCharacter::TeamId)
	static constexpr uint8 EnemyTeamId = 2;

	// บทที่ 12: คีย์ที่ Run EQS Query เขียน (BB_Enemy_EQS)
	// SearchLocation = จุดซ่อนที่ส่วน Search ไปตรวจ
	static const FName SearchLocationKey;

	// บทที่ 8: เรียกจาก Blueprint (BP_AlertZone เมื่อผู้เล่นทับซ้อน ส่งตำแหน่งของตนเอง): เขียน AlertLocation ก่อน แล้วตั้ง Alerted = true
	// SourceName ใช้ในบรรทัดบันทึกเท่านั้น (BP_AlertZone ส่ง "AlertZone")
	UFUNCTION(BlueprintCallable, Category = "AI")
	void RaiseAlert(const FVector& Location, FName SourceName);

protected:
	// เรียกเมื่อ Controller เข้าควบคุม Pawn: ผูก Blackboard, เขียนจุดแรก, ลงทะเบียนผู้สังเกต Alerted, (บทที่ 11) เปิดหรือปิด Listener ตามร่าง, เริ่ม Behavior Tree
	virtual void OnPossess(APawn* InPawn) override;

	// บทที่ 11: เครื่องมือวัดเท่านั้น — บันทึกฝ่ายของทั้งสองและความสัมพันธ์ที่เอนจิ้นคำนวณ (หลัง Pawn ของผู้เล่นเกิดแล้ว)
	virtual void BeginPlay() override;

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

	// บทที่ 11: จริงเมื่อคำขอที่เพิ่งจบคือการไล่ตาม TargetActor (คีย์ตั้งอยู่ และ Pawn อยู่ภายในระยะยอมรับของเป้าหมาย)
	bool WasChaseArrival() const;

	// ระยะที่ถือว่า "มาถึง": Acceptable Radius 50 ของ Node Move To + รัศมีแคปซูล 34 + ส่วนเผื่อ
	static constexpr float ArrivalRadius = 150.0f;

	// บทที่ 11: ระยะที่ถือว่า "ถึงเป้าหมายที่ไล่ตาม": Acceptable Radius 150 ของ Move To ของ Branch Chase + แคปซูลทั้งสอง + ส่วนเผื่อ
	static constexpr float ChaseArrivalRadius = 300.0f;

	// ดัชนีของจุดลาดตระเวนปัจจุบัน (0..Num-1) วนกลับเมื่อถึงจุดสุดท้าย
	int32 CurrentPatrolIndex = 0;

	// เขียนตำแหน่งของ PatrolPoints[CurrentPatrolIndex] ลง Blackboard
	void WriteCurrentPatrolLocation();

	// ---- บทที่ 11: ระบบรับรู้ (Component และ Sense อยู่ใน BP_EnemyAIController) ----

	// เปิด: ผูก OnTargetPerceptionUpdated และยืนยันการลงทะเบียน Listener; ปิด: ถอดการผูกและถอน Listener ออกจาก AIPerceptionSystem
	// Component ที่เพิ่มใน Blueprint ใส่ตัวเองลงช่อง PerceptionComponent ของ AAIController ใน OnRegister ก่อน Possess จึงหาได้ด้วย GetAIPerceptionComponent()
	void SetPerceptionActive(bool bActive);

	// ปลายทางของ Delegate OnTargetPerceptionUpdated (Dynamic Delegate จึงต้องเป็น UFUNCTION)
	// บันทึก Sight gained ของทุกสิ่ง แต่เขียน Blackboard เฉพาะเมื่อความสัมพันธ์ต่อ Actor เป็น Hostile:
	// Sight เห็น -> LastKnownLocation + TargetActor; Sight หาย -> LastKnownLocation/AlertLocation + Alerted แล้วล้าง TargetActor; Hearing -> AlertLocation + Alerted
	// บทที่ 13: Damage -> LastKnownLocation + TargetActor (บันทึก Damage from ของทุกผู้กระทำ); ร่างของตนหรือ Actor ที่ตายแล้ว -> ไม่ทำสิ่งใด
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	// เครื่องมือวัด: บรรทัด [Perception] Teams: enemy=2 player=1 attitude=Hostile (เรียกใน Tick ถัดจาก BeginPlay)
	void LogTeamAttitude();

	// เครื่องมือวัด: ชื่อที่ใช้ในบรรทัดบันทึก — DummyName สำหรับ ATargetDummy, ชื่อคลาส (BP_ThirdPersonCharacter_C) สำหรับ Actor อื่น
	static FString PerceivedName(const AActor* Actor);

	// ---- บทที่ 12: ผลของ EQS ใน Blackboard (BT_Enemy_EQS เท่านั้น) ----

	// เครื่องมือวัด: ผู้สังเกตคีย์ SearchLocation
	// บันทึกบรรทัด [EQS] และวาดทรงกลมที่จุดที่ Query เลือก
	EBlackboardNotificationResult OnSearchLocationChanged(
		const UBlackboardComponent& ChangedBlackboard,
		FBlackboard::FKey ChangedKey);

	// เครื่องมือวัด: ทรงกลม Debug (ไม่มีใน Shipping/Test)
	void DrawSearchLocation(const FVector& Location) const;
	static constexpr float SearchSphereRadius = 60.0f;
	static constexpr float SearchSphereSeconds = 3.0f;

	// จำแนกการมาถึงจากจุดหมายของคำขอ (กฎของบทที่ 8)
	// จุดซ่อน: ถึง SearchLocation (ล้างคีย์หลังใช้)
	bool WasSearchArrival() const;
};
