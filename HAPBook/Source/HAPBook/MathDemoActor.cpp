// บทที่ 6 — การนิยาม AMathDemoActor

#include "MathDemoActor.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h" // FindLookAtRotation

AMathDemoActor::AMathDemoActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
}

void AMathDemoActor::BeginPlay()
{
	Super::BeginPlay();

	// ระบบพิกัดของ Unreal: มือซ้าย, X ไปหน้า, Y ไปขวา, Z ขึ้น, หน่วยเซนติเมตร
	const FVector MyLocation = GetActorLocation();      // (0, 0, 100) เมื่อวางตามที่กำหนด
	const FVector Forward = FVector::ForwardVector;      // (1, 0, 0)

	// 1. ทิศทางไปยังเป้าหมาย ฉายลงระนาบ XY แล้วทำให้เป็นเวกเตอร์หนึ่งหน่วย (สำหรับการทดสอบการหันหน้า)
	FVector ToTargetXY = TargetPoint - MyLocation;
	ToTargetXY.Z = 0.0;
	const FVector Dir = ToTargetXY.GetSafeNormal();
	UE_LOG(LogTemp, Log, TEXT("[Math] Direction (XY, normalized): (%.2f, %.2f, %.2f)"), Dir.X, Dir.Y, Dir.Z);

	// 2. Dot Product -> ขนาดของมุมระหว่างทิศหน้ากับทิศเป้าหมาย
	const double Dot = FVector::DotProduct(Forward, Dir);
	const double AngleDeg = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(Dot, -1.0, 1.0)));
	UE_LOG(LogTemp, Log, TEXT("[Math] Dot=%.2f -> Angle=%.2f deg"), Dot, AngleDeg);

	// 3. Cross Product -> ด้าน: Forward x Right = Up ดังนั้น Z > 0 หมายถึงเป้าหมายอยู่ทางขวา
	const double CrossZ = FVector::CrossProduct(Forward, Dir).Z;
	UE_LOG(LogTemp, Log, TEXT("[Math] CrossZ=%.2f -> %s"), CrossZ, CrossZ > 0.0 ? TEXT("RIGHT") : TEXT("LEFT"));

	// 4. ระยะทางสามมิติ (ไม่ฉายลงระนาบ)
	const double Distance3D = FVector::Dist(MyLocation, TargetPoint);
	UE_LOG(LogTemp, Log, TEXT("[Math] Distance3D=%.2f"), Distance3D);

	// 5. มุม Yaw ที่ต้องหันเพื่อมองเป้าหมาย (องศา, + = ขวา ตามนิยามของ FRotator)
	const FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(MyLocation, TargetPoint);
	UE_LOG(LogTemp, Log, TEXT("[Math] LookAtYaw=%.2f"), LookAt.Yaw);

	// 6. Line Trace ดิ่งลงด้านล่างบนช่อง Visibility โดยละเว้นตัวเอง
	// ผลที่คาด: LogTemp: [Math] TraceDown: Hit=true Distance=100.50
	// (ชนผิวการชนของพื้นที่ Z = -0.5 ไม่ใช่ผิวของโมเดลที่ Z = 0 ค่าที่เอนจินวัดได้คือค่าที่เชื่อถือ)
	const FVector TraceEnd = MyLocation + FVector::DownVector * TraceLength;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(MathDemoTrace), false, this);
	FHitResult Hit;
	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, MyLocation, TraceEnd, ECC_Visibility, Params);
	UE_LOG(LogTemp, Log, TEXT("[Math] TraceDown: Hit=%s Distance=%.2f"),
		bHit ? TEXT("true") : TEXT("false"), bHit ? static_cast<double>(Hit.Distance) : 0.0);
}
