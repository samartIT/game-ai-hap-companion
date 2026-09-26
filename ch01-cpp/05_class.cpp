#include <iostream>

class GameCharacter {
private:
    int Health;   // ข้อมูลภายใน (ถูกซ่อนไว้)

public:
    // พฤติกรรมภายนอก (เปิดเผยให้ใช้งาน)
    void SetHealth(int NewHealth) {
        if (NewHealth >= 0) {   // ตรวจสอบความถูกต้องก่อนกำหนดค่า
            Health = NewHealth;
        }
    }

    int GetHealth() {
        return Health;
    }
};

int main()
{
    GameCharacter hero;
    hero.SetHealth(100);
    // hero.Health = 50; // ผิดพลาด: เข้าถึง private member จากภายนอกไม่ได้
    std::cout << "Hero Health: " << hero.GetHealth() << std::endl;
    return 0;
}
