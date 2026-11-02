#include "Aplicacao.hpp"
#include "ProdutoNaoPerecivel.hpp"
#include "ProdutoPerecivel.hpp"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <stdexcept>
#include <utility>

namespace {
std::string aparar(const std::string& texto) {
    const auto inicio = texto.find_first_not_of(" \t\r\n");
    if (inicio == std::string::npos) return "";
    return texto.substr(inicio, texto.find_last_not_of(" \t\r\n") - inicio + 1);
}

std::string lerLinha(const std::string& mensagem) {
    std::cout << mensagem;
    std::string linha;
    if (!std::getline(std::cin, linha)) {
        // EOF (Ctrl+D/Ctrl+Z) encerra o menu e tambem salva o vetor.
        if (std::cin.eof()) throw std::ios_base::failure("Fim da entrada");
        throw std::runtime_error("Falha na leitura do teclado.");
    }
    return aparar(linha);
}

int lerInteiro(const std::string& mensagem, int minimo, int maximo) {
    while (true) {
        const auto texto = lerLinha(mensagem);
        try {
            std::size_t lidos = 0;
            const long long valor = std::stoll(texto, &lidos);
            if (lidos == texto.size() && valor >= minimo && valor <= maximo)
                return static_cast<int>(valor);
        } catch (const std::exception&) {
            // Entrada fora do intervalo ou sem um numero inteiro.
        }
        std::cout << "Digite um inteiro entre " << minimo << " e " << maximo << ".\n";
    }
}

long long lerPreco() {
    while (true) {
        auto texto = lerLinha("Preco unitario (R$, ex.: 12,50): ");
        std::replace(texto.begin(), texto.end(), ',', '.');
        const auto separador = texto.find('.');
        const auto reais = texto.substr(0, separador);
        auto centavos = separador == std::string::npos ? "" : texto.substr(separador + 1);
        auto digitos = [](const std::string& valor) {
            return std::all_of(valor.begin(), valor.end(), [](unsigned char c) {
                return std::isdigit(c) != 0;
            });
        };
        if (!reais.empty() && reais.size() <= 8 && digitos(reais) &&
            centavos.size() <= 2 && digitos(centavos) &&
            (separador == std::string::npos || !centavos.empty())) {
            while (centavos.size() < 2) centavos += '0';
            const long long valor = std::stoll(reais) * 100 + std::stoll(centavos);
            if (valor > 0 && valor <= 1000000000LL) return valor;
        }
        std::cout << "Preco invalido. Use de 0,01 a 10000000,00, sem separador de milhar.\n";
    }
}

Data lerData(const std::string& mensagem) {
    while (true) {
        try {
            return Data(lerLinha(mensagem));
        } catch (const std::invalid_argument& erro) {
            std::cout << erro.what() << '\n';
        }
    }
}
} // namespace

Aplicacao::Aplicacao(std::string caminhoArquivo) : estoque(std::move(caminhoArquivo)) {}

std::unique_ptr<Produto> Aplicacao::lerProduto(const Data& cadastro) {
    const int tipo = lerInteiro("Tipo (1 - perecivel; 2 - nao perecivel): ", 1, 2);
    const auto nome = lerLinha("Nome completo: ");
    const auto preco = lerPreco();
    const auto quantidade = lerInteiro("Quantidade: ", 0, 100000000);
    if (tipo == 1) {
        const auto validade = lerData("Validade (AAAA-MM-DD): ");
        return std::make_unique<ProdutoPerecivel>(nome, preco, quantidade, cadastro, validade);
    }
    const auto categoria = lerLinha("Categoria: ");
    return std::make_unique<ProdutoNaoPerecivel>(nome, preco, quantidade, cadastro, categoria);
}

