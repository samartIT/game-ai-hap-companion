#include <iostream>

// Base Class
class Shape {
public:
    // ต้องระบุ 'virtual' เพื่อเปิดใช้งานภาวะพหุสัณฐาน
    virtual void Draw() {
        std::cout << "Drawing a generic shape." << std::endl;
    }

    // คลาสฐานที่มี Virtual Function ต้องมี Virtual Destructor เสมอ
    virtual ~Shape() = default;
};

// Derived Class 1
class Circle : public Shape {
public:
    void Draw() override {
        std::cout << "Drawing a Circle." << std::endl;
    }
};

// Derived Class 2
class Square : public Shape {
public:
    void Draw() override {
        std::cout << "Drawing a Square." << std::endl;
    }
};

int main()
{
    // Pointer ของคลาสฐานชี้ไปยังวัตถุของคลาสลูก
    Shape* s1 = new Circle();
    Shape* s2 = new Square();

    // ระบบเรียกฟังก์ชัน Draw() ของคลาสที่แท้จริง (Circle/Square)
    s1->Draw();
    s2->Draw();

    // คืนหน่วยความจำที่จองด้วย new ทุกครั้ง
    delete s1;
    delete s2;
    return 0;
}
