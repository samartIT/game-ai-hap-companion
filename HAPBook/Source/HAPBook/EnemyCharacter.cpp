// บทที่ 7 — การนิยาม AEnemyCharacter (บทที่ 10 เพิ่มส่วนที่กำกับว่า "บทที่ 10")

#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "HealthComponent.h"
#include "BrainComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// สมอง (Mind) ของร่างนี้คือ AEnemyAIController: เอนจิ้นจะสร้างและสั่ง Possess ให้โดยอัตโนมัติ
	// บทที่ 11: Class Defaults ของ BP_Enemy เขียนทับค่านี้เป็น BP_EnemyAIController (Blueprint ลูกที่ถือ Component AIPerception และ Sense)
	AIControllerClass = AEnemyAIController::StaticClass();

	// ให้ AI Controller เข้าควบคุมทั้งกรณีวางไว้ในฉากล่วงหน้าและกรณี Spawn ระหว่างเล่น
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// สวิตช์เปิดการเดินตามเส้นทางแบบความเร่ง: FNavMovementProperties::bUseAccelerationForPaths (หมวด Nav Movement, ค่าเริ่มต้นของเอนจิ้น = false)
	// เมื่อเป็นจริง PathFollowingComponent เรียก RequestPathMove -> AddMovementInput -> Acceleration ไม่เป็นศูนย์ -> ABP_Unarmed เล่นท่าเดิน
	// เมื่อเป็นเท็จ จะเรียก RequestDirectMove ซึ่งตั้ง RequestedVelocity โดยตรงและไม่แตะ Acceleration (ร่างไถลในท่ายืน)
	// อย่าสับสนกับ bRequestedMoveUseAcceleration (หมวด Character Movement (General Settings) > Advanced, ป้ายชื่อเดียวกัน):
	// ค่านั้นเพียงกำหนดการไต่ความเร็วในโหมด RequestDirectMove และไม่ส่งผลต่อ GetCurrentAcceleration()
	GetCharacterMovement()->GetNavMovementProperties()->bUseAccelerationForPaths = true;

	// หันตามทิศการเคลื่อนที่อย่างนุ่มนวล เช่นเดียวกับ AHAPBookCharacter (ACharacter ตั้ง bUseControllerRotationYaw = true โดยปริยาย)
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// บทที่ 10: ร่างกายที่บาดเจ็บได้ — Component ถูกสร้างเป็น Default Subobject ของทุกอินสแตนซ์ (รวม BP_Enemy ทุกตัวในแผนที่บทที่ 7–9)
	// ค่า MaxHealth เริ่มต้น 100 มาจาก Component เอง BP_Enemy ปรับได้ใน Class Defaults
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// บทที่ 10: ฝั่ง C++ ของสะพานเชื่อม — Delegate เดียวกับที่ Blueprint ผูกใน Event Graph ผูกจาก C++ ด้วย AddDynamic
	if (HealthComponent != nullptr)
	{
		HealthComponent->OnDeath.AddDynamic(this, &AEnemyCharacter::HandleDeath);
	}
}

void AEnemyCharacter::HandleDeath()
{
	// 1. หยุดตรรกะ AI: BrainComponent ของ Controller คือ UBehaviorTreeComponent ที่ RunBehaviorTree สร้างไว้
	//    StopLogic -> StopTree(Safe): Move To ที่ค้างอยู่จบด้วยผล Aborted ซึ่ง OnMoveCompleted ของบทที่ 7 ละเว้น (ไม่มีบรรทัด [AI] เพิ่ม)
	if (const AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if (UBrainComponent* Brain = AIController->GetBrainComponent())
		{
			Brain->StopLogic(TEXT("Dead"));
		}
	}

	// 2. ปิดการชนทั้ง Actor (แคปซูลไม่ทับซ้อนกับเขตอันตรายอีก และผู้เล่นเดินทะลุร่างได้) และหยุดการเคลื่อนที่ (MovementMode = None: ไม่มีแรงโน้มถ่วง ร่างไม่ร่วง)
	SetActorEnableCollision(false);
	GetCharacterMovement()->DisableMovement();

	// 3. ไม่รับความเสียหายอีก: APawn::ShouldTakeDamage ตรวจ CanBeDamaged() ก่อนเรียก TakeDamage ของคลาสฐาน
	SetCanBeDamaged(false);

	// 4. ทำลาย Actor หลัง 3 วินาที (ตัวจับเวลาของเอนจิ้น ไม่ใช่ Tick) ร่างคงอยู่ให้เห็นผลของการตายก่อนหายไป
	SetLifeSpan(CorpseLifeSpan);
}
