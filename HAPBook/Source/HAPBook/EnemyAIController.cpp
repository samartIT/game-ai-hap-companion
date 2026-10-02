// บทที่ 7 — การนิยาม AEnemyAIController (บทที่ 8 เพิ่มส่วนที่กำกับว่า "บทที่ 8")

#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h" // FPathFollowingResult, GetPathDestination
#include "TimerManager.h"

const FName AEnemyAIController::PatrolLocationKey(TEXT("PatrolLocation"));
const FName AEnemyAIController::AlertedKey(TEXT("Alerted"));
const FName AEnemyAIController::AlertLocationKey(TEXT("AlertLocation"));

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	const AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(InPawn);
	if (Enemy == nullptr || Enemy->BehaviorTreeAsset == nullptr || Enemy->PatrolPoints.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[AI] %s: Behavior Tree not started (asset or patrol points missing)"),
			*GetNameSafe(InPawn));
		return;
	}

	// 1. Blackboard: หน่วยความจำร่วมระหว่าง C++ กับ Behavior Tree (สร้าง Component ให้หากยังไม่มี)
	UBlackboardComponent* BlackboardComp = nullptr;
	if (!UseBlackboard(Enemy->BehaviorTreeAsset->BlackboardAsset, BlackboardComp))
	{
		return;
	}

	// 2. เขียนจุดแรกก่อนเริ่ม Behavior Tree เพื่อให้ Node Move To แรกอ่านคีย์ที่มีค่าแล้ว
	CurrentPatrolIndex = 0;
	WriteCurrentPatrolLocation();

	// 2b. บทที่ 8: ลงทะเบียนผู้สังเกตคีย์ Alerted ก่อนเริ่ม Behavior Tree (ถูกเรียกก่อน Decorator ในการเปลี่ยนค่าเดียวกัน)
	//     BB_Enemy ของบทที่ 7 ยังไม่มีคีย์นี้: ข้ามไปเงียบ ๆ เพื่อให้ L_Ch07 ทำงานเหมือนเดิมทุกประการ
	const FBlackboard::FKey AlertedId = BlackboardComp->GetKeyID(AlertedKey);
	if (AlertedId != FBlackboard::InvalidKey)
	{
		BlackboardComp->RegisterObserver(AlertedId, this,
			FOnBlackboardChangeNotification::CreateUObject(this, &AEnemyAIController::OnAlertedChanged));
	}

	// 3. เริ่ม Behavior Tree (BT_Enemy: Selector -> [Investigate: Alerted Is Set -> Move To(AlertLocation) -> Wait 2.0]
	//                                               | [Sequence -> Move To(PatrolLocation) -> Wait 1.0] )
	RunBehaviorTree(Enemy->BehaviorTreeAsset);

	// ผลที่คาด: LogTemp: [AI] Possessed BP_Enemy_C: 3 patrol points, Behavior Tree running
	UE_LOG(LogTemp, Log, TEXT("[AI] Possessed %s: %d patrol points, Behavior Tree running"),
		*InPawn->GetClass()->GetName(), Enemy->PatrolPoints.Num());
}

void AEnemyAIController::RaiseAlert(const FVector& Location, FName SourceName)
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (BlackboardComp == nullptr)
	{
		return;
	}

	// จุดหมายก่อนสวิตช์: ทันทีที่ Alerted เป็นจริง Decorator ร้องขอการยุติ และ Move To ของกิ่ง Investigate จะอ่าน AlertLocation
	BlackboardComp->SetValueAsVector(AlertLocationKey, Location);

	// ผลที่คาด: LogTemp: [BT] Alert raised at AlertZone
	UE_LOG(LogTemp, Log, TEXT("[BT] Alert raised at %s"), *SourceName.ToString());

	BlackboardComp->SetValueAsBool(AlertedKey, true);   // ผู้สังเกตทุกราย (รวม Decorator) ถูกเรียกภายในบรรทัดนี้
}

EBlackboardNotificationResult AEnemyAIController::OnAlertedChanged(const UBlackboardComponent& ChangedBlackboard, FBlackboard::FKey ChangedKey)
{
	if (ChangedBlackboard.GetValueAsBool(AlertedKey))
	{
		// Decorator ร้องขอการยุติในการเรียกเดียวกันนี้ แต่ Behavior Tree ประมวลผลคำขอในการ Tick ถัดไป จึงตรวจผลหลัง 0.1 วินาที
		GetWorldTimerManager().SetTimer(AbortCheckTimer, this, &AEnemyAIController::CheckAbortAfterAlert, 0.1f, false);
	}
	return EBlackboardNotificationResult::ContinueObserving;
}

