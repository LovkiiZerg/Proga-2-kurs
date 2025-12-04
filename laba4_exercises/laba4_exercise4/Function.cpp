#include "Function.h"
#include "Class.h"
#include <iostream>
#include <windows.h>
#include <clocale>
#include <limits>
#undef max

void ConsoleSettings() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	std::locale::global(std::locale("Russian_Russia.1251"));
}

void AddCustomers() {
	ATC* station_1 = ATC::getInstance();

	std::cout << "\nДобавьте клиентов" << std::endl;
	std::string cancel = "";

	while (cancel != "0") {
		std::string name;
		int t;
		float p;
		int ct;
		try {
			std::cout << "\nИмя клиента: ";
			std::cin >> name;
			std::cout << "Количество звонков клиента: ";
			std::cin >> t;
		}
		catch (...) {
			std::cout << "Что-то не так, попробуйте заново";
			continue;
		}

		if (std::cin.fail()) {
			std::cout << "Что-то не так, попробуйте заново" << std::endl;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			continue;
		}

		try {
			std::cout << "Стоимость тарифа: ";
			std::cin >> p;
		}
		catch (...) {
			std::cout << "Что-то не так, попробуйте заново";
			continue;
		}

		if (std::cin.fail()) {
			std::cout << "Что-то не так, попробуйте заново" << std::endl;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			continue;
		}

		try {
			std::cout << "Введите тип тарифа (Обычный - 0, Льготный - 1): ";
			std::cin >> ct;
		}
		catch (...) {
			std::cout << "Что-то не так, попробуйте заново";
			continue;
		}

		if (std::cin.fail()) {
			std::cout << "Что-то не так, попробуйте заново" << std::endl;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			continue;
		}

		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		station_1->register_client(name, t, ct, p);
		std::cout << "Это был последний клиент? (да - 0, нет - любой другой символ): ";
		std::cin >> cancel;
	}
}

void displayResults() {
	ATC* station_1 = ATC::getInstance();

	std::cout << "\nВы записали следующих клиентов:" << std::endl;
	station_1->print_clients();

	station_1->average_tarif_price();

	delete station_1;
}