// บทที่ 9 — ตัวสร้างศัตรูสำหรับการวัด (รุ่นที่ 2, 2026-10-03): สร้างศัตรู N ตัวบน NavMesh เมื่อเริ่มเล่น
// แต่ละตัวมีเส้นทางลาดตระเวนของตนเอง (จุดสุ่มที่เดินถึงได้สามจุดรอบจุดเกิด) จึงไม่เบียดกันที่จุดเดียว
// และสร้างหุ่น TargetDummy K ตัวรอบสนามก่อนศัตรู เพื่อให้จำนวนเป้าหมายที่เป็นไปได้ (ภาระงานจริงของ Service) ปรับได้
// กำหนด Behavior Tree ให้ศัตรูที่สร้าง (สลับ BT_Enemy_BPService / BT_Enemy ได้โดยไม่แก้ BP_Enemy)
// ใช้อีกครั้งในบทประเมินผลเชิงปริมาณ (ส่วนที่ 4)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

class AEnemyCharacter;
class ATargetDummy;
class UBehaviorTree;
class USceneComponent;
class UBillboardComponent;
class UNavigationSystemV1;

UCLASS()
class HAPBOOK_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	AEnemySpawner();

	// Actor ที่ไม่มี Root Component วางในแผนที่หรือย้ายไม่ได้ (ไม่มี Transform): Root คือจุดศูนย์กลางของสนาม
	UPROPERTY(VisibleAnywhere, Category = "Spawner")
	TObjectPtr<USceneComponent> Root;

#if WITH_EDITORONLY_DATA
	// ไอคอนใน Viewport ของ Editor เท่านั้น: ให้มองเห็น Spawner ที่วางไว้ ไม่มีผลขณะเล่น
	UPROPERTY(VisibleAnywhere, Category = "Spawner")
	TObjectPtr<UBillboardComponent> EditorIcon;
#endif

	// จำนวนศัตรูที่สร้าง (N ของตารางการวัด; 0 = ไม่สร้าง สำหรับการเล่นปกติ)
	UPROPERTY(EditAnywhere, Category = "Spawner", meta = (ClampMin = "0"))
	int32 SpawnCount = 0;

	// จำนวนหุ่น TargetDummy ที่สร้างเพิ่มรอบสนาม (K ของตารางการวัด = หุ่นที่วางไว้ในแผนที่ + DummyCount)
	UPROPERTY(EditAnywhere, Category = "Spawner", meta = (ClampMin = "0"))
	int32 DummyCount = 0;

	// ระยะห่างของตารางจุดเกิด (หน่วย) แต่ละจุดถูกฉายลง NavMesh ก่อนใช้
	UPROPERTY(EditAnywhere, Category = "Spawner", meta = (ClampMin = "100"))
	float GridSpacing = 300.0f;

	// รัศมีของจุดลาดตระเวนสุ่มรอบจุดเกิดของศัตรูแต่ละตัว (หน่วย)
	UPROPERTY(EditAnywhere, Category = "Spawner", meta = (ClampMin = "100"))
	float PatrolRadius = 1500.0f;

	// รัศมีรอบ Spawner ที่วางหุ่น (หน่วย)
	UPROPERTY(EditAnywhere, Category = "Spawner", meta = (ClampMin = "100"))
	float ArenaRadius = 2500.0f;

	// คลาสของศัตรู (BP_Enemy หรือ BP_Enemy_Bench สำหรับการวัดที่ตัดภาระ Animation ออก)
	UPROPERTY(EditAnywhere, Category = "Spawner")
	TSubclassOf<AEnemyCharacter> EnemyClass;

	// คลาสของหุ่น (TargetDummy)
	UPROPERTY(EditAnywhere, Category = "Spawner")
	TSubclassOf<ATargetDummy> DummyClass;

	// Behavior Tree ที่ใช้แทนค่าใน Class Defaults ของ BP_Enemy (ว่าง = ใช้ค่าเดิม)
	UPROPERTY(EditAnywhere, Category = "Spawner")
	TObjectPtr<UBehaviorTree> BehaviorTreeOverride;

protected:
	virtual void BeginPlay() override;

private:
	// สร้างหุ่น K ตัวบน NavMesh รอบ Spawner ตั้ง DummyName = Dummy_01..Dummy_K; คืนจำนวนที่สร้างได้
	int32 SpawnDummies(UNavigationSystemV1& NavSys);

	// สร้างศัตรู N ตัว: ฉายจุดในตารางลง NavMesh (ข้ามช่องที่ฉายไม่ได้) และให้เส้นทางลาดตระเวนสุ่มสามจุดต่อตัว
	int32 SpawnEnemies(UNavigationSystemV1& NavSys, int32& OutSkipped);
};
