// บทที่ 7 — การนิยาม AEnemyAIController

#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h" // FPathFollowingResult

const FName AEnemyAIController::PatrolLocationKey(TEXT("PatrolLocation"));

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

	// 3. เริ่ม Behavior Tree (BT_Enemy: Sequence -> Move To(PatrolLocation) -> Wait 1.0 วนซ้ำ)
	RunBehaviorTree(Enemy->BehaviorTreeAsset);

	// ผลที่คาด: LogTemp: [AI] Possessed BP_Enemy_C: 3 patrol points, Behavior Tree running
	UE_LOG(LogTemp, Log, TEXT("[AI] Possessed %s: %d patrol points, Behavior Tree running"),
		*InPawn->GetClass()->GetName(), Enemy->PatrolPoints.Num());
}

void AEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	// เลื่อนจุดเฉพาะเมื่อไปถึงจริง: ผล Aborted/Blocked/OffPath ไม่นับ
	if (!Result.IsSuccess())
	{
		return;
	}

	const AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetPawn());
	if (Enemy == nullptr || Enemy->PatrolPoints.IsEmpty())
	{
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
