#ifndef PRODUTO_HPP
#define PRODUTO_HPP

#include "Data.hpp"

#include <iosfwd>
#include <string>

class Produto {
private:
    std::string nome;
    long long precoCentavos;
    int quantidade;
    Data dataCadastro;

public:
    Produto(std::string nome, long long precoCentavos, int quantidade,
            Data dataCadastro);
    virtual ~Produto() = default;

    const std::string& getNome() const;
    long long getPrecoCentavos() const;
    int getQuantidade() const;
    const Data& getDataCadastro() const;
    long long valorEmEstoque() const;

    virtual std::string tipo() const = 0;
    virtual void exibir(std::ostream& saida) const;
    virtual void salvar(std::ostream& saida) const = 0;
};

#endif
