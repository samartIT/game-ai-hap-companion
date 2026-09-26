// บทที่ 4 — UObject ธรรมดาสำหรับสาธิต Garbage Collection

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GcDemoData.generated.h" // ต้องเป็น #include บรรทัดสุดท้ายเสมอ

UCLASS()
class HAPBOOK_API UGcDemoData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "GC")
	int32 Tag = 0;
};
