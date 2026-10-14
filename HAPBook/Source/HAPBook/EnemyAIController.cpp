// บทที่ 7 — การนิยาม AEnemyAIController (บทที่ 8, 11, 12 และ 13 เพิ่มส่วนที่กำกับว่า "บทที่ 8" / "บทที่ 11" / "บทที่ 12" / "บทที่ 13")

#include "EnemyAIController.h"
#include "EnemyCharacter.h"
#include "HealthComponent.h"                   // บทที่ 13: IsActorDead
#include "TargetDummy.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h" // FPathFollowingResult, GetPathDestination
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionSystem.h"      // GetSenseClassForStimulus, UnregisterListener
#include "Perception/AISense_Sight.h"           // ใช้เทียบคลาสของ Sense เท่านั้น: ค่าของ Sense อยู่ใน BP_EnemyAIController
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Damage.h"         // บทที่ 13
#include "Kismet/GameplayStatics.h"            // GetPlayerPawn (บรรทัด Teams เท่านั้น)
#include "TimerManager.h"
#include "DrawDebugHelpers.h"                  // บทที่ 12: ทรงกลม Debug (เครื่องมือวัด)

const FName AEnemyAIController::PatrolLocationKey(TEXT("PatrolLocation"));
const FName AEnemyAIController::AlertedKey(TEXT("Alerted"));
const FName AEnemyAIController::AlertLocationKey(TEXT("AlertLocation"));
const FName AEnemyAIController::TargetActorKey(TEXT("TargetActor"));
const FName AEnemyAIController::LastKnownLocationKey(TEXT("LastKnownLocation"));
const FName AEnemyAIController::SearchLocationKey(
	TEXT("SearchLocation"));

namespace
{
	// บทที่ 11: Attitude Solver (ตัวตัดสินความสัมพันธ์ฝ่าย) ของงานเขียนนี้ — ฝ่ายเดียวกัน Friendly, ต่างฝ่าย Hostile, ไม่มีฝ่าย (NoTeam = 255) Neutral
	// Attitude Solver เริ่มต้นของเอนจิ้น (AIInterfaces.cpp: DefaultTeamAttitudeSolver) คืน A != B ? Hostile : Friendly โดยไม่ตรวจ NoTeam
	// กฎ NoTeam -> Neutral ของเอนจิ้นมีเฉพาะเมื่อ Actor อีกฝ่าย "ไม่มี Interface" (IGenericTeamAgentInterface::GetTeamAttitudeTowards)
	// ส่วนทางเดินที่เทียบรหัสฝ่ายตรง ๆ (Hearing: FAINoiseEvent::TeamIdentifier) จะเห็นเสียงจากผู้ไม่มีฝ่ายเป็น Hostile — Attitude Solver นี้ปิดช่องนั้น
	ETeamAttitude::Type HAPAttitudeSolver(FGenericTeamId A, FGenericTeamId B)
	{
		if (A == FGenericTeamId::NoTeam || B == FGenericTeamId::NoTeam)
		{
			return ETeamAttitude::Neutral;
		}
		return A == B ? ETeamAttitude::Friendly : ETeamAttitude::Hostile;
	}

	// เครื่องมือวัด: ETeamAttitude เป็น enum ธรรมดา (ไม่มีชื่อผ่าน Reflection ที่สะดวก) จึงแปลงเอง
	const TCHAR* AttitudeToString(ETeamAttitude::Type Attitude)
	{
		switch (Attitude)
		{
		case ETeamAttitude::Friendly: return TEXT("Friendly");
		case ETeamAttitude::Hostile:  return TEXT("Hostile");
		default:                      return TEXT("Neutral");
		}
	}
}

