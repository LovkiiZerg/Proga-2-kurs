#include <iostream>

class ATC {
private:
	int users = 50;
	float price = 499.99;
	std::string adress = "Pushkina";
public:
	void print()
	{
		setlocale(LC_ALL, "rus");
		std::cout << "Абонентская плата всех клиентов: " << users * price << std::endl;
	}
};

int main()
{
	ATC dom;
	dom.print();
}
