#include <iostream>
#include <vector> 
#include <windows.h>
#include <clocale>
#include <string>
#include <limits>
#undef max

class Tarif {
private: 
    int id;
    std::string name;
    float cost;

    static int generate_id(){
        static int k;
        return k++;
    }

public:
    Tarif(std::string a = "", float b = 0) {
        id = generate_id();
        name = a;
        cost = b;
    }

    int get_id() {
        return id;
    }

    std::string get_name() {
        return name;
    }

    float get_cost() {
        return cost;
    }
};

class Client {
private:
    int id;
    std::string name;
    Tarif tarif;
    int calls;

    static int generate_id() {
        static int k;
        return k++;
    }

public:
    Client(std::string a, Tarif b, int c = 0) {
        id = generate_id();
        name = a;
        tarif = b;
        calls = c;
    }

    int get_id() {
        return id;
    }

    std::string get_name() {
        return name;
    }

    Tarif get_Tarif() {
        return tarif;
    }

    int get_calls() {
        return calls;
    }

    float get_cost_of_calls() {
        return calls * tarif.get_cost();
    }

    void change_number_of_calls(int n)
    {
        calls = n;
    }
};

class ATC {
private:
    static ATC* instance;
    ATC() {
        std::cout << "АТС работает.";
    }

    std::vector<Tarif> tarifs;
    std::vector<Client> clients;

public:
    static ATC* getInstance() {
        if (instance == nullptr) {
            instance = new ATC();
        }
        return instance;
    }

    void add_tarif(std::string a, float b) {
        if (a != "" && b > 0 && b < 214483647) {
            tarifs.push_back(Tarif(a, b));
        }
        else {
            std::cout << "Ошибка в введенных значениях!";
        }
    }

    void print_tarif() {
        for (int i = 0; i < tarifs.size(); i++) {
            std::cout << "\nТариф: '" << tarifs[i].get_name() << "'\n";
            std::cout << "Номер тарифа: " << tarifs[i].get_id() << "\n";
            std::cout << "Стоимость тарифа: " << tarifs[i].get_cost() << "\n";
        }
    }

    void register_client(std::string a, int trafic_id, int b = 0) {
        if (a == "" || b < 0) {
            std::cout << "Введены неправильные значения." << std::endl;
        }

        for (int i = 0; i < tarifs.size(); i++) {
            if (tarifs[i].get_id() == trafic_id) {
                clients.push_back(Client(a, tarifs[i], b));
                return;
            }
        }
        std::cout << "Нет такого id." << std::endl;

    }

    void print_clients() {
        for (int i = 0; i < clients.size(); i++) {
            std::cout << "\nКлиент '" << clients[i].get_name() << "'" << std::endl;
            std::cout << "Id клиента: " << clients[i].get_id() << " Номер тарифа: " << clients[i].get_Tarif().get_id() << std::endl;
        }
    }

    void change_client_calls(int a, int client_id) {
        for (int i = 0; i < clients.size(); i++) {
            if (clients[i].get_id() == client_id) {
                clients[i].change_number_of_calls(a);
                std::cout << "\nТекущее количество звонков клиента " << clients[i].get_name() << " теперь равно " << a << std::endl;
                return;
            }
        }
        std::cout << "Не существует такого Id клиента" << std::endl;

    }

    void get_cost_all_calls() {
        double value = 0;
        for (int i = 0; i < clients.size(); i++) {
            value += clients[i].get_cost_of_calls();
        }
        std::cout << "\nОбщая стоимость звонков на ATC: " << value << "\n";
    }

    void get_cost_by_surname() {
        std::string surname;
        std::cout << "Введите фамилию клиента: ";
        std::cin >> surname;

        bool found = false;

        for (int i = 0; i < clients.size(); i++) {
            if (clients[i].get_name() == surname) {
                found = true;
                int calls_count = clients[i].get_calls();

                std::cout << "\nСтоимость " << calls_count << " звонков клиента '"
                    << surname << "' по всем тарифам:" << std::endl;

                for (int j = 0; j < tarifs.size(); j++) {
                    float cost = tarifs[j].get_cost() * calls_count;
                    std::cout << "Тариф '" << tarifs[j].get_name() << "': "
                        << cost << " руб." << std::endl;
                }
                break;
            }
        }

        if (!found) {
            std::cout << "Клиент с фамилией '" << surname << "' не найден." << std::endl;
        }
    }

    ~ATC() {
        std::cout << "\n    Деструктор ATC    " << std::endl;
        std::cout << "Удалено клиентов: " << clients.size() << std::endl;
        std::cout << "Удалено тарифов: " << tarifs.size() << std::endl;
    }
};

ATC* ATC::instance = nullptr;


