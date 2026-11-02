#include "Produto.hpp"

#include <iomanip>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace {
std::string dinheiro(long long centavos) {
    std::ostringstream texto;
    texto << "R$ " << centavos / 100 << ',' << std::setfill('0')
          << std::setw(2) << centavos % 100;
    return texto.str();
}
}

Produto::Produto(std::string nome, long long precoCentavos, int quantidade,
                 Data dataCadastro)
    : nome(std::move(nome)), precoCentavos(precoCentavos),
      quantidade(quantidade), dataCadastro(std::move(dataCadastro)) {
    for (unsigned char caractere : this->nome) {
        if (caractere < 32 || caractere == 127) {
            throw std::invalid_argument("O nome nao pode conter caracteres de controle.");
        }
    }
    const auto inicio = this->nome.find_first_not_of(' ');
    const auto fim = this->nome.find_last_not_of(' ');
    if (inicio == std::string::npos) {
        throw std::invalid_argument("O nome nao pode ficar vazio.");
    }
    this->nome = this->nome.substr(inicio, fim - inicio + 1);
    if (this->nome.size() > 120) {
        throw std::invalid_argument("O nome deve ter no maximo 120 bytes.");
    }
    if (precoCentavos < 1 || precoCentavos > 1000000000LL) {
        throw std::invalid_argument("Preco permitido: de R$ 0,01 a R$ 10000000,00.");
    }
    if (quantidade < 0 || quantidade > 100000000) {
        throw std::invalid_argument("Quantidade permitida: de 0 a 100000000.");
    }
}

const std::string& Produto::getNome() const {
    return nome;
}

long long Produto::getPrecoCentavos() const {
    return precoCentavos;
}

int Produto::getQuantidade() const {
    return quantidade;
}

const Data& Produto::getDataCadastro() const {
    return dataCadastro;
}

long long Produto::valorEmEstoque() const {
    return precoCentavos * quantidade;
}

void Produto::exibir(std::ostream& saida) const {
    saida << "Nome: " << nome << '\n'
          << "Preco unitario: " << dinheiro(precoCentavos) << '\n'
          << "Quantidade: " << quantidade << '\n'
          << "Valor em estoque: " << dinheiro(valorEmEstoque()) << '\n'
          << "Data de cadastro: " << dataCadastro.toString() << '\n';
}
