#include "GameCharacter.h" // อ้างอิงกลับไปยังไฟล์ Header

// การนิยามฟังก์ชัน (Definition)
// ใช้ตัวดำเนินการ :: เพื่อระบุว่าฟังก์ชันนี้เป็นของคลาส GameCharacter
void GameCharacter::SetHealth(int NewHealth) {
    if (NewHealth >= 0) {
        Health = NewHealth;
    }
}

int GameCharacter::GetHealth() {
    return Health;
}
