// บทที่ 10 — การนิยาม UHealthComponent

#include "HealthComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Controller.h"
#include "GameFramework/DamageType.h"
#include "Perception/AISense_Damage.h" // บทที่ 13: ReportDamageEvent

const FString UHealthComponent::DamageTypePrefix(TEXT("DamageType_"));

UHealthComponent::UHealthComponent()
{
	// ไม่มีงานต่อเฟรม: Component นี้ขับเคลื่อนด้วย Event ของท่อความเสียหายเท่านั้น
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	AActor* Owner = GetOwner();
	if (Owner == nullptr)
	{
		return;
	}

	// OnTakeAnyDamage เป็น Dynamic Multicast Delegate ของ AActor: AddDynamic ผูกฟังก์ชันสมาชิกที่เป็น UFUNCTION ด้วยชื่อ
	// Component ไม่ Override TakeDamage ของเจ้าของ (ทำไม่ได้: มันเป็นฟังก์ชันของ Actor) แต่รับผลจากปลายท่อแทน
	Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);

	if (bLogDamage)
	{
		// ผลที่คาด: LogTemp: [Damage] BP_Enemy_C health 100/100 (ready)
		UE_LOG(LogTemp, Log, TEXT("[Damage] %s health %.0f/%.0f (ready)"),
			*Owner->GetClass()->GetName(), CurrentHealth, MaxHealth);
	}
}

void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// เอนจิ้น Broadcast แม้ความเสียหายติดลบ (ดู AActor::TakeDamage) Component นี้นับเฉพาะความเสียหายบวก และไม่รับเพิ่มหลังตาย
	if (IsDead() || Damage <= 0.0f)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);

	if (bLogDamage)
	{
		// ผลที่คาด: LogTemp: [Damage] BP_Enemy_C took 25.00 (Hazard) -> 75.00/100.00
		UE_LOG(LogTemp, Log, TEXT("[Damage] %s took %.2f (%s) -> %.2f/%.2f"),
			*DamagedActor->GetClass()->GetName(), Damage, *DamageLabel(DamageType), CurrentHealth, MaxHealth);
	}

	// ชั้นสะพานเชื่อม: ตัวรับฟังทุกราย (C++ ของ AEnemyCharacter หรือ Event Graph ของ BP_ThirdPersonCharacter) รับค่าเดียวกัน
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	// บทที่ 13: รายงานเหตุนี้ต่อระบบรับรู้ (เอนจิ้นไม่รายงานเอง)
	// ผู้กระทำ = Pawn ของ Instigator หากมี มิฉะนั้น Damage Causer
	AActor* Attacker = DamageCauser;
	if (InstigatedBy != nullptr
		&& InstigatedBy->GetPawn() != nullptr)
	{
		Attacker = InstigatedBy->GetPawn();
	}
	if (Attacker != nullptr)
	{
		// EventLocation = ผู้กระทำ, HitLocation = ผู้ถูกกระทำ
		UAISense_Damage::ReportDamageEvent(this, DamagedActor,
			Attacker, Damage, Attacker->GetActorLocation(),
			DamagedActor->GetActorLocation());
	}

	if (IsDead())
	{
		if (bLogDamage)
		{
			// ผลที่คาด: LogTemp: [Damage] BP_Enemy_C died
			UE_LOG(LogTemp, Log, TEXT("[Damage] %s died"), *DamagedActor->GetClass()->GetName());
		}
		OnDeath.Broadcast();   // เจ้าของตัดสินใจเองว่า "ตาย" หมายถึงอะไร (ศัตรู: หยุด Behavior Tree ปิดการชน ทำลายใน 3 วินาที)
	}
}

bool UHealthComponent::IsActorDead(const AActor* Actor)
{
	const UHealthComponent* Health = Actor != nullptr
		? Actor->FindComponentByClass<UHealthComponent>()
		: nullptr;
	return Health != nullptr && Health->IsDead();
}

FString UHealthComponent::DamageLabel(const UDamageType* DamageType)
{
	// เครื่องมือวัดเท่านั้น: ตรรกะของเกมไม่เปรียบเทียบข้อความนี้ (แบบฝึกหัดเกราะใช้ IsA<UDamageType_Hazard>() แทน)
	if (DamageType == nullptr)
	{
		return TEXT("None");
	}
	FString Label = DamageType->GetClass()->GetName();
	Label.RemoveFromStart(DamageTypePrefix);
	return Label;
}
