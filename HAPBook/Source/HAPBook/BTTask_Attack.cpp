// บทที่ 13 — การนิยาม UBTTask_Attack

#include "BTTask_Attack.h"
#include "DamageType_Melee.h"
#include "EnemyAIController.h"
#include "HealthComponent.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Kismet/GameplayStatics.h"

UBTTask_Attack::UBTTask_Attack()
{
	NodeName = TEXT("Attack Target");

	// รับเฉพาะคีย์ชนิด Actor; ชื่อเริ่มต้นมาจาก Controller
	// ซึ่งเป็นเจ้าของชื่อคีย์เพียงหนึ่งเดียว
	TargetActorKey.AddObjectFilter(this,
		GET_MEMBER_NAME_CHECKED(UBTTask_Attack, TargetActorKey),
		AActor::StaticClass());
	TargetActorKey.SelectedKeyName =
		AEnemyAIController::TargetActorKey;

	DamageTypeClass = UDamageType_Melee::StaticClass();
}

void UBTTask_Attack::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		TargetActorKey.ResolveSelectedKey(*BBAsset);
	}
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	APawn* Self = AI ? AI->GetPawn() : nullptr;
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	const FName Key = TargetActorKey.SelectedKeyName;
	AActor* Target = BB
		? Cast<AActor>(BB->GetValueAsObject(Key)) : nullptr;
	if (Self == nullptr || Target == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	// ช่องว่างระหว่างแคปซูลในระนาบ XY
	// (Move To หยุดเมื่อช่องว่างนี้ไม่เกิน Acceptable Radius)
	float SelfRadius = 0.0f, TargetRadius = 0.0f, Half = 0.0f;
	Self->GetSimpleCollisionCylinder(SelfRadius, Half);
	Target->GetSimpleCollisionCylinder(TargetRadius, Half);
	const FVector ToTarget =
		Target->GetActorLocation() - Self->GetActorLocation();
	const float Gap =
		ToTarget.Size2D() - SelfRadius - TargetRadius;
	if (Gap > AttackRange)
	{
		// นอกระยะ: Selector ไปยัง Branch Chase
		return EBTNodeResult::Failed;
	}

	// หันหาเป้าหมายก่อนโจมตี (แบบ Face Target ของบทที่ 9)
	const FRotator Facing(0.0f, ToTarget.Rotation().Yaw, 0.0f);
	Self->SetActorRotation(Facing);

	// Damage Pipeline ของบทที่ 10: Instigator = Controller
	// (ผู้รับผิดชอบคำสั่ง), Damage Causer = ร่าง (ผู้ลงมือ)
	UGameplayStatics::ApplyDamage(Target, Damage, AI, Self,
		DamageTypeClass);

	// เครื่องมือวัด: พลังชีวิตของเป้าหมายหลังการโจมตี
	const UHealthComponent* Health =
		Target->FindComponentByClass<UHealthComponent>();
	if (Health != nullptr)
	{
		// ผลที่คาด: LogTemp: [Combat] Attack: BP_ThirdPersonCharacter_C -> 190.00/200.00
		UE_LOG(LogTemp, Log,
			TEXT("[Combat] Attack: %s -> %.2f/%.2f"),
			*Target->GetClass()->GetName(),
			Health->CurrentHealth, Health->MaxHealth);

		// เป้าหมายที่ตายแล้วไม่ใช่เป้าหมาย: จบการต่อสู้
		if (Health->IsDead())
		{
			BB->ClearValue(Key);
		}
	}
	return EBTNodeResult::Succeeded;
}

FString UBTTask_Attack::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s\n%s within %.0f: %.0f damage"),
		*Super::GetStaticDescription(),
		*TargetActorKey.SelectedKeyName.ToString(),
		AttackRange, Damage);
}
