#ifndef PRODUTO_PERECIVEL_HPP
#define PRODUTO_PERECIVEL_HPP

#include "Produto.hpp"

class ProdutoPerecivel : public Produto {
private:
    Data validade;

public:
    ProdutoPerecivel(std::string nome, long long precoCentavos, int quantidade,
                     Data dataCadastro, Data validade);
    const Data& getValidade() const;
    std::string tipo() const override;
    void exibir(std::ostream& saida) const override;
    void salvar(std::ostream& saida) const override;
};

#endif