int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    std::locale::global(std::locale("Russian_Russia.1251"));

    ATC* station1 = ATC::getInstance();
    
    std::cout << "\n|==================Меню========================|\n";
    std::cout << "|Добавить тарифы: 1                            |\n";
    std::cout << "|Добавить клиентов: 2                          |\n"; 
    std::cout << "|Поменять количество звонков клиентов: 3       |\n";
    std::cout << "|Узнать стоимость звонков по фамилии клиента: 4|\n";
    std::cout << "|Выход: 0                                      |\n";
    std::cout << "|==============================================|\n";
    std::cout << "\nВаш выбор:";


    int n;
    std::string leave = "";

    std::cin >> n;
    while (n != 0) {
        switch (n) {
        case 1:
            leave = "";

            while (leave != "0") {
                std::string name;
                float cost;

                try {
                    std::cout << "\nНазвание тарифа: ";
                    std::cin >> name;
                    std::cout << "Стоимость тарифа: ";
                    std::cin >> cost;
                }
                catch (...) {
                    std::cout << "Ошибка, попробуйте заново." << std::endl;
                    continue;
                }

                if (std::cin.fail()) {
                    std::cout << "Ошибка, попробуйте заново." << std::endl;
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                station1->add_tarif(name, cost);
                std::cout << "\nЭто был последний тариф? (да - 0, нет - любой другой символ): ";
                std::cin >> leave;
            }

            std::cout << "\nВы записали следующие тарифы:" << std::endl;
            station1->print_tarif();
            std::cout << "\n|==================Меню========================|\n";
            std::cout << "|Добавить тарифы: 1                            |\n";
            std::cout << "|Добавить клиентов: 2                          |\n";
            std::cout << "|Поменять количество звонков клиентов: 3       |\n";
            std::cout << "|Узнать стоимость звонков по фамилии клиента: 4|\n";
            std::cout << "|Выход: 0                                      |\n";
            std::cout << "|==============================================|\n";
            std::cout << "\nВаш выбор:";
            std::cin >> n;
            break;
        case 2:
            leave = "";

            while (leave != "0") {
                std::string name;
                int id_tarif;
                int t;
                try {
                    std::cout << "\nФамилия клиента: ";
                    std::cin >> name;
                    std::cout << "Id тарифа клиента: ";
                    std::cin >> id_tarif;
                }
                catch (...) {
                    std::cout << "Ошибка, попробуйте заново.";
                    continue;
                }
                try {
                    std::cout << "Количество звонков клиента: ";
                    std::cin >> t;
                }
                catch (...) {
                    std::cout << "Ошибка, попробуйте заново.";
                    continue;
                }
                if (std::cin.fail()) {
                    std::cout << "Ошибка, попробуйте заново." << std::endl;
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                station1->register_client(name, id_tarif, t);
                std::cout << "\nЭто был последний клиент? (да - 0, нет - любой другой символ): ";
                std::cin >> leave;
            }

            std::cout << "\nВы записали следующих клиентов:" << std::endl;
            station1->print_clients();
            std::cout << "\n|==================Меню========================|\n";
            std::cout << "|Добавить тарифы: 1                            |\n";
            std::cout << "|Добавить клиентов: 2                          |\n";
            std::cout << "|Поменять количество звонков клиентов: 3       |\n";
            std::cout << "|Узнать стоимость звонков по фамилии клиента: 4|\n";
            std::cout << "|Выход: 0                                      |\n";
            std::cout << "|==============================================|\n";
            std::cout << "\nВаш выбор:";
            std::cin >> n;
            break;
        case 3:
            leave = "";

            while (leave != "0") {
                int id;
                int t;
                try {
                    std::cout << "Введите id клиента: ";
                    std::cin >> id;
                }
                catch (...) {
                    std::cout << "Ошибка, попробуйте заново.";
                    continue;
                }
                try {
                    std::cout << "Количество звонков клиента: ";
                    std::cin >> t;
                }
                catch (...) {
                    std::cout << "Ошибка, попробуйте заново.";
                    continue;
                }
                if (std::cin.fail()) {
                    std::cout << "Ошибка, попробуйте заново." << std::endl;
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    continue;
                }

                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                station1->change_client_calls(t, id);
                std::cout << "\nИзменения закончились? (да - 0, нет - любой другой символ): ";
                std::cin >> leave;
            }
            std::cout << "\n|==================Меню========================|\n";
            std::cout << "|Добавить тарифы: 1                            |\n";
            std::cout << "|Добавить клиентов: 2                          |\n";
            std::cout << "|Поменять количество звонков клиентов: 3       |\n";
            std::cout << "|Узнать стоимость звонков по фамилии клиента: 4|\n";
            std::cout << "|Выход: 0                                      |\n";
            std::cout << "|==============================================|\n";
            std::cout << "\nВаш выбор:";
            std::cin >> n;
            break;
        case 4:
            leave = "";

            while (leave != "0") {
                station1->get_cost_by_surname();
                std::cout << "\nПроверить другого клиента? (да - любой другой символ, нет - 0): ";
                std::cin >> leave;
            }
            std::cout << "\n|==================Меню========================|\n";
            std::cout << "|Добавить тарифы: 1                            |\n";
            std::cout << "|Добавить клиентов: 2                          |\n";
            std::cout << "|Поменять количество звонков клиентов: 3       |\n";
            std::cout << "|Узнать стоимость звонков по фамилии клиента: 4|\n";
            std::cout << "|Выход: 0                                      |\n";
            std::cout << "|==============================================|\n";
            std::cout << "\nВаш выбор:";
            std::cin >> n;
            break;
        default:
            std::cout << "\n|==================Меню========================|\n";
            std::cout << "|Добавить тарифы: 1                            |\n";
            std::cout << "|Добавить клиентов: 2                          |\n";
            std::cout << "|Поменять количество звонков клиентов: 3       |\n";
            std::cout << "|Узнать стоимость звонков по фамилии клиента: 4|\n";
            std::cout << "|Выход: 0                                      |\n";
            std::cout << "|==============================================|\n";
            std::cout << "\nВаш выбор:";
            std::cin >> n;
            break;
        }
    }

    station1->get_cost_all_calls();

    delete station1;
}
