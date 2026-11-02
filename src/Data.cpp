#include "Data.hpp"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

Data::Data(const std::string& iso) : ano(0), mes(0), dia(0) {
    if (iso.size() != 10 || iso[4] != '-' || iso[7] != '-') {
        throw std::invalid_argument("Data invalida. Use AAAA-MM-DD.");
    }
    for (std::size_t i = 0; i < iso.size(); ++i) {
        if (i != 4 && i != 7 && (iso[i] < '0' || iso[i] > '9')) {
            throw std::invalid_argument("Data invalida. Use AAAA-MM-DD.");
        }
    }
    ano = std::stoi(iso.substr(0, 4));
    mes = std::stoi(iso.substr(5, 2));
    dia = std::stoi(iso.substr(8, 2));
    if (ano < 1 || mes < 1 || mes > 12) {
        throw std::invalid_argument("Ano ou mes invalido.");
    }
    const bool bissexto = ano % 400 == 0 || (ano % 4 == 0 && ano % 100 != 0);
    const int diasNoMes[] = {31, bissexto ? 29 : 28, 31, 30, 31, 30,
                             31, 31, 30, 31, 30, 31};
    if (dia < 1 || dia > diasNoMes[mes - 1]) {
        throw std::invalid_argument("Dia inexistente nesse mes.");
    }
}

Data Data::hoje() {
    const std::time_t agora = std::time(nullptr);
    const std::tm* local = std::localtime(&agora);
    if (local == nullptr) {
        throw std::runtime_error("Nao foi possivel consultar a data atual.");
    }
    std::ostringstream texto;
    texto << std::put_time(local, "%Y-%m-%d");
    return Data(texto.str());
}

std::string Data::toString() const {
    std::ostringstream texto;
    texto << std::setfill('0') << std::setw(4) << ano << '-'
          << std::setw(2) << mes << '-' << std::setw(2) << dia;
    return texto.str();
}

bool Data::operator<(const Data& outra) const {
    if (ano != outra.ano) {
        return ano < outra.ano;
    }
    if (mes != outra.mes) {
        return mes < outra.mes;
    }
    return dia < outra.dia;
}

bool Data::operator==(const Data& outra) const {
    return ano == outra.ano && mes == outra.mes && dia == outra.dia;
}
