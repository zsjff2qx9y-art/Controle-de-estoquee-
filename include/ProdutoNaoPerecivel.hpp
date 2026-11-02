#ifndef PRODUTO_NAO_PERECIVEL_HPP
#define PRODUTO_NAO_PERECIVEL_HPP

#include "Produto.hpp"

class ProdutoNaoPerecivel : public Produto {
private:
    std::string categoria;

public:
    ProdutoNaoPerecivel(std::string nome, long long precoCentavos, int quantidade,
                        Data dataCadastro, std::string categoria);
    const std::string& getCategoria() const;
    std::string tipo() const override;
    void exibir(std::ostream& saida) const override;
    void salvar(std::ostream& saida) const override;
};

#endif
