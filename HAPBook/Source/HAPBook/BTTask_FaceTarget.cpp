// บทที่ 9 — การนิยาม UBTTask_FaceTarget

#include "BTTask_FaceTarget.h"
#include "TargetDummy.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"

UBTTask_FaceTarget::UBTTask_FaceTarget()
{
	NodeName = TEXT("Face Target");
	NearestTargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FaceTarget, NearestTargetKey), ATargetDummy::StaticClass());
	LastFacedTargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_FaceTarget, LastFacedTargetKey), ATargetDummy::StaticClass());
}

void UBTTask_FaceTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		NearestTargetKey.ResolveSelectedKey(*BBAsset);
		LastFacedTargetKey.ResolveSelectedKey(*BBAsset);
	}
}

EBTNodeResult::Type UBTTask_FaceTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	APawn* Pawn = Controller ? Controller->GetPawn() : nullptr;
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	ATargetDummy* Target = Blackboard
		? Cast<ATargetDummy>(Blackboard->GetValueAsObject(
			NearestTargetKey.SelectedKeyName))
		: nullptr;
	if (Pawn == nullptr || Target == nullptr)
	{
		return EBTNodeResult::Failed;   // ข้อมูลไม่ครบ: ล้มเหลวอย่างชัดเจน ไม่ค้าง
	}

	// พฤติกรรม: หมุนร่างทันที (AEnemyCharacter ตั้ง bUseControllerRotationYaw = false ทิศของ Controller จึงไม่หมุนร่าง
	// และ bOrientRotationToMovement จะหมุนกลับเมื่อเดินต่อ) ไม่ตั้งจุดสนใจ: เมื่อไม่มีจุดสนใจ ทิศการมองของ Controller
	// ตามทิศของร่าง (หันหาหุ่นระหว่าง Wait) และตามทิศเดินเมื่อเดินต่อ
	const FVector ToTarget = Target->GetActorLocation() - Pawn->GetActorLocation();
	Pawn->SetActorRotation(FRotator(0.0f, ToTarget.Rotation().Yaw, 0.0f));
	Blackboard->SetValueAsObject(LastFacedTargetKey.SelectedKeyName, Target);   // หันครั้งเดียวต่อเป้าหมาย

	// ผลที่คาด: LogTemp: [Nodes] Task: facing Dummy_01
	UE_LOG(LogTemp, Log, TEXT("[Nodes] Task: facing %s"), *Target->DummyName.ToString());
	return EBTNodeResult::Succeeded;
}

FString UBTTask_FaceTarget::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s\nface: %s"), *Super::GetStaticDescription(), *NearestTargetKey.SelectedKeyName.ToString());
}
