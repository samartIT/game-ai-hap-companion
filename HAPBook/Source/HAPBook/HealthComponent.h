// บทที่ 10 — Component พลังชีวิต: ร่างกายที่บาดเจ็บได้ ประกอบขึ้นจาก Component ไม่ใช่จากการสืบทอด
// Component เดียวกันนี้แนบกับ AEnemyCharacter ด้วย CreateDefaultSubobject (C++) และกับ BP_ThirdPersonCharacter ด้วย Add Component (Blueprint)
// รับความเสียหายจาก Delegate OnTakeAnyDamage ของเจ้าของ (ปลายทางของ ApplyDamage -> TakeDamage) แล้วส่งต่อเป็น Delegate ที่ Blueprint ผูกได้
// ชั้นสะพานเชื่อมของ HAP ในทางปฏิบัติ: UPROPERTY ที่ปรับใน Editor + Delegate แบบ BlueprintAssignable ที่ผูกใน Event Graph
// บทที่ 13 เพิ่ม: รายงานเหตุความเสียหายทุกครั้งต่อระบบรับรู้ (UAISense_Damage::ReportDamageEvent) และ IsActorDead()

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

class UDamageType;
class AController;

// Delegate แบบ Dynamic Multicast: Blueprint ผูกได้ (BlueprintAssignable) และผู้ฟังหลายรายรับพร้อมกันได้
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, NewHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

// BlueprintSpawnableComponent: ทำให้คลาสนี้ปรากฏในเมนู Add Component ของ Blueprint Editor (ขั้นที่ 4: BP_ThirdPersonCharacter)
UCLASS(ClassGroup = (HAP), meta = (BlueprintSpawnableComponent))
class HAPBOOK_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	// พลังชีวิตสูงสุด: ค่าเริ่มต้นใน C++ ปรับได้ใน Class Defaults ของ Blueprint เจ้าของ (BP_Enemy 100, BP_ThirdPersonCharacter 200)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (ClampMin = "1.0"))
	float MaxHealth = 100.0f;

	// พลังชีวิตปัจจุบัน: อ่านได้จาก Blueprint และเห็นในแผง Details แต่แก้ไม่ได้ เปลี่ยนค่าได้ทางเดียวคือผ่านท่อความเสียหายของเอนจิ้น
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 0.0f;

	// เรียกทุกครั้งที่พลังชีวิตเปลี่ยน (ค่าใหม่, ค่าสูงสุด) — Blueprint ผูกได้จากแผง Details ของ Component (Events) หรือจาก Event Graph
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChangedSignature OnHealthChanged;

	// เรียกครั้งเดียวเมื่อพลังชีวิตถึงศูนย์
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeathSignature OnDeath;

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const { return CurrentHealth <= 0.0f; }

	// บทที่ 13: Actor นี้มี Component พลังชีวิตที่ตายแล้วหรือไม่
	// Actor ที่ไม่มี Component นี้ไม่ถือว่าตาย
	static bool IsActorDead(const AActor* Actor);

	// เครื่องมือวัด (instrumentation): บันทึกบรรทัด [Damage] ลง Output Log ปิดได้ใน Class Defaults โดยพฤติกรรมไม่เปลี่ยน
	UPROPERTY(EditAnywhere, Category = "Health|Debug")
	bool bLogDamage = true;

protected:
	// ผูก OnTakeAnyDamage ของเจ้าของที่นี่ (เจ้าของมีตัวตนแล้วและกำลังเริ่มเล่น) ไม่ใช่ใน Constructor ซึ่งเจ้าของอาจเป็นเพียง CDO
	virtual void BeginPlay() override;

private:
	// ปลายทางของท่อความเสียหาย: ApplyDamage -> TakeDamage -> OnTakeAnyDamage.Broadcast -> ฟังก์ชันนี้
	// ต้องเป็น UFUNCTION เพราะ Dynamic Delegate ผูกด้วยชื่อผ่าน Reflection
	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	// เครื่องมือวัด: ชื่อชนิดความเสียหายในบรรทัดบันทึก = ชื่อคลาสตัดคำนำหน้า "DamageType_" (UDamageType_Hazard -> Hazard)
	static FString DamageLabel(const UDamageType* DamageType);
	static const FString DamageTypePrefix;
};
