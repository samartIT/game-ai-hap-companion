#include <iostream>

// Base Class
class Animal {
public:
    void Eat() {
        std::cout << "Animal is eating." << std::endl;
    }
};

// Derived Class (สืบทอดจาก Animal)
class Dog : public Animal {
public:
    void Bark() {
        std::cout << "Dog is barking." << std::endl;
    }
};

int main()
{
    Dog myDog;
    myDog.Eat();  // เรียกใช้ฟังก์ชันจาก Base Class
    myDog.Bark(); // เรียกใช้ฟังก์ชันของตัวเอง
    return 0;
}
