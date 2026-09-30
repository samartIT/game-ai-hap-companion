// บทที่ 7 — การนิยาม AEnemyCharacter

#include "EnemyCharacter.h"
#include "EnemyAIController.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// สมอง (Mind) ของร่างนี้คือ AEnemyAIController: เอนจินจะสร้างและสั่ง Possess ให้โดยอัตโนมัติ
	AIControllerClass = AEnemyAIController::StaticClass();

	// ให้ AI Controller เข้าควบคุมทั้งกรณีวางไว้ในฉากล่วงหน้าและกรณี Spawn ระหว่างเล่น
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}
