#include "Class.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iomanip>

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

Database::Database() : db(nullptr) {}

Database::~Database() {
    close();
}

bool Database::executeQuery(const std::string& query) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

bool Database::open(const std::string& path) {
    dbPath = path;
    int rc = sqlite3_open(path.c_str(), &db);

    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    std::cout << "Database opened successfully: " << path << std::endl;
    return true;
}

bool Database::close() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
    return true;
}

bool Database::isOpen() const {
    return db != nullptr;
}

bool Database::createTables() {
    if (!isOpen()) return false;

    std::string query =
        "CREATE TABLE IF NOT EXISTS clients ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "calls INTEGER NOT NULL, "
        "base_price REAL NOT NULL, "
        "tarif_type INTEGER NOT NULL, "
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ");";

    return executeQuery(query);
}

bool Database::insertClient(const std::string& name, int calls, float basePrice, int tarifType) {
    if (!isOpen()) return false;

    std::stringstream ss;
    ss << "INSERT INTO clients (name, calls, base_price, tarif_type) VALUES ('"
        << name << "', " << calls << ", " << basePrice << ", " << tarifType << ");";

    return executeQuery(ss.str());
}

bool Database::updateClient(int id, const std::string& name, int calls, float basePrice, int tarifType) {
    if (!isOpen()) return false;

    std::stringstream ss;
    ss << "UPDATE clients SET name = '" << name
        << "', calls = " << calls
        << ", base_price = " << basePrice
        << ", tarif_type = " << tarifType
        << " WHERE id = " << id << ";";

    return executeQuery(ss.str());
}

bool Database::deleteClient(int id) {
    if (!isOpen()) return false;

    std::stringstream ss;
    ss << "DELETE FROM clients WHERE id = " << id << ";";

    return executeQuery(ss.str());
}

static int clientCallback(void* data, int argc, char** argv, char** azColName) {
    auto* tarifs = static_cast<std::vector<std::unique_ptr<TarifStrategy>>*>(data);

    std::string name = argv[1] ? argv[1] : "";
    int calls = argv[2] ? std::atoi(argv[2]) : 0;
    float basePrice = argv[3] ? std::atof(argv[3]) : 0.0f;
    int tarifType = argv[4] ? std::atoi(argv[4]) : 0;

    if (tarifType == 0) {
        tarifs->push_back(std::make_unique<DefaultTarif>(name, calls, basePrice));
    }
    else {
        tarifs->push_back(std::make_unique<BenefitTarif>(name, calls, basePrice));
    }

    return 0;
}

bool Database::getAllClients(std::vector<std::unique_ptr<TarifStrategy>>& tarifs) {
    if (!isOpen()) return false;

    tarifs.clear();

    char* errMsg = nullptr;
    std::string query = "SELECT id, name, calls, base_price, tarif_type FROM clients ORDER BY id;";

    int rc = sqlite3_exec(db, query.c_str(), clientCallback, &tarifs, &errMsg);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

bool Database::clearTable() {
    if (!isOpen()) return false;

    return executeQuery("DELETE FROM clients;");
}

bool Database::exportToCSV(const std::string& filename) {
    if (!isOpen()) return false;

    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << "id,name,calls,base_price,tarif_type\n";

    sqlite3_stmt* stmt;
    std::string query = "SELECT id, name, calls, base_price, tarif_type FROM clients;";

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            int calls = sqlite3_column_int(stmt, 2);
            double basePrice = sqlite3_column_double(stmt, 3);
            int tarifType = sqlite3_column_int(stmt, 4);

            file << id << ","
                << (name ? name : "") << ","
                << calls << ","
                << std::fixed << std::setprecision(2) << basePrice << ","
                << tarifType << "\n";
        }
        sqlite3_finalize(stmt);
    }

    file.close();
    return true;
}

