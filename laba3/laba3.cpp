#include <iostream>
#include <windows.h>
#include <clocale>
#include <string>
#include <vector>
#include <limits>
#undef max

class TarifStrategy {
public: 
	virtual float get_cost_of_calls() = 0;
	virtual std::string get_name() = 0;
	virtual int get_number_of_calls() = 0;
	virtual float get_price() = 0;
};

class DefaultTarif : public TarifStrategy {
private: 
	int callnumber;
	std::string name;
	float price;
public:
	DefaultTarif(std::string r, int a, float p) {
		callnumber = a;
		name = r;
		price = p;
	}

	float get_cost_of_calls() override {
		return price * callnumber;
	}

	std::string get_name() override {
		return name;
	}

	int get_number_of_calls() override {
		return callnumber;
	}

	float get_price() override {
		return price;
	}
};

class BenefitTarif : public TarifStrategy {
private:
	int callnumber;
	std::string name;
	float price;
public:
	BenefitTarif(std::string r, int a, float p) {
		callnumber = a;
		name = r;
		price = p;
	}

	float get_cost_of_calls() override {
		return price * callnumber * 0.8;
	}

	std::string get_name() override {
		return name;
	}

	int get_number_of_calls() override {
		return callnumber;
	}

	float get_price() override {
		return price * 0.8;
	}
};

class ATC {
private:
	static ATC* instance;

	ATC() {
		std::cout << "АТС работает!\n";
	}

	std::vector<TarifStrategy*> tarifs;
	std::string name;
	int callnumber = 0;

public:
	static ATC* getInstance() {
		if (instance == nullptr) {
			instance = new ATC();
		}

		return instance;
	}

	void register_client(std::string r, int a, int tarif_type, float p) {
		if (r == "" or a < 0 or p <= 0) {
			std::cout << "Ошибка! Введены неправильные значения!" << std::endl;
			return;
		}

		switch (tarif_type) {
		case 0:
			tarifs.push_back(new DefaultTarif(r, a, p));
			break;
		case 1:
			tarifs.push_back(new BenefitTarif(r, a, p));
			break;
		default:
			std::cout << "Ошибка! Введены неправильные значения!\n";
			break;
		}
	}

	void print_clients() {
		for (int i = 0; i < tarifs.size(); i++) {
			std::cout << "\nКлиент " << '"' << tarifs[i]->get_name() << '"' << std::endl;
			std::cout << "Количество звонков клиента: " << tarifs[i]->get_number_of_calls() << std::endl;
			std::cout << "Сколько заплатил клиент: "<< tarifs[i]->get_cost_of_calls() << std::endl;
		}
	}

	void average_tarif_price() {
		float value = 0;
		for (int i = 0; i < tarifs.size(); i++) {
			value += tarifs[i]->get_price();
		}
		value /= tarifs.size();
		std::cout << "\nСредняя стоимость тарифов с учетом скидки: " << value << "\n";
	}

	~ATC() {
		std::cout << "\n    Деструктор InternetOperator    " << std::endl;
		for (auto client : tarifs) {
			delete client;
		}
		std::cout << "Удалено клиентов: " << tarifs.size() << std::endl;
	}

};

ATC* ATC::instance = nullptr;

int main() {

	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	std::locale::global(std::locale("Russian_Russia.1251"));

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

	std::cout << "\nВы записали следующих клиентов:" << std::endl;
	station_1->print_clients();

	station_1->average_tarif_price();

	delete station_1;

	return 0;
}
