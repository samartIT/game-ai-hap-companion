#include <iostream>
#include "GameCharacter.h"   // นำเข้าการประกาศจากไฟล์ Header

// ไฟล์นี้ไม่ได้อยู่ในหนังสือ: เพิ่มขึ้นเพื่อให้ตัวอย่างการแยกไฟล์ทำงานได้จริง
int main()
{
    GameCharacter hero;
    hero.SetHealth(100);
    std::cout << "Hero Health: " << hero.GetHealth() << std::endl;
    return 0;
}
