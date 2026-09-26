// บทที่ 2 — Actor สาธิตข้อกำหนดของ Unreal C++ ในที่เดียว
// คำนำหน้า A: คลาสที่สืบทอดจาก AActor (วางในฉากได้)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ConventionsDemoActor.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

class UConventionsData;

UCLASS()
class HAPBOOK_API AConventionsDemoActor : public AActor
{
	GENERATED_BODY()

public:
	AConventionsDemoActor();

	// int32 แทน int (Platform Agnostic)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conventions")
	int32 Health = 100;

	// FString แทน std::string; ข้อความคงที่ต้องครอบด้วย TEXT()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conventions")
	FString DisplayName = TEXT("Sentinel-01");

	// TArray แทน std::vector; ทำงานร่วมกับ UPROPERTY และระบบ Reflection ได้
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conventions")
	TArray<int32> Scores;

	// สมาชิกที่อ้างอิง UObject ประกาศเป็น TObjectPtr<T> ภายใต้ UPROPERTY
	// (ไม่ใช่ UConventionsData* แบบดั้งเดิม)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Conventions")
	TObjectPtr<UConventionsData> Data;

	// UFUNCTION(BlueprintCallable): เปิดเผยฟังก์ชันให้ Blueprint เรียกใช้ได้
	UFUNCTION(BlueprintCallable, Category = "Conventions")
	int32 GetScoreSum() const;

protected:
	virtual void BeginPlay() override;
};
