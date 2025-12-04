#include <iostream>
#include <windows.h>
#include <clocale>

class Engine {
public:
	void action() {
		std::cout << "Двигатель запущен" << std::endl;
	}
};

class GearBox {
public:
	void action() {
		std::cout << "Передача переключена" << std::endl;
	}
};

class Car : public Engine, public GearBox {};

int main() {
	setlocale(LC_ALL, "rus");
	Car porshe;
	porshe.Engine::action();
	porshe.GearBox::action();
	return 0;
}