AEnemyAIController::AEnemyAIController()
{
	// บทที่ 11: ฝ่ายของศัตรู — AAIController Implement IGenericTeamAgentInterface อยู่แล้ว (สมาชิก TeamID, ค่าเริ่มต้น NoTeam)
	SetGenericTeamId(FGenericTeamId(EnemyTeamId));

	// บทที่ 11: ติดตั้ง Attitude Solver ครั้งเดียวทั้งโปรเจกต์ (เป็นค่าสถิตของ FGenericTeamId) ตั้งแต่ CDO ถูกสร้างตอนโหลดโมดูล
	static bool bSolverInstalled = false;
	if (!bSolverInstalled)
	{
		FGenericTeamId::SetAttitudeSolver(&HAPAttitudeSolver);
		bSolverInstalled = true;
	}
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	const AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(InPawn);
	if (Enemy == nullptr || Enemy->BehaviorTreeAsset == nullptr || Enemy->PatrolPoints.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[AI] %s: Behavior Tree not started (asset or patrol points missing)"),
			*GetNameSafe(InPawn));
		SetPerceptionActive(false);   // บทที่ 11: ไม่มี Behavior Tree ก็ไม่มีผู้ใช้ความรู้ ถอน Listener ที่ Component ลงทะเบียนไว้ตอน Spawn
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

	// 2c. บทที่ 11: เปิด Listener เฉพาะอินสแตนซ์ที่เลือกไว้ในแผนที่ (L_Ch07–L_Ch10 ไม่ได้เลือก: Listener ถูกถอน ไม่มี Line Trace ไม่มีบรรทัดใดเพิ่ม)
	SetPerceptionActive(Enemy->bPerceptionEnabled);

	// 2d. บทที่ 12: ผู้สังเกตคีย์ SearchLocation (มีเฉพาะใน BB_Enemy_EQS)
	//     BB_Enemy ของบทที่ 7–11 ไม่มีคีย์นี้: ข้ามไป
	const FBlackboard::FKey SearchId =
		BlackboardComp->GetKeyID(SearchLocationKey);
	if (SearchId != FBlackboard::InvalidKey)
	{
		BlackboardComp->RegisterObserver(SearchId, this,
			FOnBlackboardChangeNotification::CreateUObject(
				this,
				&AEnemyAIController::OnSearchLocationChanged));
	}

	// 3. เริ่ม Behavior Tree (BT_Enemy: Selector -> [Chase: TargetActor Is Set (Both) -> Move To(TargetActor) -> Wait 0.5]
	//                                               | [Investigate: Alerted Is Set -> Move To(AlertLocation) -> Wait 2.0]
	//                                               | [Face (บทที่ 9)] | [Sequence -> Move To(PatrolLocation) -> Wait 1.0] )
	//    บทที่ 12: อินสแตนซ์ใน L_Ch12 ตั้ง BT_Enemy_EQS (สำเนาของ BT_Enemy ที่ Investigate มีส่วน Search
	//    ค้นหาจุดซ่อน; Chase ไม่เปลี่ยน) ส่วน BT_Enemy ไม่เปลี่ยน
	RunBehaviorTree(Enemy->BehaviorTreeAsset);

	// ผลที่คาด: LogTemp: [AI] Possessed BP_Enemy_C: 3 patrol points, Behavior Tree running
	UE_LOG(LogTemp, Log, TEXT("[AI] Possessed %s: %d patrol points, Behavior Tree running"),
		*InPawn->GetClass()->GetName(), Enemy->PatrolPoints.Num());
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	// บทที่ 11 (เครื่องมือวัด): Pawn ของผู้เล่นเกิดหลัง PostInitializeComponents ของ Actor ที่วางในฉาก (ซึ่งคือตอนที่ OnPossess ทำงาน)
	// แต่ก่อน BeginPlay ของ World จึงอ่านฝ่ายของผู้เล่นได้ที่นี่ รอหนึ่ง Tick เพื่อให้ไม่ขึ้นกับลำดับ BeginPlay ระหว่าง Actor
	const AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(GetPawn());
	if (Enemy != nullptr && Enemy->bPerceptionEnabled)
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &AEnemyAIController::LogTeamAttitude);
	}
}

