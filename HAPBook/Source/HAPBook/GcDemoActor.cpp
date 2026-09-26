// บทที่ 4 — การนิยาม AGcDemoActor

#include "GcDemoActor.h"
#include "GcDemoData.h"
#include "Engine/Engine.h"  // GEngine
#include "Engine/World.h"   // GetWorld()
#include "TimerManager.h"   // FTimerManager

AGcDemoActor::AGcDemoActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AGcDemoActor::BeginPlay()
{
	Super::BeginPlay();

	// วัตถุที่ 1: ถือด้วย UPROPERTY TObjectPtr -> GC ติดตามและรักษาไว้
	TrackedData = NewObject<UGcDemoData>(this);
	TrackedData->Tag = 1;

	// วัตถุที่ 2: สร้างแล้วเก็บไว้เฉพาะใน TWeakObjectPtr -> ไม่มีการอ้างอิงอย่างเข้มที่ใดเลย
	// Untracked เป็นตัวแปรภายในฟังก์ชัน (Raw Pointer ตามกฎของบทที่ 2) และสิ้นอายุเมื่อออกจาก BeginPlay
	UGcDemoData* Untracked = NewObject<UGcDemoData>(this);
	Untracked->Tag = 2;
	UntrackedWatcher = Untracked;

	// ก่อนการเก็บกวาด: ทั้งสองวัตถุยังอยู่
	UE_LOG(LogTemp, Log, TEXT("[GC] Before collection: Tracked=%s, Untracked=%s"),
		IsValid(TrackedData) ? TEXT("valid") : TEXT("INVALID"),
		UntrackedWatcher.IsValid() ? TEXT("valid") : TEXT("INVALID"));

	// ขอให้เอนจินเก็บกวาดแบบเต็ม (Full Purge) ในโอกาสถัดไป คือรอบ Tick ของเฟรมถัดไป
	// จึงรายงานผลผ่านตัวจับเวลา ไม่รายงานทันทีในเฟรมนี้
	GEngine->ForceGarbageCollection(true);
	GetWorld()->GetTimerManager().SetTimer(
		ReportTimerHandle, this, &AGcDemoActor::ReportAfterCollection, ReportDelaySeconds, false);
}

void AGcDemoActor::ReportAfterCollection()
{
	// ฝั่ง Tracked: IsValid(TObjectPtr ที่มี UPROPERTY) ปลอดภัย เพราะ GC จะตั้งค่าเป็น null ให้หากวัตถุถูกทำลาย
	// ฝั่ง Untracked: ต้องถามผ่าน TWeakObjectPtr::IsValid() เท่านั้น
	// ห้ามเก็บ Raw Pointer ไว้แล้วเรียก IsValid(Raw) เพราะพอยน์เตอร์นั้นชี้ไปยังหน่วยความจำที่คืนแล้ว (Dangling)
	UE_LOG(LogTemp, Log, TEXT("[GC] After collection: Tracked=%s, Untracked=%s"),
		IsValid(TrackedData) ? TEXT("valid") : TEXT("INVALID"),
		UntrackedWatcher.IsValid() ? TEXT("valid") : TEXT("INVALID"));
}
