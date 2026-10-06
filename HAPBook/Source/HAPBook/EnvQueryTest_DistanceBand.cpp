// บทที่ 12 — การนิยาม UEnvQueryTest_DistanceBand

#include "EnvQueryTest_DistanceBand.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"

UEnvQueryTest_DistanceBand::UEnvQueryTest_DistanceBand(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// อ่านข้อมูลและคำนวณเท่านั้น (ตัวแก้ไขใช้จัดลำดับ)
	Cost = EEnvTestCost::Low;
	// รองรับทุกรายการที่มีตำแหน่ง (จุดและ Actor)
	ValidItemType = UEnvQueryItemType_VectorBase::StaticClass();
	// ค่าที่วัดได้เป็น float: ตัวแก้ไขแสดงตัวกรองและสมการให้คะแนน
	SetWorkOnFloatValues(true);
	// ค่าเริ่มต้นเท่านั้น: Query ของบทนี้ใช้ Context ของเป้าหมาย
	DistanceTo = UEnvQueryContext_Querier::StaticClass();
	PreferredDistance.DefaultValue = 600.0f;
	Tolerance.DefaultValue = 300.0f;
}

void UEnvQueryTest_DistanceBand::RunTest(
	FEnvQueryInstance& QueryInstance) const
{
	UObject* QueryOwner = QueryInstance.Owner.Get();
	if (QueryOwner == nullptr)
	{
		return;
	}

	// ผูกพารามิเตอร์กับ Query ที่รัน (ค่าอาจมาจาก Query Config)
	FloatValueMin.BindData(QueryOwner, QueryInstance.QueryID);
	FloatValueMax.BindData(QueryOwner, QueryInstance.QueryID);
	PreferredDistance.BindData(QueryOwner, QueryInstance.QueryID);
	Tolerance.BindData(QueryOwner, QueryInstance.QueryID);

	const float MinThreshold = FloatValueMin.GetValue();
	const float MaxThreshold = FloatValueMax.GetValue();
	const float Preferred = PreferredDistance.GetValue();
	// กันหารด้วยศูนย์
	const float Band = FMath::Max(Tolerance.GetValue(), 1.0f);

	// ตำแหน่งของ Context; คืนเท็จเมื่อ Context ไม่ให้ข้อมูล
	TArray<FVector> Contexts;
	if (!QueryInstance.PrepareContext(DistanceTo, Contexts))
	{
		return;
	}

	// ItemIterator ข้ามรายการที่ถูกกรองแล้ว และแบ่งงานข้ามเฟรมให้
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance);
		It; ++It)
	{
		const FVector ItemLocation =
			GetItemLocation(QueryInstance, It.GetIndex());
		for (const FVector& ContextLocation : Contexts)
		{
			const float Distance = static_cast<float>(
				FVector::Dist(ItemLocation, ContextLocation));
			// ค่าที่วัดได้: 1 ที่ระยะพอดี
			// ลดเชิงเส้นเป็น 0 ที่ขอบแถบ
			const float Raw = FMath::Max(0.0f,
				1.0f - FMath::Abs(Distance - Preferred) / Band);
			It.SetScore(TestPurpose, FilterType, Raw,
				MinThreshold, MaxThreshold);
		}
	}
}

FText UEnvQueryTest_DistanceBand::GetDescriptionTitle() const
{
	return FText::FromString(FString::Printf(TEXT("%s: to %s"),
		*Super::GetDescriptionTitle().ToString(),
		*UEnvQueryTypes::DescribeContext(DistanceTo).ToString()));
}

FText UEnvQueryTest_DistanceBand::GetDescriptionDetails() const
{
	return DescribeFloatTestParams();
}