// O retorno e o indice no vetor, reutilizado pelas tres operacoes do enunciado.
int Aplicacao::pesquisar(std::string nome) {
    const auto encontrados = estoque.pesquisar(nome);
    if (encontrados.empty()) {
        std::cout << "Nenhum produto encontrado.\n";
        return -1;
    }
    if (encontrados.size() == 1) return static_cast<int>(encontrados.front());
    std::cout << "Mais de um resultado. Indices atuais do vetor (comecam em 0):\n";
    for (const auto indice : encontrados) {
        std::cout << "\nIndice " << indice << ":\n";
        estoque.listar()[indice]->exibir(std::cout);
    }
    while (true) {
        const int indice = lerInteiro("Selecione o indice (-1 cancela): ", -1,
                                     static_cast<int>(estoque.listar().size() - 1));
        if (indice == -1) return -1;
        if (std::find(encontrados.begin(), encontrados.end(), static_cast<std::size_t>(indice))
            != encontrados.end()) return indice;
        std::cout << "Escolha um dos indices exibidos.\n";
    }
}

void Aplicacao::inserir() {
    estoque.adicionar(lerProduto(Data::hoje()));
    std::cout << "Produto inserido.\n";
}

void Aplicacao::exibirUm() {
    const int indice = pesquisar(lerLinha("Nome completo ou inicio do nome: "));
    if (indice >= 0) estoque.listar()[static_cast<std::size_t>(indice)]->exibir(std::cout);
}

void Aplicacao::alterar() {
    const int indice = pesquisar(lerLinha("Nome completo ou inicio do nome: "));
    if (indice < 0) return;
    const auto posicao = static_cast<std::size_t>(indice);
    estoque.listar()[posicao]->exibir(std::cout);
    std::cout << "Informe os novos dados. A data de cadastro sera mantida.\n";
    auto novo = lerProduto(estoque.listar()[posicao]->getDataCadastro());
    estoque.alterar(posicao, std::move(novo));
    std::cout << "Produto alterado.\n";
}

void Aplicacao::remover() {
    const int indice = pesquisar(lerLinha("Nome completo ou inicio do nome: "));
    if (indice < 0) return;
    estoque.listar()[static_cast<std::size_t>(indice)]->exibir(std::cout);
    const auto resposta = lerLinha("Confirmar remocao? (s/n): ");
    if (resposta == "s" || resposta == "S") {
        estoque.remover(static_cast<std::size_t>(indice));
        std::cout << "Produto removido.\n";
    } else {
        std::cout << "Remocao cancelada.\n";
    }
}

void Aplicacao::executar() {
    // Se o arquivo estiver corrompido, carregar lanca excecao: nao sobrescrevemos os dados.
    estoque.carregar();
    bool continuar = true;
    while (continuar) {
        try {
            std::cout << "\n=== CONTROLE DE ESTOQUE ===\n"
                      << "1. Inserir\n2. Listar todos\n3. Exibir um (pesquisar por nome)\n"
                      << "4. Alterar (pesquisar por nome)\n5. Remover (pesquisar por nome)\n"
                      << "6. Exibir relatorio\n7. Sair\n";
            const int opcao = lerInteiro("Opcao: ", 1, 7);
            switch (opcao) {
                case 1: inserir(); break;
                case 2:
                    if (estoque.listar().empty()) std::cout << "Estoque vazio.\n";
                    for (std::size_t i = 0; i < estoque.listar().size(); ++i) {
                        std::cout << "\nIndice " << i << ":\n";
                        estoque.listar()[i]->exibir(std::cout);
                    }
                    break;
                case 3: exibirUm(); break;
                case 4: alterar(); break;
                case 5: remover(); break;
                case 6: estoque.relatorio(std::cout); break;
                case 7: continuar = false; break;
            }
        } catch (const std::ios_base::failure&) {
            std::cout << "\nFim da entrada. Encerrando...\n";
            continuar = false;
        } catch (const std::exception& erro) {
            std::cout << "Operacao nao concluida: " << erro.what() << '\n';
        }
    }
    estoque.salvar();
    std::cout << "Dados salvos. Ate logo!\n";
}
