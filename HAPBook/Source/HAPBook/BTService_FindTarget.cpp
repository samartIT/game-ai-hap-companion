// บทที่ 9 — การนิยาม UBTService_FindTarget

#include "BTService_FindTarget.h"
#include "TargetDummy.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Kismet/GameplayStatics.h"

UBTService_FindTarget::UBTService_FindTarget()
{
	NodeName = TEXT("Find Target");
	Interval = 0.1f;                 // เท่ากับต้นแบบ Blueprint
	RandomDeviation = 0.0f;
	bNotifyBecomeRelevant = true;    // ใช้ OnBecomeRelevant
	bNotifyTick = true;              // ใช้ TickNode

	NearestTargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_FindTarget, NearestTargetKey), ATargetDummy::StaticClass());
	DistanceToTargetKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_FindTarget, DistanceToTargetKey));
}

void UBTService_FindTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		NearestTargetKey.ResolveSelectedKey(*BBAsset);
		DistanceToTargetKey.ResolveSelectedKey(*BBAsset);
	}
}

void UBTService_FindTarget::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const
{
	if (InitType == EBTMemoryInit::Initialize)
	{
		new (NodeMemory) FBTFindTargetMemory();   // TArray ต้องถูกสร้างจริง ไม่ใช่แค่หน่วยความจำที่เติมด้วยศูนย์
	}
}

void UBTService_FindTarget::CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const
{
	if (CleanupType == EBTMemoryClear::Destroy)
	{
		reinterpret_cast<FBTFindTargetMemory*>(NodeMemory)->~FBTFindTargetMemory();
	}
}

void UBTService_FindTarget::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	FBTFindTargetMemory* Memory = reinterpret_cast<FBTFindTargetMemory*>(NodeMemory);
	Memory->Candidates.Reset();
	Memory->LastNearest.Reset();

	// รวบรวมครั้งเดียวต่อการเข้า Branch: GetAllActorsOfClass ค้นทั้งฉาก จึงไม่ทำใน TickNode (ความเป็นธรรมของการวัด)
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(OwnerComp.GetWorld(), ATargetDummy::StaticClass(), Found);
	for (AActor* Actor : Found)
	{
		Memory->Candidates.Add(Cast<ATargetDummy>(Actor));
	}
}

void UBTService_FindTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const AAIController* Controller = OwnerComp.GetAIOwner();
	const APawn* Pawn = Controller ? Controller->GetPawn() : nullptr;
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (Pawn == nullptr || Blackboard == nullptr)
	{
		return;
	}

	FBTFindTargetMemory* Memory = reinterpret_cast<FBTFindTargetMemory*>(NodeMemory);
	const FVector PawnLocation = Pawn->GetActorLocation();

	ATargetDummy* Best = nullptr;
	float BestDistance = TNumericLimits<float>::Max();
	for (const TWeakObjectPtr<ATargetDummy>& Candidate : Memory->Candidates)
	{
		ATargetDummy* Dummy = Candidate.Get();
		if (Dummy == nullptr)
		{
			continue;
		}
		const FVector ToTarget = Dummy->GetActorLocation() - PawnLocation;
		const float Distance = static_cast<float>(ToTarget.Size());
		if (Distance < BestDistance)   // ใกล้กว่าตัวที่ดีที่สุดเดิม (ไม่กรองด้วยทิศหน้า: เรื่องของ Decorator/Task)
		{
			Best = Dummy;
			BestDistance = Distance;
		}
	}

	if (Best != nullptr)
	{
		Blackboard->SetValueAsObject(NearestTargetKey.SelectedKeyName, Best);
		Blackboard->SetValueAsFloat(DistanceToTargetKey.SelectedKeyName, BestDistance);
	}
	else
	{
		Blackboard->ClearValue(NearestTargetKey.SelectedKeyName);
		Blackboard->ClearValue(DistanceToTargetKey.SelectedKeyName);
	}

	if (Memory->LastNearest.Get() != Best)   // เฉพาะเมื่อเป้าหมายเปลี่ยน (ต่อ AI ตัวนี้ เพราะอยู่ในหน่วยความจำจำเพาะ)
	{
		Memory->LastNearest = Best;
		if (bLogTargetChanges && Best != nullptr)
		{
			// ผลที่คาด: LogTemp: [Nodes] Service: nearest target changed -> Dummy_01
			UE_LOG(LogTemp, Log, TEXT("[Nodes] Service: nearest target changed -> %s"), *Best->DummyName.ToString());
		}
	}
}

FString UBTService_FindTarget::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s\nnearest: %s\ndistance: %s"), *Super::GetStaticDescription(),
		*NearestTargetKey.SelectedKeyName.ToString(), *DistanceToTargetKey.SelectedKeyName.ToString());
}
