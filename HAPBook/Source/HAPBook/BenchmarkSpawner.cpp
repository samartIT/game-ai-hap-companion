// บทที่ 14 — การนิยาม ABenchmarkSpawner

#include "BenchmarkSpawner.h"
#include "BenchmarkActorBase.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "HAL/PlatformTime.h"
#include "Misc/CommandLine.h"
#include "Misc/DateTime.h"
#include "Misc/Parse.h"
#if WITH_EDITORONLY_DATA
#include "Components/BillboardComponent.h"
#endif

ABenchmarkSpawner::ABenchmarkSpawner()
{
	// Tick ของ Spawner ใช้จับเวลาเท่านั้น และเปิดเมื่อจับเวลาเอง
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

#if WITH_EDITORONLY_DATA
	EditorIcon = CreateDefaultSubobject<UBillboardComponent>(
		TEXT("EditorIcon"));
	if (EditorIcon != nullptr)
	{
		EditorIcon->SetupAttachment(Root);
		EditorIcon->bIsEditorOnly = true;
	}
#endif
}

void ABenchmarkSpawner::BeginPlay()
{
	Super::BeginPlay();
	ReadCommandLine();
	if (ArmClass == nullptr && !ArmArgument.IsEmpty())
	{
		// ค่าที่ได้รับและรูปแบบที่ยอมรับ: แก้ -BenchArm= ได้ทันที
		UE_LOG(LogTemp, Warning,
			TEXT("[Bench] Arm Class \"%s\" not found; use ")
			TEXT("/Game/Blueprints/")
			TEXT("BP_Arm_PureBP.BP_Arm_PureBP_C or ")
			TEXT("/Script/HAPBook.BenchmarkActorCpp"),
			*ArmArgument);
		return;
	}
	if (ArmClass == nullptr)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[Bench] Arm Class not set, nothing spawned"));
		return;
	}

	const int32 Spawned = SpawnGrid();
	// เครื่องมือวัด: ยืนยันเซลล์ของรอบนี้ (สถาปัตยกรรม, N, I)
	UE_LOG(LogTemp, Log,
		TEXT("[Bench] Spawned %d x %s with %d iterations"),
		Spawned, *ArmClass->GetName(), Iterations);

	StartSeconds = FPlatformTime::Seconds();
	SetActorTickEnabled(bAutoCapture);
}

void ABenchmarkSpawner::ReadCommandLine()
{
	const TCHAR* Cmd = FCommandLine::Get();
	if (FParse::Value(Cmd, TEXT("BenchArm="), ArmArgument))
	{
		// เส้นทางของคลาสที่คอมไพล์แล้ว เช่น
		// /Game/Blueprints/BP_Arm_PureBP.BP_Arm_PureBP_C
		ArmClass = LoadClass<ABenchmarkActorBase>(
			nullptr, *ArmArgument);
	}
	FParse::Value(Cmd, TEXT("BenchN="), Count);
	FParse::Value(Cmd, TEXT("BenchI="), Iterations);
	if (FParse::Param(Cmd, TEXT("BenchAutoCapture")))
	{
		bAutoCapture = true;
	}
}

int32 ABenchmarkSpawner::SpawnGrid()
{
	const int32 Columns =
		FMath::CeilToInt(FMath::Sqrt(static_cast<float>(Count)));
	const double Half = 0.5 * (Columns - 1) * GridSpacing;
	const FVector Origin =
		GetActorLocation() - FVector(Half, Half, 0.0);
	int32 Spawned = 0;
	for (int32 Index = 0; Index < Count; ++Index)
	{
		const FVector Cell = Origin + FVector(
			(Index / Columns) * GridSpacing,
			(Index % Columns) * GridSpacing, 0.0);
		const FTransform Where(FRotator::ZeroRotator, Cell);
		// Deferred: เขียน I ก่อน FinishSpawning
		// (ก่อน BeginPlay และก่อน Tick แรกของตัวนั้น)
		ABenchmarkActorBase* Arm = GetWorld()
			->SpawnActorDeferred<ABenchmarkActorBase>(
				ArmClass, Where, this, nullptr,
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (Arm == nullptr)
		{
			continue;
		}
		Arm->Iterations = Iterations;
		Arm->FinishSpawning(Where);
		++Spawned;
	}
	return Spawned;
}

void ABenchmarkSpawner::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	// นาฬิกาจริง ไม่ใช่เวลาของเกม ซึ่งถูกจำกัดที่ 0.4 วินาที
	// ต่อเฟรม (Max Undilated Frame Time) เมื่อเฟรมช้ากว่านั้น
	const double Elapsed =
		FPlatformTime::Seconds() - StartSeconds;
	if (CaptureStage == 0 && Elapsed >= WarmupSeconds)
	{
		// ชื่อไฟล์บอกเซลล์: Bench_<คลาส>_N<N>_I<I>_<เวลา>.csv
		const FString File = FString::Printf(
			TEXT("Bench_%s_N%d_I%d_%s"), *ArmClass->GetName(),
			Count, Iterations,
			*FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S")));
		GEngine->Exec(GetWorld(),
			*(TEXT("csvprofile startfile=") + File));
		GEngine->Exec(GetWorld(), TEXT("csvprofile start"));
		CaptureStage = 1;
	}
	else if (CaptureStage == 1
		&& Elapsed >= WarmupSeconds + CaptureSeconds)
	{
		GEngine->Exec(GetWorld(), TEXT("csvprofile stop"));
		CaptureStage = 2;
		SetActorTickEnabled(false);
	}
}
