// บทที่ 14 — Spawner ของชุดทดสอบ: Spawn Actor ทดสอบของสถาปัตยกรรมเดียว N ตัวเป็นตารางรอบตัวเองเมื่อเริ่มเล่น
// เขียน I ให้ทุกตัวก่อนตัวนั้นเริ่มทำงาน แล้วรายงานหนึ่งบรรทัดเพื่อยืนยันเซลล์ที่วัด (สถาปัตยกรรม, N, I)
// ไม่ใช่ AEnemySpawner ของบทที่ 9: Actor ทดสอบไม่เคลื่อนที่และไม่มีการชน จึงไม่ต้องใช้ NavMesh
// บรรทัดคำสั่ง -BenchArm= -BenchN= -BenchI= แทนค่าในแผง Details ทำให้แต่ละรอบเป็นการเปิดโปรแกรมใหม่โดยไม่แก้แผนที่
// -BenchAutoCapture: Spawner ส่ง csvprofile start/stop เองตามนาฬิกาจริง (อุ่นเครื่อง 10 วินาที บันทึก 60 วินาที)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BenchmarkSpawner.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

class ABenchmarkActorBase;
class USceneComponent;
class UBillboardComponent;

UCLASS()
class HAPBOOK_API ABenchmarkSpawner : public AActor
{
	GENERATED_BODY()

public:
	ABenchmarkSpawner();

	// แบบที่วัด: BP_Arm_A, BenchmarkActorCpp หรือ BP_Arm_C
	UPROPERTY(EditAnywhere, Category = "Benchmark")
	TSubclassOf<ABenchmarkActorBase> ArmClass;

	// N: จำนวน Actor ทดสอบ
	UPROPERTY(EditAnywhere, Category = "Benchmark",
		meta = (ClampMin = "0"))
	int32 Count = 100;

	// I: เขียนให้ Actor ทดสอบทุกตัว
	UPROPERTY(EditAnywhere, Category = "Benchmark",
		meta = (ClampMin = "1"))
	int32 Iterations = 1000;

	// ระยะห่างของตาราง (หน่วย)
	UPROPERTY(EditAnywhere, Category = "Benchmark",
		meta = (ClampMin = "0"))
	float GridSpacing = 300.0f;

	// จับเวลาการวัดเอง (-BenchAutoCapture เปิดได้เช่นกัน)
	UPROPERTY(EditAnywhere, Category = "Benchmark")
	bool bAutoCapture = false;

	UPROPERTY(EditAnywhere, Category = "Benchmark",
		meta = (ClampMin = "0"))
	float WarmupSeconds = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Benchmark",
		meta = (ClampMin = "1"))
	float CaptureSeconds = 60.0f;

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

private:
	// ค่าจากบรรทัดคำสั่งแทนค่าในแผง Details (ถ้ามี)
	void ReadCommandLine();

	// Spawn N ตัวเป็นตาราง คืนจำนวนที่สร้างได้
	int32 SpawnGrid();

	// Actor ที่ไม่มี Root Component วางในแผนที่ไม่ได้
	UPROPERTY(VisibleAnywhere, Category = "Benchmark")
	TObjectPtr<USceneComponent> Root;

#if WITH_EDITORONLY_DATA
	// ไอคอนใน Viewport ของ Editor เท่านั้น ไม่มีผลขณะเล่น
	UPROPERTY(VisibleAnywhere, Category = "Benchmark")
	TObjectPtr<UBillboardComponent> EditorIcon;
#endif

	double StartSeconds = 0.0;  // นาฬิกาจริงเมื่อ Spawn เสร็จ
	int32 CaptureStage = 0;     // 0 อุ่นเครื่อง 1 บันทึก 2 จบ
};
