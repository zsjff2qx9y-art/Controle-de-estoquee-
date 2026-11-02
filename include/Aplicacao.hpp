#pragma once

#include "Estoque.hpp"
#include <memory>
#include <string>

class Aplicacao {
private:
    Estoque estoque;
    int pesquisar(std::string nome);
    std::unique_ptr<Produto> lerProduto(const Data& cadastro);
    void inserir();
    void exibirUm();
    void alterar();
    void remover();

public:
    explicit Aplicacao(std::string caminhoArquivo);
    void executar();
};
