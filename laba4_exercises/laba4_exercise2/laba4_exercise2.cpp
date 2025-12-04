#include <iostream>
#include <string>

class Car
{
public:
    Car(std::string n)
    {
        name = n;
    }
    void print()
    {
        std::cout << "Car name: " << name << std::endl;
    }
    Car operator + (const Car& car) const
    {
        return Car{ name + car.name };
    }
private:
    std::string name;
};

int main()
{
    Car c1{ "Porshe" };
    Car c2{ " 911 GT2" };
    Car c3 = c1 + c2;
    c3.print();  
}