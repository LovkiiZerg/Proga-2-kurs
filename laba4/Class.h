#ifndef CLASS_H
#define CLASS_H

#include <string>
#include <vector>
#include <memory>

class TarifStrategy {
public:
    virtual ~TarifStrategy() = default;
    virtual float get_cost_of_calls() const = 0;
    virtual std::string get_name() const = 0;
    virtual int get_number_of_calls() const = 0;
    virtual float get_tarif_price() const = 0;
    virtual float get_base_price() const = 0;
    virtual int get_tarif_type() const = 0;
    virtual void set_name(const std::string& name) = 0;
    virtual void set_number_of_calls(int calls) = 0;
    virtual void set_base_price(float price) = 0;
};

class DefaultTarif : public TarifStrategy {
private:
    int callnumber;
    std::string name;
    float baseprice;

public:
    DefaultTarif(const std::string& r = "", int a = 0, float p = 0.0f);
    float get_cost_of_calls() const override;
    std::string get_name() const override;
    int get_number_of_calls() const override;
    float get_tarif_price() const override;
    float get_base_price() const override;
    int get_tarif_type() const override;
    void set_name(const std::string& name) override;
    void set_number_of_calls(int calls) override;
    void set_base_price(float price) override;
};

class BenefitTarif : public TarifStrategy {
private:
    int callnumber;
    std::string name;
    float baseprice;

public:
    BenefitTarif(const std::string& r = "", int a = 0, float p = 0.0f);
    float get_cost_of_calls() const override;
    std::string get_name() const override;
    int get_number_of_calls() const override;
    float get_tarif_price() const override;
    float get_base_price() const override;
    int get_tarif_type() const override;
    void set_name(const std::string& name) override;
    void set_number_of_calls(int calls) override;
    void set_base_price(float price) override;
};

class ATC {
private:
    static ATC* instance;
    std::vector<std::unique_ptr<TarifStrategy>> tarifs;
    ATC();

public:
    ~ATC();
    static ATC* getInstance();
    static void destroyInstance();
    void register_client(const std::string& name, int calls, int tarif_type, float price);
    bool remove_client(int index);
    bool update_client(int index, const std::string& name, int calls, int tarif_type, float price);
    size_t get_client_count() const;
    const TarifStrategy* get_client(int index) const;
    void sort_by_name();
    void sort_by_cost();
    void sort_by_calls();
    bool save_to_file(const std::string& filename);
    bool load_from_file(const std::string& filename);
    float get_average_tarif_price() const;
    float get_total_income() const;
    std::string get_most_expensive_client() const;
    static bool validate_name(const std::string& name);
    static bool validate_calls(int calls);
    static bool validate_price(float price);
};

#endif