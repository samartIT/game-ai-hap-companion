#pragma once // คำสั่งป้องกันการ include ซ้ำ

// การประกาศ Class (Declaration)
class GameCharacter {
private:
    int Health;
public:
    void SetHealth(int NewHealth); // ประกาศฟังก์ชัน (ยังไม่ระบุการทำงาน)
    int GetHealth();
};