void AEnemyAIController::SetPerceptionActive(bool bActive)
{
	// Component ถูกเพิ่มใน BP_EnemyAIController (Add Component) — ระหว่าง Spawn ของ Controller ซึ่งเกิดก่อน Possess
	// OnRegister ของ Component ลงทะเบียน Sense และ Listener กับ AIPerceptionSystem และใส่ตัวเองลงช่อง PerceptionComponent ของ AAIController (AIPerceptionComponent.cpp: OnRegister)
	// นี่คือทางเดียว: AAIController::PostRegisterAllComponents (FindComponentByClass) ทำงานก่อนที่ Component ของ Blueprint ถูกสร้าง จึงพบเฉพาะ Component ของ C++
	UAIPerceptionComponent* Perception = GetAIPerceptionComponent();
	if (Perception == nullptr)
	{
		if (bActive)
		{
			// AI Controller Class ของ BP_Enemy ยังเป็นคลาส C++ (ไม่มี Component) แทน BP_EnemyAIController
			UE_LOG(LogTemp, Warning, TEXT("[Perception] %s: Perception Enabled but the controller has no AIPerception component (set AI Controller Class to BP_EnemyAIController)"),
				*GetNameSafe(GetPawn()));
		}
		return;
	}

	if (bActive)
	{
		// สะพานเชื่อม: Dynamic Multicast Delegate ผูกด้วยชื่อผ่าน Reflection (ปลายทางต้องเป็น UFUNCTION)
		Perception->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
		// ยืนยันการลงทะเบียน Listener (ลงทะเบียนใหม่หากเคยถูกถอน; หากลงทะเบียนอยู่แล้ว Sense เพียงสร้าง Query ของ Listener นี้ใหม่)
		Perception->RequestStimuliListenerUpdate();
	}
	else
	{
		Perception->OnTargetPerceptionUpdated.RemoveDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
		// ถอน Listener ออกจากระบบกลาง: Sense ทุกตัวลบ Query ของ Listener นี้ ไม่มี Line Trace และไม่มีสิ่งเร้า (AIPerceptionSystem.cpp: UnregisterListener)
		if (UAIPerceptionSystem* PerceptionSystem = UAIPerceptionSystem::GetCurrent(this))
		{
			PerceptionSystem->UnregisterListener(*Perception);
		}
	}
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (BlackboardComp == nullptr || Actor == nullptr)
	{
		return;
	}

	// บทที่ 13: ร่างที่ตายแล้วไม่รับรู้สิ่งใดเพิ่ม
	// (Listener ยังทำงานขณะร่างคงอยู่ 3 วินาที)
	// และผู้ที่ตายแล้วไม่ใช่ภัยคุกคาม
	if (UHealthComponent::IsActorDead(GetPawn())
		|| UHealthComponent::IsActorDead(Actor))
	{
		return;
	}

	const TSubclassOf<UAISense> SenseClass = UAIPerceptionSystem::GetSenseClassForStimulus(this, Stimulus);

	// นโยบายของตัวรับ: ระบบรับรู้รายงานทุกสิ่งที่ผ่าน Config แต่ AI รู้เฉพาะสิ่งที่ตัวรับเลือกเขียน
	// เขียนความรู้ลง Blackboard เฉพาะเมื่อความสัมพันธ์ต่อ Actor เป็น Hostile (ถามสดผ่าน Attitude Solver ของงานเขียนนี้ทุกครั้ง)
	const bool bHostile = GetTeamAttitudeTowards(*Actor) == ETeamAttitude::Hostile;

	if (SenseClass == UAISense_Sight::StaticClass())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			// เครื่องมือวัด: บันทึกทุกสิ่งที่เห็น ไม่ว่าฝ่ายใด
			// ผลที่คาด: LogTemp: [Perception] Sight gained: BP_ThirdPersonCharacter_C
			UE_LOG(LogTemp, Log, TEXT("[Perception] Sight gained: %s"), *PerceivedName(Actor));

			if (bHostile)
			{
				// ความจำของเป้าหมาย (ตำแหน่งปัจจุบัน) แล้วเป้าหมาย: Decorator ของ Branch Chase (Is Set, Both) ยุติ Branch ที่สำคัญต่ำกว่าในการเขียนนี้
				BlackboardComp->SetValueAsVector(LastKnownLocationKey, Stimulus.StimulusLocation);
				BlackboardComp->SetValueAsObject(TargetActorKey, Actor);
			}
		}
		else if (bHostile)
		{
			// Sight รายงาน "หาย" เอง (FAIStimulus::SensingFailed) เมื่อแนวสายตาขาดหรือเป้าหมายพ้น LoseSightRadius
			// ลำดับการเขียนมีความหมาย: ความจำ -> จุดหมาย -> สวิตช์ -> ล้างเป้าหมาย
			//   (ก) LastKnownLocation และ AlertLocation = ตำแหน่งที่พบล่าสุด (นำกลไก Investigate ของบทที่ 8 กลับมาใช้)
			//   (ข) Alerted = true ขณะที่ TargetActor ยังตั้งอยู่: ผู้สังเกตของบทที่ 8 เห็นว่ากำลังไล่ตาม จึงไม่ตั้งตัวตรวจ "Patrol aborted"
			//   (ค) ล้าง TargetActor: Decorator (Both) ยุติ Branch ของตนเอง (Self) Selector ตกไป Branch Investigate ซึ่งพร้อมแล้ว
			BlackboardComp->SetValueAsVector(LastKnownLocationKey, Stimulus.StimulusLocation);
			BlackboardComp->SetValueAsVector(AlertLocationKey, Stimulus.StimulusLocation);
			BlackboardComp->SetValueAsBool(AlertedKey, true);

			// ผลที่คาด: LogTemp: [Perception] Sight lost: BP_ThirdPersonCharacter_C -> investigating last known location
			UE_LOG(LogTemp, Log, TEXT("[Perception] Sight lost: %s -> investigating last known location"), *PerceivedName(Actor));

			BlackboardComp->ClearValue(TargetActorKey);
		}
		// สิ่งที่ไม่ใช่ Hostile หายจากสายตา: ไม่มีความรู้ให้ลบ ไม่มีการตรวจสอบ และไม่มีบรรทัด
	}
	else if (SenseClass == UAISense_Hearing::StaticClass() && Stimulus.WasSuccessfullySensed() && bHostile)
	{
		// เสียง: ไม่มี Actor ให้ไล่ตาม (ได้ยินแต่ไม่เห็น) จึงใช้กลไก Investigate ของบทที่ 8 กับตำแหน่งของสิ่งเร้า
		// หากกำลังไล่ตามอยู่ Branch Chase สำคัญกว่าและไม่ถูกกระทบ (Lower Priority ของ Investigate ยุติเฉพาะ Branch ที่ต่ำกว่าตน)
		BlackboardComp->SetValueAsVector(AlertLocationKey, Stimulus.StimulusLocation);
		BlackboardComp->SetValueAsBool(AlertedKey, true);

		// ผลที่คาด: LogTemp: [Perception] Heard: BP_ThirdPersonCharacter_C -> investigating noise
		UE_LOG(LogTemp, Log, TEXT("[Perception] Heard: %s -> investigating noise"), *PerceivedName(Actor));
	}
	// บทที่ 13: Sense Damage — สิ่งเร้าชี้ไปยังผู้กระทำ
	// และไม่ผ่านการกรองฝ่าย
	else if (SenseClass == UAISense_Damage::StaticClass()
		&& Stimulus.WasSuccessfullySensed())
	{
		// เครื่องมือวัด: ทุกการโจมตี ไม่ว่าผู้กระทำเป็นฝ่ายใด
		// ผลที่คาด: LogTemp: [Perception] Damage from: BP_ThirdPersonCharacter_C
		UE_LOG(LogTemp, Log,
			TEXT("[Perception] Damage from: %s"),
			*PerceivedName(Actor));

		if (bHostile)
		{
			// ผู้กระทำเป็นเป้าหมายแม้ยังมองไม่เห็น: Decorator
			// (Both) ของ Chase ยุติ Branch ที่ต่ำกว่าทันที
			// ไล่ตามผู้กระทำอยู่แล้ว: ค่าเดิม ไม่มีสิ่งใดเปลี่ยน
			BlackboardComp->SetValueAsVector(LastKnownLocationKey,
				Stimulus.StimulusLocation);
			BlackboardComp->SetValueAsObject(TargetActorKey,
				Actor);
		}
	}
}

