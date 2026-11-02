#include "ProdutoNaoPerecivel.hpp"

#include <iomanip>
#include <ostream>
#include <stdexcept>
#include <utility>

ProdutoNaoPerecivel::ProdutoNaoPerecivel(std::string nome, long long precoCentavos,
                                        int quantidade, Data dataCadastro,
                                        std::string categoria)
    : Produto(std::move(nome), precoCentavos, quantidade, std::move(dataCadastro)),
      categoria(std::move(categoria)) {
    for (unsigned char caractere : this->categoria) {
        if (caractere < 32 || caractere == 127) {
            throw std::invalid_argument("A categoria nao pode conter caracteres de controle.");
        }
    }
    const auto inicio = this->categoria.find_first_not_of(' ');
    const auto fim = this->categoria.find_last_not_of(' ');
    if (inicio == std::string::npos) {
        throw std::invalid_argument("A categoria nao pode ficar vazia.");
    }
    this->categoria = this->categoria.substr(inicio, fim - inicio + 1);
    if (this->categoria.size() > 80) {
        throw std::invalid_argument("A categoria deve ter no maximo 80 bytes.");
    }
}

const std::string& ProdutoNaoPerecivel::getCategoria() const {
    return categoria;
}

std::string ProdutoNaoPerecivel::tipo() const {
    return "NAO_PERECIVEL";
}

void ProdutoNaoPerecivel::exibir(std::ostream& saida) const {
    saida << "Tipo: Nao perecivel\n";
    Produto::exibir(saida);
    saida << "Categoria: " << categoria << '\n';
}

void ProdutoNaoPerecivel::salvar(std::ostream& saida) const {
    saida << tipo() << ' ' << std::quoted(getNome()) << ' '
          << getPrecoCentavos() << ' ' << getQuantidade() << ' '
          << std::quoted(getDataCadastro().toString()) << ' '
          << std::quoted(categoria) << '\n';
}
