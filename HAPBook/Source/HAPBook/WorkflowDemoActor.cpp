// บทที่ 5 — การนิยาม AWorkflowDemoActor

#include "WorkflowDemoActor.h"
#include "Engine/World.h" // GetWorld(), SpawnActor

AWorkflowDemoActor::AWorkflowDemoActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AWorkflowDemoActor::BeginPlay()
{
	Super::BeginPlay();

	// ค่าเริ่มต้นมาจาก Class Defaults ของลูก Blueprint (BP_WorkflowDemo ตั้ง MaxHealth = 120)
	CurrentHealth = MaxHealth;
	UE_LOG(LogTemp, Log, TEXT("[Workflow] BeginPlay: MaxHealth=%d, CurrentHealth=%d"), MaxHealth, CurrentHealth);

	// C++ เป็นผู้กระทำ แล้วส่งสัญญาณให้ Blueprint ผ่าน OnHealthChanged
	ApplyDamage(30);

	// BlueprintNativeEvent: ถ้า Blueprint เขียนทับ จะได้ข้อความจาก Blueprint มิฉะนั้นได้ค่าเริ่มต้นจาก C++
	UE_LOG(LogTemp, Log, TEXT("[Workflow] DescribeSelf: %s"), *DescribeSelf());

	// TSubclassOf: C++ รู้เพียงว่าต้องการคลาสลูกของ AActor ส่วนคลาสจริงถูกกำหนดใน Class Defaults
	if (SpawnClass)
	{
		AActor* Spawned = GetWorld()->SpawnActor<AActor>(SpawnClass, GetActorTransform());
		if (Spawned != nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("[Workflow] Spawned: %s"), *Spawned->GetClass()->GetName());
		}
	}
}

void AWorkflowDemoActor::ApplyDamage(int32 Amount)
{
	CurrentHealth = FMath::Max(0, CurrentHealth - Amount);
	UE_LOG(LogTemp, Log, TEXT("[Workflow] ApplyDamage(%d) -> CurrentHealth=%d"), Amount, CurrentHealth);

	// เรียกเหตุการณ์ที่ Blueprint เป็นผู้กำหนดพฤติกรรม (ไม่มีตัวนิยามใน C++)
	OnHealthChanged(CurrentHealth);
}

FString AWorkflowDemoActor::DescribeSelf_Implementation()
{
	// ค่าเริ่มต้นฝั่ง C++; BP_WorkflowDemo เขียนทับให้คืนค่า "Blueprint override"
	return TEXT("C++ default");
}

void AWorkflowDemoActor::LogFromBlueprint(const FString& Message)
{
	UE_LOG(LogTemp, Log, TEXT("[Workflow][BP] %s"), *Message);
}
