// บทที่ 12 — การนิยาม UEnvQueryContext_Target

#include "EnvQueryContext_Target.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

const FName UEnvQueryContext_Target::TeachingAidTag(
	TEXT("EQSTeachingAid"));

void UEnvQueryContext_Target::ProvideContext(
	FEnvQueryInstance& QueryInstance,
	FEnvQueryContextData& ContextData) const
{
	if (QueryInstance.World == nullptr)
	{
		return;
	}

	TArray<AActor*> Tagged;
	UGameplayStatics::GetAllActorsWithTag(
		QueryInstance.World, TeachingAidTag, Tagged);
	if (Tagged.IsEmpty())
	{
		// ไม่บันทึก Context: Generator และ Test ไม่ได้ข้อมูล
		return;
	}

	UEnvQueryItemType_Actor::SetContextHelper(
		ContextData, Tagged[0]);
}

const UBlackboardComponent*
UEnvQueryContext_Target::FindQuerierBlackboard(
	const FEnvQueryInstance& QueryInstance)
{
	const UObject* Querier = QueryInstance.Owner.Get();

	// Run EQS Query ส่ง Pawn เป็นผู้ถาม; FEnvQueryRequest อาจส่ง
	// Controller มาเอง จึงรองรับทั้งสองกรณี
	const AAIController* AI = Cast<AAIController>(Querier);
	if (const APawn* Pawn = Cast<APawn>(Querier))
	{
		AI = Cast<AAIController>(Pawn->GetController());
	}
	if (AI == nullptr)
	{
		return nullptr;
	}
	return AI->GetBlackboardComponent();
}
