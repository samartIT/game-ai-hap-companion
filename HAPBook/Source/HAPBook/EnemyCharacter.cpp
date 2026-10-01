// บทที่ 7 — การนิยาม AEnemyCharacter

#include "EnemyCharacter.h"
#include "EnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// สมอง (Mind) ของร่างนี้คือ AEnemyAIController: เอนจินจะสร้างและสั่ง Possess ให้โดยอัตโนมัติ
	AIControllerClass = AEnemyAIController::StaticClass();

	// ให้ AI Controller เข้าควบคุมทั้งกรณีวางไว้ในฉากล่วงหน้าและกรณี Spawn ระหว่างเล่น
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// ประตูของการเดินตามเส้นทางแบบความเร่ง: FNavMovementProperties::bUseAccelerationForPaths (หมวด Nav Movement, ค่าเริ่มต้นของเอนจิ้น = false)
	// เมื่อเป็นจริง PathFollowingComponent เรียก RequestPathMove -> AddMovementInput -> Acceleration ไม่เป็นศูนย์ -> ABP_Unarmed เล่นท่าเดิน
	// เมื่อเป็นเท็จ จะเรียก RequestDirectMove ซึ่งตั้ง RequestedVelocity โดยตรงและไม่แตะ Acceleration (ร่างไถลในท่ายืน)
	// อย่าสับสนกับ bRequestedMoveUseAcceleration (หมวด Character Movement (General Settings) > Advanced, ป้ายชื่อเดียวกัน):
	// ค่านั้นเพียงกำหนดการไต่ความเร็วในโหมด RequestDirectMove และไม่ส่งผลต่อ GetCurrentAcceleration()
	GetCharacterMovement()->GetNavMovementProperties()->bUseAccelerationForPaths = true;

	// หันตามทิศการเคลื่อนที่อย่างนุ่มนวล เช่นเดียวกับ AHAPBookCharacter (ACharacter ตั้ง bUseControllerRotationYaw = true โดยปริยาย)
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
}
