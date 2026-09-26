#include <iostream>

int main()
{
    // 1. การประกาศตัวแปร (ต้องระบุชนิดข้อมูล)
    int score = 75;

    // 2. เงื่อนไข if/else และตัวดำเนินการตรรกะ (&&, ||, !)
    if (score >= 50 && score <= 100) {
        std::cout << "Pass" << std::endl;
    }
    else {
        std::cout << "Fail" << std::endl;
    }

    // 3. การวนซ้ำด้วย for loop
    for (int i = 0; i < 3; i++) {
        std::cout << "Count: " << i << std::endl;
    }

    return 0;
}
