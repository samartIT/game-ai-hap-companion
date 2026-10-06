// บทที่ 9 — การนิยาม AEnemySpawner (รุ่นที่ 2)

#include "EnemySpawner.h"
#include "EnemyCharacter.h"
#include "TargetDummy.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Components/SceneComponent.h"
#include "Engine/TargetPoint.h"
#include "Engine/World.h"
#include "NavigationSystem.h"
#if WITH_EDITORONLY_DATA
#include "Components/BillboardComponent.h"
#endif

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	// Root Component: ไม่มีมันแล้ว Actor ที่วางจะไม่มี Location/Rotation/Scale และสนามจะอยู่ที่จุดกำเนิดของโลก
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

#if WITH_EDITORONLY_DATA
	EditorIcon = CreateDefaultSubobject<UBillboardComponent>(TEXT("EditorIcon"));
	if (EditorIcon != nullptr)
	{
		EditorIcon->SetupAttachment(Root);
		EditorIcon->bIsEditorOnly = true;
	}
#endif

	DummyClass = ATargetDummy::StaticClass();
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	if (SpawnCount <= 0 && DummyCount <= 0)
	{
		return;
	}

	// การวางตัวละคร AI คือคำถามเรื่องการนำทาง: ทุกตำแหน่งที่ใช้ต้องอยู่บน NavMesh
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (NavSys == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Nodes] Spawner: no navigation system in this level, nothing spawned"));
		return;
	}

	// ลำดับสำคัญ: หุ่นก่อนศัตรู เพราะ Service รวบรวมเป้าหมายที่เป็นไปได้ครั้งเดียวเมื่อ Branch ลาดตระเวนเริ่ม (OnBecomeRelevant / Receive Activation AI)
	const int32 Dummies = SpawnDummies(*NavSys);
	int32 Skipped = 0;
	const int32 Enemies = SpawnEnemies(*NavSys, Skipped);

	// เครื่องมือวัดเท่านั้น: ยืนยันว่าแต่ละรอบวัดฉบับที่ตั้งใจ ด้วยจำนวนและชื่อ Behavior Tree
	UE_LOG(LogTemp, Log, TEXT("[Nodes] Spawner: %d enemies (%d off-navmesh skipped), %d dummies, tree = %s"),
		Enemies, Skipped, Dummies, *GetNameSafe(BehaviorTreeOverride));
}

int32 AEnemySpawner::SpawnDummies(UNavigationSystemV1& NavSys)
{
	if (DummyClass == nullptr || DummyCount <= 0)
	{
		return 0;
	}

	int32 Spawned = 0;
	for (int32 Index = 0; Index < DummyCount; ++Index)
	{
		FNavLocation Location;
		if (!NavSys.GetRandomReachablePointInRadius(GetActorLocation(), ArenaRadius, Location))
		{
			continue;
		}
		const FTransform SpawnTransform(FRotator::ZeroRotator, Location.Location);
		// Deferred: ตั้ง DummyName ก่อน FinishSpawning เพื่อให้ OnConstruction เขียนป้ายด้วยชื่อที่ถูกต้องตั้งแต่แรก
		ATargetDummy* Dummy = GetWorld()->SpawnActorDeferred<ATargetDummy>(DummyClass, SpawnTransform, this, nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (Dummy == nullptr)
		{
			continue;
		}
		Dummy->DummyName = FName(*FString::Printf(TEXT("Dummy_%02d"), Index + 1));
		Dummy->FinishSpawning(SpawnTransform);
		++Spawned;
	}
	return Spawned;
}

int32 AEnemySpawner::SpawnEnemies(UNavigationSystemV1& NavSys, int32& OutSkipped)
{
	OutSkipped = 0;
	if (EnemyClass == nullptr || SpawnCount <= 0)
	{
		return 0;
	}

	const int32 Columns = FMath::CeilToInt(FMath::Sqrt(static_cast<float>(SpawnCount)));
	const FVector HalfExtent(0.5f * Columns * GridSpacing, 0.5f * Columns * GridSpacing, 0.0f);
	const FVector ProjectExtent(GridSpacing, GridSpacing, 500.0f);
	int32 Spawned = 0;

	for (int32 Index = 0; Index < SpawnCount; ++Index)
	{
		// ตารางรอบ Spawner แล้วฉายแต่ละช่องลง NavMesh: ช่องที่ฉายไม่ได้ (นอกพื้นสีเขียว) ถูกข้ามและนับไว้
		const FVector Cell = GetActorLocation() - HalfExtent + FVector((Index / Columns) * GridSpacing, (Index % Columns) * GridSpacing, 0.0f);
		FNavLocation Projected;
		if (!NavSys.ProjectPointToNavigation(Cell, Projected, ProjectExtent))
		{
			++OutSkipped;
			continue;
		}

		// เส้นทางลาดตระเวนของตัวนี้เอง: TargetPoint สามจุดที่สุ่มในรัศมี PatrolRadius และเดินถึงได้
		// เลือกสร้าง TargetPoint แทนการเพิ่มสมาชิกใหม่ใน AEnemyCharacter เพราะโค้ดของบทที่ 7–8 (PatrolPoints : TArray<AActor*>)
		// ไม่ต้องเปลี่ยนแม้บรรทัดเดียว: ศัตรูที่ Spawner สร้างเดินด้วยเส้นทางโค้ดเดียวกับศัตรูที่วางด้วยมือ
		TArray<TObjectPtr<AActor>> Route;
		for (int32 Point = 0; Point < 3; ++Point)
		{
			FNavLocation RoutePoint;
			if (NavSys.GetRandomReachablePointInRadius(Projected.Location, PatrolRadius, RoutePoint))
			{
				if (ATargetPoint* Marker = GetWorld()->SpawnActor<ATargetPoint>(ATargetPoint::StaticClass(), RoutePoint.Location, FRotator::ZeroRotator))
				{
					Route.Add(Marker);
				}
			}
		}
		if (Route.IsEmpty())
		{
			++OutSkipped;
			continue;
		}

		const FTransform SpawnTransform(GetActorRotation(), Projected.Location + FVector(0.0f, 0.0f, 90.0f));
		// Deferred: กำหนด Behavior Tree และจุดลาดตระเวนก่อน FinishSpawning ซึ่งเป็นจังหวะที่ AI Controller เข้าควบคุมและอ่านค่าเหล่านี้ (OnPossess ของบทที่ 7)
		AEnemyCharacter* Enemy = GetWorld()->SpawnActorDeferred<AEnemyCharacter>(EnemyClass, SpawnTransform, this, nullptr,
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
		if (Enemy == nullptr)
		{
			++OutSkipped;
			continue;
		}
		if (BehaviorTreeOverride != nullptr)
		{
			Enemy->BehaviorTreeAsset = BehaviorTreeOverride;
		}
		Enemy->PatrolPoints = Route;
		Enemy->FinishSpawning(SpawnTransform);
		++Spawned;
	}
	return Spawned;
}
