// บทที่ 14 — คลาสฐานของชุดทดสอบ: Actor ไร้ Mesh ที่ถือภาระงานร่วมของทั้งสามสถาปัตยกรรม
// RunWork() นิยามครั้งเดียวที่นี่ สถาปัตยกรรมทั้งสามต่างกันเฉพาะเส้นทางที่เรียกภาระงานทุกเฟรม
// Blueprint ล้วน (BP_Arm_PureBP) และแบบผสม (BP_Arm_Hybrid) เป็นคลาสลูกแบบ Blueprint ส่วน C++ ล้วนคือ ABenchmarkActorCpp
// Tick() ของคลาสฐานว่างโดยเจตนา (กล่องคำเตือนของบท) และ Abstract ทำให้เลือกคลาสฐานเป็น Arm Class ไม่ได้

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BenchmarkActorBase.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

class USceneComponent;

UCLASS(Abstract)
class HAPBOOK_API ABenchmarkActorBase : public AActor
{
	GENERATED_BODY()

public:
	ABenchmarkActorBase();

	// I: จำนวนรอบของภาระงานต่อเฟรม (Spawner เขียนค่านี้)
	UPROPERTY(EditAnywhere, BlueprintReadOnly,
		Category = "Benchmark", meta = (ClampMin = "1"))
	int32 Iterations = 1000;

	// ผลของเฟรมล่าสุด: ทุกสถาปัตยกรรมได้ 2 x (I - 1)
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite,
		Category = "Benchmark")
	double Result = 0.0;

	// ภาระงานร่วมของทั้งสามสถาปัตยกรรม
	UFUNCTION(BlueprintCallable, Category = "Benchmark")
	void RunWork();

	virtual void Tick(float DeltaSeconds) override;

private:
	// Root ที่ไม่มี Mesh: ให้ Actor มีตำแหน่งในตาราง
	UPROPERTY(VisibleAnywhere, Category = "Benchmark")
	TObjectPtr<USceneComponent> Root;
};
