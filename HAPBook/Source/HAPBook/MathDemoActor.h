// บทที่ 6 — Actor สาธิตคณิตศาสตร์เวกเตอร์ด้วยตัวเลขที่กำหนดตายตัว (ผลลัพธ์ทุกบรรทัดคำนวณล่วงหน้าได้)
// วางไว้ในแผนที่ L_Ch06 ที่ตำแหน่ง (0, 0, 100) เหนือพื้นของแม่แบบ (ผิวการชนของพื้นอยู่ที่ Z = -0.5 cm)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MathDemoActor.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

UCLASS()
class HAPBOOK_API AMathDemoActor : public AActor
{
	GENERATED_BODY()

public:
	AMathDemoActor();

	// จุดเป้าหมายในพิกัดโลก (หน่วยเซนติเมตร) ผู้อ่านเปลี่ยนค่าได้จาก Details เพื่อทดลองทำนายผลใหม่
	UPROPERTY(EditAnywhere, Category = "Math")
	FVector TargetPoint = FVector(700.0, 700.0, 0.0);

	// ความยาวของเส้นตรวจสอบที่ยิงลงด้านล่าง (เซนติเมตร)
	UPROPERTY(EditAnywhere, Category = "Math")
	float TraceLength = 1000.0f;

protected:
	virtual void BeginPlay() override;

private:
	// รากของ Actor: ต้องมีเพื่อให้ Actor มีตำแหน่งในฉากและวางได้
	UPROPERTY(VisibleAnywhere, Category = "Math")
	TObjectPtr<USceneComponent> SceneRoot;
};