void AEnemyAIController::LogTeamAttitude()
{
	const APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (PlayerPawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Perception] Teams: player pawn not found"));
		return;
	}

	// ทั้งสองค่าผ่านเส้นทางจริงของเอนจิ้น: GetTeamIdentifier Cast Pawn เป็น Interface; GetTeamAttitudeTowards -> FGenericTeamId::GetAttitude -> Attitude Solver
	// ผู้เล่นที่ไม่มี Interface (ยังไม่ Reparent) อ่านว่า player=255 attitude=Neutral — การวินิจฉัยอยู่ในบรรทัดนี้
	// ผลที่คาด: LogTemp: [Perception] Teams: enemy=2 player=1 attitude=Hostile
	UE_LOG(LogTemp, Log, TEXT("[Perception] Teams: enemy=%d player=%d attitude=%s"),
		GetGenericTeamId().GetId(),
		FGenericTeamId::GetTeamIdentifier(PlayerPawn).GetId(),
		AttitudeToString(GetTeamAttitudeTowards(*PlayerPawn)));
}

FString AEnemyAIController::PerceivedName(const AActor* Actor)
{
	if (const ATargetDummy* Dummy = Cast<ATargetDummy>(Actor))
	{
		return Dummy->DummyName.ToString();   // Dummy_01 (บทที่ 9) ไม่ใช่ TargetDummy_C
	}
	return Actor->GetClass()->GetName();      // BP_ThirdPersonCharacter_C เช่นเดียวกับบรรทัดของบทที่ 10
}

