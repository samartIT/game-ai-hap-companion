// บทที่ 9 — การนิยาม UBTDecorator_TargetInRange

#include "BTDecorator_TargetInRange.h"
#include "TargetDummy.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"

UBTDecorator_TargetInRange::UBTDecorator_TargetInRange()
{
	NodeName = TEXT("Target In Range");

	// BlackboardKey (ของคลาสฐาน) = คีย์ที่ถูกสังเกต: DistanceToTarget (Float)
	BlackboardKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_TargetInRange, BlackboardKey));
	NearestTargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_TargetInRange, NearestTargetKey), ATargetDummy::StaticClass());
	LastFacedTargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_TargetInRange, LastFacedTargetKey), ATargetDummy::StaticClass());
}

void UBTDecorator_TargetInRange::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);   // คลาสฐานแปลง BlackboardKey ให้แล้ว
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		NearestTargetKey.ResolveSelectedKey(*BBAsset);
		LastFacedTargetKey.ResolveSelectedKey(*BBAsset);
	}
}

bool UBTDecorator_TargetInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (Blackboard == nullptr)
	{
		return false;
	}

	// อ่านและเปรียบเทียบเท่านั้น: ฟังก์ชันนี้ถูกเรียกทุกการค้นหาและทุกครั้งที่ DistanceToTarget เปลี่ยน
	const ATargetDummy* Target = Cast<ATargetDummy>(Blackboard->GetValueAsObject(NearestTargetKey.SelectedKeyName));
	const UObject* LastFaced = Blackboard->GetValueAsObject(LastFacedTargetKey.SelectedKeyName);
	const float Distance = Blackboard->GetValueAsFloat(BlackboardKey.SelectedKeyName);
	const bool bInRange = Target != nullptr && Distance < Range && LastFaced != Target;

	// เครื่องมือวัดเท่านั้น: บันทึกเฉพาะเมื่อผลเปลี่ยนเป็นจริง (ผลครั้งก่อนอยู่ในหน่วยความจำจำเพาะของ AI ตัวนี้)
	FBTTargetInRangeMemory* Memory = reinterpret_cast<FBTTargetInRangeMemory*>(NodeMemory);
	if (bInRange && !Memory->bWasInRange)
	{
		// ผลที่คาด: LogTemp: [Nodes] Decorator: Dummy_01 in range (800)
		UE_LOG(LogTemp, Log, TEXT("[Nodes] Decorator: %s in range (%d)"), *Target->DummyName.ToString(), FMath::RoundToInt(Range));
	}
	Memory->bWasInRange = bInRange;
	return bInRange;
}

FString UBTDecorator_TargetInRange::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s\n%s < %.0f and not yet faced"), *Super::GetStaticDescription(), *BlackboardKey.SelectedKeyName.ToString(), Range);
}
