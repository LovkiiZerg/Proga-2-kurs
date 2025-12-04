#include <iostream>
#include <string>

class Car {
public:
    std::string name;
    int mileage;

    Car(std::string n, int a) : name(n), mileage(a) {}

    void display() {
        std::cout << "Car name: " << name << ", mileage: " << mileage << " km" << std::endl;
    }
};

void change_mileage(Car& s) {
    s.display();
    s.mileage = 128730;
}

int main() {
    Car car1("Porshe", 11500);
    change_mileage(car1);
    car1.display();
    return 0;
}