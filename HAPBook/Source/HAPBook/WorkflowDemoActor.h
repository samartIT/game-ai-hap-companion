// บทที่ 5 — Actor สาธิตกระบวนการทำงานแบบผสมผสาน: คลาส C++ หนึ่งคลาสกับลูก Blueprint หนึ่งคลาส (BP_WorkflowDemo)
// คำนำหน้า A: คลาสที่สืบทอดจาก AActor (วางในฉากได้)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorkflowDemoActor.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

UCLASS(Blueprintable)
class HAPBOOK_API AWorkflowDemoActor : public AActor
{
	GENERATED_BODY()

public:
	AWorkflowDemoActor();

	// EditAnywhere: แก้ได้ทั้งใน Class Defaults และที่อินสแตนซ์ในฉาก; BlueprintReadWrite: Blueprint อ่านและเขียนได้
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Workflow")
	int32 MaxHealth = 100;

	// VisibleAnywhere: มองเห็นแต่แก้ไม่ได้ (ช่องสีเทาใน Details); BlueprintReadOnly: Blueprint อ่านได้อย่างเดียว
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Workflow")
	int32 CurrentHealth = 0;

	// EditDefaultsOnly: กำหนดได้เฉพาะใน Class Defaults; TSubclassOf กรองให้เลือกได้เฉพาะคลาสลูกของ AActor
	UPROPERTY(EditDefaultsOnly, Category = "Workflow")
	TSubclassOf<AActor> SpawnClass;

	// BlueprintCallable: ตรรกะอยู่ใน C++ ทั้งหมด Blueprint เรียกใช้ได้
	UFUNCTION(BlueprintCallable, Category = "Workflow")
	void ApplyDamage(int32 Amount);

	// BlueprintImplementableEvent: ไม่มีตัวนิยามใน .cpp; Blueprint เป็นผู้กำหนดพฤติกรรม
	UFUNCTION(BlueprintImplementableEvent, Category = "Workflow")
	void OnHealthChanged(int32 NewHealth);

	// BlueprintNativeEvent: C++ ให้ค่าเริ่มต้นใน DescribeSelf_Implementation; Blueprint เขียนทับได้
	UFUNCTION(BlueprintNativeEvent, Category = "Workflow")
	FString DescribeSelf();

	// BlueprintCallable: ให้ Blueprint ส่งข้อความกลับมาบันทึกลง Output Log ผ่าน C++
	UFUNCTION(BlueprintCallable, Category = "Workflow")
	void LogFromBlueprint(const FString& Message);

protected:
	virtual void BeginPlay() override;
};