bool Database::importFromCSV(const std::string& filename) {
    if (!isOpen()) return false;

    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string id_str, name, calls_str, price_str, type_str;

        if (std::getline(ss, id_str, ',') &&
            std::getline(ss, name, ',') &&
            std::getline(ss, calls_str, ',') &&
            std::getline(ss, price_str, ',') &&
            std::getline(ss, type_str)) {

            try {
                int calls = std::stoi(calls_str);
                float price = std::stof(price_str);
                int type = std::stoi(type_str);

                if (!insertClient(name, calls, price, type)) {
                    std::cerr << "Failed to insert client: " << name << std::endl;
                }
            }
            catch (...) {
                continue;
            }
        }
    }

    file.close();
    return true;
}

std::string Database::getDatabaseInfo() const {
    if (!isOpen()) return "Database not open";

    std::stringstream ss;
    ss << "Database: " << dbPath << "\n";

    sqlite3_stmt* stmt;
    std::string query = "SELECT COUNT(*) FROM clients;";

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int count = sqlite3_column_int(stmt, 0);
            ss << "Total clients: " << count << "\n";
        }
        sqlite3_finalize(stmt);
    }

    query = "SELECT name FROM sqlite_master WHERE type='table';";
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        ss << "Tables: ";
        bool first = true;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* tableName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            if (!first) ss << ", ";
            ss << tableName;
            first = false;
        }
        sqlite3_finalize(stmt);
    }

    return ss.str();
}

ATC::ATC() {
    std::cout << "АТС создана!" << std::endl;
    initDatabase();
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

bool ATC::initDatabase() {
    if (!database.open("atc_database.db")) {
        return false;
    }

    if (!database.createTables()) {
        return false;
    }

    loadFromDatabase();
    return true;
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

    database.insertClient(name, calls, price, tarif_type);
}

bool ATC::remove_client(int index) {
    if (index < 0 || index >= (int)tarifs.size()) {
        return false;
    }

    sqlite3_stmt* stmt;
    std::string query = "SELECT id FROM clients ORDER BY id LIMIT 1 OFFSET ?;";
    sqlite3* dbHandle = database.getDBHandle();

    if (sqlite3_prepare_v2(dbHandle,  // ИЗМЕНИТЬ ЭТУ СТРОКУ
        query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, index);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            tarifs.erase(tarifs.begin() + index);
            return database.deleteClient(id);
        }
        sqlite3_finalize(stmt);
    }

    return false;
}

bool ATC::update_client(int index, const std::string& name, int calls, int tarif_type, float price) {
    if (index < 0 || index >= (int)tarifs.size() ||
        name.empty() || calls < 0 || price <= 0) {
        return false;
    }

    sqlite3_stmt* stmt;
    std::string query = "SELECT id FROM clients ORDER BY id LIMIT 1 OFFSET ?;";
    sqlite3* dbHandle = database.getDBHandle();

    if (sqlite3_prepare_v2(dbHandle,  // ИЗМЕНИТЬ ЭТУ СТРОКУ
        query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, index);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);

            tarifs.erase(tarifs.begin() + index);
            register_client(name, calls, tarif_type, price);

            return database.updateClient(id, name, calls, price, tarif_type);
        }
        sqlite3_finalize(stmt);
    }

    return false;
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

bool ATC::loadFromDatabase() {
    return database.getAllClients(tarifs);
}

bool ATC::saveToDatabase() {
    database.clearTable();

    for (const auto& client : tarifs) {
        if (!database.insertClient(client->get_name(),
            client->get_number_of_calls(),
            client->get_base_price(),
            client->get_tarif_type())) {
            return false;
        }
    }

    return true;
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

bool ATC::exportToFile(const std::string& filename) {
    return database.exportToCSV(filename);
}

bool ATC::importFromFile(const std::string& filename) {
    if (!database.importFromCSV(filename)) {
        return false;
    }

    return loadFromDatabase();
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

std::string ATC::getDatabaseInfo() const {
    return database.getDatabaseInfo();
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