#include <iostream>

int main()
{
    int value = 42;          // 1. ตัวแปรต้นฉบับ เก็บค่า 42

    int* ptr = &value;       // 2. Pointer: เก็บ "ที่อยู่" ของ value (ใช้ &)
    int& ref = value;        // 3. Reference: นามแฝงของ value

    *ptr = 50;               // 4. เข้าถึงค่าผ่านพอยน์เตอร์ (ใช้ *) แล้วแก้เป็น 50
    ref = 60;                // 5. แก้ผ่านนามแฝง ซึ่งคือการแก้ value โดยตรง

    std::cout << "value = " << value << std::endl;
    std::cout << "*ptr  = " << *ptr << std::endl;

    return 0;
}
