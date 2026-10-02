// บทที่ 4 — Actor สาธิตระบบ Reflection: Constructor กับ CDO และการไล่อ่าน UPROPERTY ของตนเอง

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReflectionDemoActor.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

UCLASS()
class HAPBOOK_API AReflectionDemoActor : public AActor
{
	GENERATED_BODY()

public:
	// Constructor ทำงานทั้งตอนสร้าง CDO (IsTemplate=true) และตอนสร้างอินสแตนซ์จริง (IsTemplate=false)
	AReflectionDemoActor();

	// สมาชิกสามตัวต่อไปนี้ถูก UHT ลงทะเบียนในระบบ Reflection
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reflection")
	int32 Health = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reflection")
	FString Codename = TEXT("Warden");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reflection")
	float Speed = 300.f;

	// สมาชิกที่ไม่มี UPROPERTY โดยเจตนา: เป็นตัวแปร C++ ธรรมดา ระบบ Reflection มองไม่เห็น
	int32 HiddenCounter = 0;

protected:
	// ไล่อ่านสมาชิกที่ระบบ Reflection รู้จักของคลาสนี้ (ไม่รวมคลาสแม่) แล้วบันทึกลง Output Log
	virtual void BeginPlay() override;
};