void AEnemyAIController::RaiseAlert(const FVector& Location, FName SourceName)
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (BlackboardComp == nullptr)
	{
		return;
	}

	// จุดหมายก่อนสวิตช์: ทันทีที่ Alerted เป็นจริง Decorator ร้องขอการยุติ และ Move To ของ Branch Investigate จะอ่าน AlertLocation
	BlackboardComp->SetValueAsVector(AlertLocationKey, Location);

	// ผลที่คาด: LogTemp: [BT] Alert raised at AlertZone
	UE_LOG(LogTemp, Log, TEXT("[BT] Alert raised at %s"), *SourceName.ToString());

	BlackboardComp->SetValueAsBool(AlertedKey, true);   // ผู้สังเกตทุกราย (รวม Decorator) ถูกเรียกภายในบรรทัดนี้
}

EBlackboardNotificationResult AEnemyAIController::OnAlertedChanged(const UBlackboardComponent& ChangedBlackboard, FBlackboard::FKey ChangedKey)
{
	// บทที่ 11: ขณะไล่ตาม (TargetActor ตั้งอยู่) สัญญาณเตือนไม่ยุติการลาดตระเวน เพราะไม่ได้ลาดตระเวนอยู่ — เครื่องมือวัดของบทที่ 8 จึงไม่ทำงาน
	if (ChangedBlackboard.GetValueAsBool(AlertedKey) && ChangedBlackboard.GetValueAsObject(TargetActorKey) == nullptr)
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

	// คำขอเคลื่อนที่ปัจจุบันมุ่งไป AlertLocation = Branch Investigate เข้ามาแทนที่ Branch ลาดตระเวนแล้ว (สังเกตได้จริง ไม่ใช่อนุมานจากค่าที่ตั้ง)
	const bool bMovingToAlert = PathComp->HasValidPath()
		&& FVector::Dist2D(PathComp->GetPathDestination(), BlackboardComp->GetValueAsVector(AlertLocationKey)) <= ArrivalRadius;

	if (bMovingToAlert)
	{
		// ผลที่คาด: LogTemp: [BT] Patrol aborted by Observer Aborts (Lower Priority)
		UE_LOG(LogTemp, Log, TEXT("[BT] Patrol aborted by Observer Aborts (Lower Priority)"));
	}
	else
	{
		// Observer Aborts ของ Decorator เป็น None: ค่าคีย์เปลี่ยนแล้ว แต่ Behavior Tree ยังอยู่ Branch ลาดตระเวนจนจบรอบ (แบบฝึกหัดข้อ 4)
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

	// บทที่ 11: การถึงเป้าหมายที่ไล่ตามไม่เลื่อนจุดลาดตระเวนและไม่ล้างคีย์ใด Branch Chase วน Move To -> Wait 0.5 ต่อจนกว่า Sight จะรายงานว่าหาย
	if (WasChaseArrival())
	{
		return;
	}

	// บทที่ 8: การมาถึง AlertLocation ไม่เลื่อนจุดลาดตระเวน และล้าง Alerted ให้ Decorator เห็นค่าเท็จ
	// (ด้วย Observer Aborts แบบ Lower Priority Decorator ไม่ยุติ Branch ของตนเอง Wait 2.0 จึงเล่นจนจบ แล้ว Selector ตกไป Branch ลาดตระเวน)
	// บทที่ 12: กลไกเดียวกันนี้ทำให้การค้นหาจุดซ่อนของ BT_Enemy_EQS ทำงานต่อหลังบรรทัดนี้
	if (WasInvestigateArrival())
	{
		GetBlackboardComponent()->ClearValue(AlertedKey);
		// ผลที่คาด: LogTemp: [BT] Investigate: reached AlertLocation, Alerted cleared
		UE_LOG(LogTemp, Log, TEXT("[BT] Investigate: reached AlertLocation, Alerted cleared"));
		return;
	}

	// บทที่ 12: การถึงจุดซ่อนไม่เลื่อนจุดลาดตระเวน
	if (WasSearchArrival())
	{
		// ใช้ค่าแล้ว: จุดซ่อนเก่าจะไม่ถูกนับซ้ำเมื่อลาดตระเวนผ่าน
		GetBlackboardComponent()->ClearValue(SearchLocationKey);
		return;
	}

	// กฎคงที่ (บันทึกจากการถดถอยของบทที่ 12): ทุกคำขอที่สำเร็จของ Branch ลาดตระเวนต้องมาถึงบรรทัดนี้
	// บทที่เพิ่ม Branch อื่นในภายหลังต้องจำแนกการมาถึงจาก "จุดหมายของคำขอที่เพิ่งจบ" (คีย์ Blackboard + ตำแหน่ง Pawn)
	// ไม่ใช่จากสวิตช์โหมดของตัวละคร เพราะ Selector อาจตกมา Branch ลาดตระเวนขณะสวิตช์นั้นยังเปิดอยู่
	// และ UPathFollowingComponent::OnPathFinished() ล้างเส้นทางก่อนแจ้ง OnMoveCompleted() จึงอ่านจุดหมายจากเส้นทางไม่ได้
	const int32 Count = Enemy->PatrolPoints.Num();
	// ผลที่คาด: LogTemp: [AI] Reached waypoint 1 of 3 (แล้ว 2 of 3, 3 of 3, วนกลับ 1 of 3 ...)
	UE_LOG(LogTemp, Log, TEXT("[AI] Reached waypoint %d of %d"), CurrentPatrolIndex + 1, Count);

	CurrentPatrolIndex = (CurrentPatrolIndex + 1) % Count;   // วนกลับไปจุดแรกหลังจุดสุดท้าย
	WriteCurrentPatrolLocation();                             // Node Move To รอบถัดไปอ่านค่าใหม่นี้
}

bool AEnemyAIController::WasChaseArrival() const
{
	const UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	const APawn* ControlledPawn = GetPawn();
	if (BlackboardComp == nullptr || ControlledPawn == nullptr)
	{
		return false;
	}

	// BB_Enemy ของบทที่ 7–10 ไม่มีคีย์นี้ (GetValueAsObject คืน nullptr) และไม่มีสิ่งใดเขียนคีย์เมื่อ Listener ถูกถอน (Perception Enabled เป็นเท็จ)
	const AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey));
	if (Target == nullptr)
	{
		return false;
	}

	return FVector::Dist2D(ControlledPawn->GetActorLocation(), Target->GetActorLocation()) <= ChaseArrivalRadius;
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

