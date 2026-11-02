#ifndef ESTOQUE_HPP
#define ESTOQUE_HPP

#include "Produto.hpp"

#include <cstddef>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

// Gerencia o vetor polimorfico. O arquivo somente e usado ao carregar/salvar.
class Estoque {
private:
    std::vector<std::unique_ptr<Produto>> produtos;
    std::string caminhoArquivo;
    bool carregado = false;

public:
    explicit Estoque(std::string caminhoArquivo);

    void adicionar(std::unique_ptr<Produto> produto);
    const std::vector<std::unique_ptr<Produto>>& listar() const;
    std::vector<std::size_t> pesquisar(const std::string& nome) const;
    void alterar(std::size_t indice, std::unique_ptr<Produto> produto);
    void remover(std::size_t indice);

    void carregar();
    void salvar() const;
    void relatorio(std::ostream& saida) const;
};

#endif
