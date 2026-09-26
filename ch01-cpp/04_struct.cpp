#include <string>
#include <iostream>

// นิยามโครงสร้างข้อมูลสำหรับเก็บข้อมูลบุคคล
struct Person {
    std::string Name;
    int Age;
};

int main()
{
    // การสร้างอินสแตนซ์ (Instance) และกำหนดค่า
    Person p1;
    p1.Name = "Alice";
    p1.Age = 25;

    std::cout << "Name: " << p1.Name << ", Age: " << p1.Age << std::endl;
    return 0;
}