void AEnemyAIController::CheckAbortAfterAlert()
{
	const UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	const UPathFollowingComponent* PathComp = GetPathFollowingComponent();
	if (BlackboardComp == nullptr || PathComp == nullptr)
	{
		return;
	}

	// คำขอเคลื่อนที่ปัจจุบันมุ่งไป AlertLocation = กิ่ง Investigate เข้ามาแทนที่กิ่งลาดตระเวนแล้ว (สังเกตได้จริง ไม่ใช่อนุมานจากค่าที่ตั้ง)
	const bool bMovingToAlert = PathComp->HasValidPath()
		&& FVector::Dist2D(PathComp->GetPathDestination(), BlackboardComp->GetValueAsVector(AlertLocationKey)) <= ArrivalRadius;

	if (bMovingToAlert)
	{
		// ผลที่คาด: LogTemp: [BT] Patrol aborted by Observer Aborts (Lower Priority)
		UE_LOG(LogTemp, Log, TEXT("[BT] Patrol aborted by Observer Aborts (Lower Priority)"));
	}
	else
	{
		// Observer Aborts ของ Decorator เป็น None: ค่าคีย์เปลี่ยนแล้ว แต่ Behavior Tree ยังอยู่กิ่งลาดตระเวนจนจบรอบ (แบบฝึกหัดข้อ 4)
		UE_LOG(LogTemp, Warning, TEXT("[BT] Alerted is set but patrol was not aborted (Observer Aborts = None?)"));
	}
}

void AEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	// เลื่อนจุดเฉพาะเมื่อไปถึงจริง: ผล Aborted (เช่น ถูก Observer Aborts ขัดจังหวะ)/Blocked/OffPath ไม่นับ ดัชนีจึงคงเดิม
	if (!Result.IsSuccess())
	{
		return;
	}

	const AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetPawn());
	if (Enemy == nullptr || Enemy->PatrolPoints.IsEmpty())
	{
		return;
	}

	// บทที่ 8: การมาถึง AlertLocation ไม่เลื่อนจุดลาดตระเวน และล้าง Alerted ให้ Decorator เห็นค่าเท็จ
	// (ด้วย Observer Aborts แบบ Lower Priority Decorator ไม่ยุติกิ่งของตนเอง Wait 2.0 จึงเล่นจนจบ แล้ว Selector ตกไปกิ่งลาดตระเวน)
	if (WasInvestigateArrival())
	{
		GetBlackboardComponent()->ClearValue(AlertedKey);
		// ผลที่คาด: LogTemp: [BT] Investigate: reached AlertLocation, Alerted cleared
		UE_LOG(LogTemp, Log, TEXT("[BT] Investigate: reached AlertLocation, Alerted cleared"));
		return;
	}

	// กฎคงที่ (บันทึกจากการถดถอยของบทที่ 12): ทุกคำขอที่สำเร็จของกิ่งลาดตระเวนต้องมาถึงบรรทัดนี้
	// บทที่เพิ่มกิ่งอื่นในภายหลังต้องจำแนกการมาถึงจาก "จุดหมายของคำขอที่เพิ่งจบ" (คีย์ Blackboard + ตำแหน่ง Pawn)
	// ไม่ใช่จากสวิตช์โหมดของตัวละคร เพราะ Selector อาจตกมากิ่งลาดตระเวนขณะสวิตช์นั้นยังเปิดอยู่
	// และ UPathFollowingComponent::OnPathFinished() ล้างเส้นทางก่อนแจ้ง OnMoveCompleted() จึงอ่านจุดหมายจากเส้นทางไม่ได้
	const int32 Count = Enemy->PatrolPoints.Num();
	// ผลที่คาด: LogTemp: [AI] Reached waypoint 1 of 3 (แล้ว 2 of 3, 3 of 3, วนกลับ 1 of 3 ...)
	UE_LOG(LogTemp, Log, TEXT("[AI] Reached waypoint %d of %d"), CurrentPatrolIndex + 1, Count);

	CurrentPatrolIndex = (CurrentPatrolIndex + 1) % Count;   // วนกลับไปจุดแรกหลังจุดสุดท้าย
	WriteCurrentPatrolLocation();                             // Node Move To รอบถัดไปอ่านค่าใหม่นี้
}

bool AEnemyAIController::WasInvestigateArrival() const
{
	const UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	const APawn* ControlledPawn = GetPawn();
	if (BlackboardComp == nullptr || ControlledPawn == nullptr || !BlackboardComp->GetValueAsBool(AlertedKey))
	{
		return false;   // ไม่มีสัญญาณค้าง (ลาดตระเวนล้วน) หรือ BB_Enemy ไม่มีคีย์ (บทที่ 7)
	}

	// การมาถึง = Pawn อยู่ภายในระยะยอมรับของ AlertLocation
	return FVector::Dist2D(ControlledPawn->GetActorLocation(), BlackboardComp->GetValueAsVector(AlertLocationKey)) <= ArrivalRadius;
}

void AEnemyAIController::WriteCurrentPatrolLocation()
{
	const AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetPawn());
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (Enemy == nullptr || BlackboardComp == nullptr || !Enemy->PatrolPoints.IsValidIndex(CurrentPatrolIndex))
	{
		return;
	}

	const AActor* Point = Enemy->PatrolPoints[CurrentPatrolIndex];
	if (Point == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AI] PatrolPoints[%d] is not assigned"), CurrentPatrolIndex);
		return;
	}

	BlackboardComp->SetValueAsVector(PatrolLocationKey, Point->GetActorLocation());
}
