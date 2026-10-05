// บทที่ 12 — Test ที่เขียนเอง: ให้คะแนนตามความใกล้เคียงกับระยะที่ต้องการ
// Test ของ EQS สร้างได้เฉพาะใน C++ (Generator และ Context สร้างใน
// Blueprint ได้) จึงเป็นงานของชั้นรากฐานตาม HAP
// แบบแผนตาม UEnvQueryTest_Distance ของ AIModule: Cost, ValidItemType,
// SetWorkOnFloatValues, พารามิเตอร์ FAIDataProvider*, ItemIterator

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "DataProviders/AIDataProvider.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvironmentQuery/EnvQueryTest.h"
// ต้องเป็น #include บรรทัดสุดท้ายเสมอ
#include "EnvQueryTest_DistanceBand.generated.h"

UCLASS()
class HAPBOOK_API UEnvQueryTest_DistanceBand
	: public UEnvQueryTest
{
	GENERATED_BODY()

public:
	UEnvQueryTest_DistanceBand(
		const FObjectInitializer& ObjectInitializer);

	// Context ที่วัดระยะถึง (ใน EQS_TacticalPoint คือเป้าหมาย)
	UPROPERTY(EditDefaultsOnly, Category = "DistanceBand")
	TSubclassOf<UEnvQueryContext> DistanceTo;

	// ระยะที่ต้องการ (เซนติเมตร) ค่าเริ่มต้น 600
	UPROPERTY(EditDefaultsOnly, Category = "DistanceBand")
	FAIDataProviderFloatValue PreferredDistance;

	// ความกว้างของแถบ: ค่าดิบเป็น 0 เมื่อห่างจากระยะที่ต้องการ
	// เท่านี้หรือมากกว่า ค่าเริ่มต้น 300
	UPROPERTY(EditDefaultsOnly, Category = "DistanceBand")
	FAIDataProviderFloatValue Tolerance;

	// ค่าดิบต่อรายการ: r = max(0, 1 - |d - P| / T)
	virtual void RunTest(
		FEnvQueryInstance& QueryInstance) const override;

	// ข้อความที่ Environment Query Editor แสดงบน Node ของ Test
	virtual FText GetDescriptionTitle() const override;
	virtual FText GetDescriptionDetails() const override;
};