// ---- บทที่ 12: ผลของ EQS ใน Blackboard ----

EBlackboardNotificationResult
AEnemyAIController::OnSearchLocationChanged(
	const UBlackboardComponent& ChangedBlackboard,
	FBlackboard::FKey /*ChangedKey*/)
{
	// Run EQS Query เขียนคีย์ด้วย SetValue ผู้สังเกตจึงถูกเรียก
	// ทันที และเฉพาะเมื่อค่าเปลี่ยน (จุดเดิมซ้ำไม่มีบรรทัดใหม่)
	// ค่าถูกล้าง (Query ล้มเหลว หรือการมาถึงใช้ค่าแล้ว):
	// ไม่มีบรรทัด
	if (!ChangedBlackboard.IsVectorValueSet(SearchLocationKey))
	{
		return EBlackboardNotificationResult::ContinueObserving;
	}

	// ผลที่คาด: LogTemp: [EQS] Search: checking hidden spot
	UE_LOG(LogTemp, Log,
		TEXT("[EQS] Search: checking hidden spot"));
	DrawSearchLocation(
		ChangedBlackboard.GetValueAsVector(SearchLocationKey));
	return EBlackboardNotificationResult::ContinueObserving;
}

void AEnemyAIController::DrawSearchLocation(
	const FVector& Location) const
{
#if ENABLE_DRAW_DEBUG
	// เครื่องมือวัดเท่านั้น: Shipping และ Test ไม่วาด Debug
	DrawDebugSphere(GetWorld(), Location, SearchSphereRadius, 12,
		FColor::Orange, false, SearchSphereSeconds);
#endif
}

bool AEnemyAIController::WasSearchArrival() const
{
	const UBlackboardComponent* BlackboardComp =
		GetBlackboardComponent();
	const APawn* ControlledPawn = GetPawn();
	// BB_Enemy (บทที่ 7–11) ไม่มีคีย์นี้: คืนเท็จเสมอ
	if (BlackboardComp == nullptr || ControlledPawn == nullptr
		|| !BlackboardComp->IsVectorValueSet(SearchLocationKey))
	{
		return false;
	}
	const FVector Point =
		BlackboardComp->GetValueAsVector(SearchLocationKey);
	const FVector Here = ControlledPawn->GetActorLocation();
	return FVector::Dist2D(Here, Point) <= ArrivalRadius;
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
