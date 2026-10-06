// บทที่ 12 — การนิยาม UEnvQueryContext_PerceivedTarget

#include "EnvQueryContext_PerceivedTarget.h"
#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UEnvQueryContext_PerceivedTarget::ProvideContext(
	FEnvQueryInstance& QueryInstance,
	FEnvQueryContextData& ContextData) const
{
	const UBlackboardComponent* BlackboardComp =
		FindQuerierBlackboard(QueryInstance);
	if (BlackboardComp == nullptr)
	{
		// Querier ไม่ใช่ AI (EQS Testing Pawn): ใช้ตัวช่วยสาธิต
		Super::ProvideContext(QueryInstance, ContextData);
		return;
	}

	// ชื่อคีย์มีเจ้าของเดียว: ค่าคงที่ของ Controller (บทที่ 11)
	const AActor* Target = Cast<AActor>(
		BlackboardComp->GetValueAsObject(
			AEnemyAIController::TargetActorKey));
	if (Target == nullptr)
	{
		// ไม่ได้รับรู้เป้าหมายใด: ไม่บันทึก Query ได้ 0 รายการ
		return;
	}

	UEnvQueryItemType_Actor::SetContextHelper(
		ContextData, Target);
}
