// บทที่ 12 — การนิยาม UEnvQueryContext_LastKnownLocation

#include "EnvQueryContext_LastKnownLocation.h"
#include "EnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"

void UEnvQueryContext_LastKnownLocation::ProvideContext(
	FEnvQueryInstance& QueryInstance,
	FEnvQueryContextData& ContextData) const
{
	const UBlackboardComponent* BlackboardComp =
		FindQuerierBlackboard(QueryInstance);
	if (BlackboardComp == nullptr)
	{
		// Querier ไม่ใช่ AI: ตัวช่วยสาธิตแทนตำแหน่งที่พบล่าสุด
		Super::ProvideContext(QueryInstance, ContextData);
		return;
	}

	const FName Key = AEnemyAIController::LastKnownLocationKey;
	if (!BlackboardComp->IsVectorValueSet(Key))
	{
		// ยังไม่เคยเห็นเป้าหมาย: ไม่บันทึก Query ได้ 0 รายการ
		return;
	}

	UEnvQueryItemType_Point::SetContextHelper(
		ContextData, BlackboardComp->GetValueAsVector(Key));
}
