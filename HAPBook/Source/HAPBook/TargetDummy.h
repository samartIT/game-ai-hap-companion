// บทที่ 9 — หุ่นเป้าหมายที่มีชื่อ: Static Mesh ทรงกระบอก + ป้าย Text Render + DummyName ที่ใช้ในทุกบรรทัดของผลลัพธ์
// ไม่มีการชน (ไม่เปลี่ยน NavMesh และไม่กระทบเส้นทางลาดตระเวนของบทที่ 7–8)
// บทที่ 11 เพิ่ม: UAIPerceptionStimuliSourceComponent — Actor ที่ไม่ใช่ Pawn ต้องลงทะเบียนเป็นแหล่งสิ่งเร้าของ Sight เอง
// (Pawn ทุกตัวถูกลงทะเบียนอัตโนมัติ: UAISense_Sight::bAutoRegisterAllPawnsAsSources = true) หุ่นไม่มีฝ่าย จึงถูกเห็นเฉพาะเมื่อ Detect Neutrals เป็นจริง

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetDummy.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

class UStaticMeshComponent;
class UTextRenderComponent;
class UAIPerceptionStimuliSourceComponent;

UCLASS(Blueprintable)
class HAPBOOK_API ATargetDummy : public AActor
{
	GENERATED_BODY()

public:
	ATargetDummy();

	// ชื่อที่ปรากฏในป้ายและใน Output Log (ตั้งที่อินสแตนซ์: Dummy_01, Dummy_02, ...)
	// EditAnywhere = แก้ได้ในแผง Details; BlueprintReadOnly = มี Node Get ในกราฟ Blueprint (ไม่มีแล้ว Print String ของขั้นที่ 1 อ่านไม่ได้)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dummy")
	FName DummyName = TEXT("Dummy");

	// ป้ายตามชื่อ: ทำงานทั้งในตัวแก้ไข (เมื่อแก้ DummyName) และตอนโหลดแผนที่
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// บทที่ 11: ลงทะเบียนกับ AIPerceptionSystem เมื่อ World มีตัวตนแล้ว (RegisterForSense ต้องการ World จึงเรียกใน Constructor ไม่ได้)
	virtual void BeginPlay() override;

private:
	// บทที่ 11: แหล่งสิ่งเร้าสำหรับ Sight ไม่มีผลในแผนที่ที่ไม่มี Listener ลงทะเบียนอยู่ (L_Ch09–L_Ch10: Perception Enabled เป็นเท็จ Controller ถอน Listener ตอน Possess)
	UPROPERTY(VisibleAnywhere, Category = "Dummy")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSource;

	UPROPERTY(VisibleAnywhere, Category = "Dummy")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Dummy")
	TObjectPtr<UTextRenderComponent> Label;
};
