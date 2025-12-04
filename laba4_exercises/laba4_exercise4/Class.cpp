#include "Class.h"
#include <iostream>

DefaultTarif::DefaultTarif(std::string r, int a, float p) {
	callnumber = a;
	name = r;
	price = p;
}

float DefaultTarif::get_cost_of_calls() {
	return price * callnumber;
}

std::string DefaultTarif::get_name() {
	return name;
}

int DefaultTarif::get_number_of_calls() {
	return callnumber;
}

float DefaultTarif::get_price() {
	return price;
}

BenefitTarif::BenefitTarif(std::string r, int a, float p) {
	callnumber = a;
	name = r;
	price = p;
}

float BenefitTarif::get_cost_of_calls() {
	return price * callnumber * 0.8;
}

std::string BenefitTarif::get_name() {
	return name;
}

int BenefitTarif::get_number_of_calls() {
	return callnumber;
}

float BenefitTarif::get_price() {
	return price * 0.8;
}

ATC* ATC::instance = nullptr;

ATC::ATC() {
	std::cout << "АТС работает!\n";
}

ATC* ATC::getInstance() {
	if (instance == nullptr) {
		instance = new ATC();
	}

	return instance;
}

void ATC::register_client(std::string r, int a, int tarif_type, float p) {
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

void ATC::print_clients() {
	for (int i = 0; i < tarifs.size(); i++) {
		std::cout << "\nКлиент " << '"' << tarifs[i]->get_name() << '"' << std::endl;
		std::cout << "Количество звонков клиента: " << tarifs[i]->get_number_of_calls() << std::endl;
		std::cout << "Сколько заплатил клиент: " << tarifs[i]->get_cost_of_calls() << std::endl;
	}
}

void ATC::average_tarif_price() {
	float value = 0;
	for (int i = 0; i < tarifs.size(); i++) {
		value += tarifs[i]->get_price();
	}
	value /= tarifs.size();
	std::cout << "\nСредняя стоимость тарифов с учетом скидки: " << value << "\n";
}

ATC::~ATC() {
	std::cout << "\n    Деструктор InternetOperator    " << std::endl;
	for (auto client : tarifs) {
		delete client;
	}
	std::cout << "Удалено клиентов: " << tarifs.size() << std::endl;
}