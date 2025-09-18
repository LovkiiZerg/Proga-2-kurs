#include <iostream>
#include <stdexcept>

class ATC {
private:
	float price;
	int users;
public:
	std::string adress;
	void plata()
	{
		std::cout << "Абонентская плата всех клиентов: " << users * price << std::endl;
	}
	void place()
	{
		std::cout << "Адрес АТС: " << adress << std::endl;
	}
	ATC(std::string p_adress, float p_price, int p_users)
	{
		if (p_adress.empty()) {
			throw std::invalid_argument("Строка не может быть пустой!");
		}
		if (p_users <= 0) {
			throw std::invalid_argument("Количество пользователей должно быть больше нуля!");
		}
		if (p_price <= 0) {
			throw std::invalid_argument("Цена не может быть меньше либо равна 0!");
		}
		adress = p_adress;
		price = p_price;
		users = p_users;
	}
};

int main()
{
	setlocale(LC_ALL, "rus");
	try {
		ATC dom{"ул. Пушкина д.15", 499.99, 10};
		dom.plata();
		dom.place();
	}
	catch (const std::exception& e) {
		std::cerr << "Ошибка: " << e.what() << std::endl;
		return 1;
	}
}
