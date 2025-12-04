#ifndef CLASS_H
#define CLASS_H

#include <string>
#include <vector>

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
	DefaultTarif(std::string r, int a, float p);
	float get_cost_of_calls() override;
	std::string get_name() override;
	int get_number_of_calls() override;
	float get_price() override;
};

class BenefitTarif : public TarifStrategy {
private:
	int callnumber;
	std::string name;
	float price;
public:
	BenefitTarif(std::string r, int a, float p);
	float get_cost_of_calls() override;
	std::string get_name() override;
	int get_number_of_calls() override;
	float get_price() override;
};

class ATC {
private:
	static ATC* instance;
	std::vector<TarifStrategy*> tarifs;
	std::string name;
	int callnumber = 0;

	ATC();
public:
	static ATC* getInstance();
	~ATC();

	void register_client(std::string r, int a, int tarif_type, float p);
	void print_clients();
	void average_tarif_price();
};

#endif