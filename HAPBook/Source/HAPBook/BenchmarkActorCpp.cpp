// บทที่ 14 — การนิยาม ABenchmarkActorCpp

#include "BenchmarkActorCpp.h"

void ABenchmarkActorCpp::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// การวนซ้ำทั้งหมดทำงานเป็นรหัสเครื่องที่คอมไพล์แล้ว
	RunWork();
}
