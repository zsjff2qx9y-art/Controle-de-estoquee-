#ifndef DATA_HPP
#define DATA_HPP

#include <string>

// Representa uma data civil valida, armazenada por valor nos produtos.
class Data {
private:
    int ano;
    int mes;
    int dia;

public:
    explicit Data(const std::string& iso);
    static Data hoje();
    std::string toString() const;
    bool operator<(const Data& outra) const;
    bool operator==(const Data& outra) const;
};

#endif
