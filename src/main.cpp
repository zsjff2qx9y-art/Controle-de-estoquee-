#include "Aplicacao.hpp"
#include <exception>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc > 2) {
        std::cerr << "Uso: " << argv[0] << " [caminho-do-arquivo]\n";
        return 1;
    }
    try {
        Aplicacao app(argc == 2 ? argv[1] : "dados/estoque.txt");
        app.executar();
        return 0;
    } catch (const std::exception& erro) {
        std::cerr << "Erro: " << erro.what() << "\nO arquivo original foi preservado.\n";
        return 1;
    }
}
