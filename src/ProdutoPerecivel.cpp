#include "ProdutoPerecivel.hpp"

#include <iomanip>
#include <ostream>
#include <utility>

ProdutoPerecivel::ProdutoPerecivel(std::string nome, long long precoCentavos,
                                   int quantidade, Data dataCadastro,
                                   Data validade)
    : Produto(std::move(nome), precoCentavos, quantidade, std::move(dataCadastro)),
      validade(std::move(validade)) {}

const Data& ProdutoPerecivel::getValidade() const {
    return validade;
}

std::string ProdutoPerecivel::tipo() const {
    return "PERECIVEL";
}

void ProdutoPerecivel::exibir(std::ostream& saida) const {
    saida << "Tipo: Perecivel\n";
    Produto::exibir(saida);
    saida << "Validade: " << validade.toString() << '\n';
}

void ProdutoPerecivel::salvar(std::ostream& saida) const {
    saida << tipo() << ' ' << std::quoted(getNome()) << ' '
          << getPrecoCentavos() << ' ' << getQuantidade() << ' '
          << std::quoted(getDataCadastro().toString()) << ' '
          << std::quoted(validade.toString()) << '\n';
}
