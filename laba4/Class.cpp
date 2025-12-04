#include "Class.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

ATC* ATC::instance = nullptr;

DefaultTarif::DefaultTarif(const std::string& r, int a, float p)
    : name(r), callnumber(a), baseprice(p) {
}

float DefaultTarif::get_cost_of_calls() const {
    return get_tarif_price() * callnumber;
}

std::string DefaultTarif::get_name() const {
    return name;
}

int DefaultTarif::get_number_of_calls() const {
    return callnumber;
}

float DefaultTarif::get_tarif_price() const {
    return baseprice;
}

float DefaultTarif::get_base_price() const {
    return baseprice;
}

int DefaultTarif::get_tarif_type() const {
    return 0;
}

void DefaultTarif::set_name(const std::string& name) {
    this->name = name;
}

void DefaultTarif::set_number_of_calls(int calls) {
    this->callnumber = calls;
}

void DefaultTarif::set_base_price(float price) {
    this->baseprice = price;
}

BenefitTarif::BenefitTarif(const std::string& r, int a, float p)
    : name(r), callnumber(a), baseprice(p) {
}

float BenefitTarif::get_cost_of_calls() const {
    return get_tarif_price() * callnumber;
}

std::string BenefitTarif::get_name() const {
    return name;
}

int BenefitTarif::get_number_of_calls() const {
    return callnumber;
}

float BenefitTarif::get_tarif_price() const {
    return baseprice * 0.8f;
}

float BenefitTarif::get_base_price() const {
    return baseprice;
}

int BenefitTarif::get_tarif_type() const {
    return 1;
}

void BenefitTarif::set_name(const std::string& name) {
    this->name = name;
}

void BenefitTarif::set_number_of_calls(int calls) {
    this->callnumber = calls;
}

void BenefitTarif::set_base_price(float price) {
    this->baseprice = price;
}

ATC::ATC() {
    std::cout << "АТС создана!" << std::endl;
}

ATC::~ATC() {
    std::cout << "АТС уничтожена. Клиентов: " << tarifs.size() << std::endl;
}

ATC* ATC::getInstance() {
    if (instance == nullptr) {
        instance = new ATC();
    }
    return instance;
}

void ATC::destroyInstance() {
    delete instance;
    instance = nullptr;
}

void ATC::register_client(const std::string& name, int calls, int tarif_type, float price) {
    if (name.empty() || calls < 0 || price <= 0) {
        throw std::invalid_argument("Некорректные данные клиента");
    }

    switch (tarif_type) {
    case 0:
        tarifs.push_back(std::make_unique<DefaultTarif>(name, calls, price));
        break;
    case 1:
        tarifs.push_back(std::make_unique<BenefitTarif>(name, calls, price));
        break;
    default:
        throw std::invalid_argument("Неизвестный тип тарифа");
    }
}

bool ATC::remove_client(int index) {
    if (index < 0 || index >= (int)tarifs.size()) {
        return false;
    }
    tarifs.erase(tarifs.begin() + index);
    return true;
}

bool ATC::update_client(int index, const std::string& name, int calls, int tarif_type, float price) {
    if (index < 0 || index >= (int)tarifs.size() ||
        name.empty() || calls < 0 || price <= 0) {
        return false;
    }

    tarifs.erase(tarifs.begin() + index);
    register_client(name, calls, tarif_type, price);
    return true;
}

size_t ATC::get_client_count() const {
    return tarifs.size();
}

const TarifStrategy* ATC::get_client(int index) const {
    if (index < 0 || index >= (int)tarifs.size()) {
        return nullptr;
    }
    return tarifs[index].get();
}

void ATC::sort_by_name() {
    std::sort(tarifs.begin(), tarifs.end(),
        [](const std::unique_ptr<TarifStrategy>& a,
            const std::unique_ptr<TarifStrategy>& b) {
                return a->get_name() < b->get_name();
        });
}

void ATC::sort_by_cost() {
    std::sort(tarifs.begin(), tarifs.end(),
        [](const std::unique_ptr<TarifStrategy>& a,
            const std::unique_ptr<TarifStrategy>& b) {
                return a->get_cost_of_calls() < b->get_cost_of_calls();
        });
}

void ATC::sort_by_calls() {
    std::sort(tarifs.begin(), tarifs.end(),
        [](const std::unique_ptr<TarifStrategy>& a,
            const std::unique_ptr<TarifStrategy>& b) {
                return a->get_number_of_calls() < b->get_number_of_calls();
        });
}

bool ATC::save_to_file(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    for (const auto& client : tarifs) {
        file << client->get_name() << ","
            << client->get_number_of_calls() << ","
            << client->get_base_price() << ","
            << client->get_tarif_type() << "\n";
    }

    file.close();
    return true;
}

bool ATC::load_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    tarifs.clear();

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string name;
        std::string calls_str, price_str, type_str;

        if (std::getline(ss, name, ',') &&
            std::getline(ss, calls_str, ',') &&
            std::getline(ss, price_str, ',') &&
            std::getline(ss, type_str)) {

            try {
                int calls = std::stoi(calls_str);
                float price = std::stof(price_str);
                int type = std::stoi(type_str);

                register_client(name, calls, type, price);
            }
            catch (...) {
                continue;
            }
        }
    }

    file.close();
    return true;
}

float ATC::get_average_tarif_price() const {
    if (tarifs.empty()) return 0.0f;

    float total = 0.0f;
    for (const auto& client : tarifs) {
        total += client->get_tarif_price();
    }
    return total / tarifs.size();
}

float ATC::get_total_income() const {
    float total = 0.0f;
    for (const auto& client : tarifs) {
        total += client->get_cost_of_calls();
    }
    return total;
}

std::string ATC::get_most_expensive_client() const {
    if (tarifs.empty()) return "Нет данных";

    float maxCost = 0;
    std::string mostExpensive;

    for (const auto& client : tarifs) {
        float cost = client->get_cost_of_calls();
        if (cost > maxCost) {
            maxCost = cost;
            mostExpensive = client->get_name();
        }
    }

    return mostExpensive;
}

bool ATC::validate_name(const std::string& name) {
    return !name.empty() && name.length() <= 100;
}

bool ATC::validate_calls(int calls) {
    return calls >= 0 && calls <= 10000;
}

bool ATC::validate_price(float price) {
    return price > 0 && price <= 10000;
}