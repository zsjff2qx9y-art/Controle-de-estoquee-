#include "Estoque.hpp"
#include "Data.hpp"
#include "ProdutoNaoPerecivel.hpp"
#include "ProdutoPerecivel.hpp"
#include "ResumoEstoque.hpp"

#include <chrono>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <system_error>
#include <utility>

namespace {

std::string normalizar(const std::string& texto) {
    const auto inicio = texto.find_first_not_of(" \t\r\n");
    if (inicio == std::string::npos) {
        return {};
    }
    const auto fim = texto.find_last_not_of(" \t\r\n");
    std::string resultado = texto.substr(inicio, fim - inicio + 1);
    // Converte apenas ASCII: caracteres UTF-8 com acento ficam preservados.
    for (char& caractere : resultado) {
        if (caractere >= 'A' && caractere <= 'Z') {
            caractere = static_cast<char>(caractere - 'A' + 'a');
        }
    }
    return resultado;
}

void somarProduto(const Produto& produto, long long& quantidade, long long& valor) {
    const auto limite = std::numeric_limits<long long>::max();
    if (quantidade > limite - produto.getQuantidade() ||
        valor > limite - produto.valorEmEstoque()) {
        throw std::overflow_error("O total do estoque ultrapassa o limite numerico.");
    }
    quantidade += produto.getQuantidade();
    valor += produto.valorEmEstoque();
}

void validarTotais(const std::vector<std::unique_ptr<Produto>>& produtos,
                   const Produto* novo = nullptr,
                   std::size_t ignorado = std::numeric_limits<std::size_t>::max()) {
    long long quantidade = 0;
    long long valor = 0;
    for (std::size_t indice = 0; indice < produtos.size(); ++indice) {
        if (indice != ignorado) {
            somarProduto(*produtos[indice], quantidade, valor);
        }
    }
    if (novo != nullptr) {
        somarProduto(*novo, quantidade, valor);
    }
}

std::string dinheiro(long long centavos) {
    std::ostringstream texto;
    texto << "R$ " << centavos / 100 << ',' << std::setfill('0')
          << std::setw(2) << centavos % 100;
    return texto.str();
}

void exibirResumo(std::ostream& saida, const std::string& titulo, const ResumoEstoque& resumo) {
    saida << titulo << ": " << resumo.cadastros << " cadastro(s), "
          << resumo.unidades << " unidade(s), " << dinheiro(resumo.valor) << '\n';
}

} // namespace

Estoque::Estoque(std::string caminhoArquivo)
    : caminhoArquivo(std::move(caminhoArquivo)) {
    if (this->caminhoArquivo.empty()) {
        throw std::invalid_argument("Informe o caminho do arquivo de estoque.");
    }
}

void Estoque::adicionar(std::unique_ptr<Produto> produto) {
    if (!produto) {
        throw std::invalid_argument("Produto nao pode ser nulo.");
    }
    validarTotais(produtos, produto.get());
    produtos.push_back(std::move(produto));
}

const std::vector<std::unique_ptr<Produto>>& Estoque::listar() const {
    return produtos;
}

std::vector<std::size_t> Estoque::pesquisar(const std::string& nome) const {
    const std::string prefixo = normalizar(nome);
    std::vector<std::size_t> encontrados;
    if (prefixo.empty()) {
        return encontrados;
    }
    for (std::size_t indice = 0; indice < produtos.size(); ++indice) {
        const std::string candidato = normalizar(produtos[indice]->getNome());
        if (candidato.compare(0, prefixo.size(), prefixo) == 0) {
            encontrados.push_back(indice);
        }
    }
    return encontrados;
}

void Estoque::alterar(std::size_t indice, std::unique_ptr<Produto> produto) {
    if (indice >= produtos.size()) {
        throw std::out_of_range("Indice de produto inexistente.");
    }
    if (!produto) {
        throw std::invalid_argument("Produto nao pode ser nulo.");
    }
    validarTotais(produtos, produto.get(), indice);
    produtos[indice] = std::move(produto);
}

void Estoque::remover(std::size_t indice) {
    if (indice >= produtos.size()) {
        throw std::out_of_range("Indice de produto inexistente.");
    }
    produtos.erase(produtos.begin() + static_cast<std::ptrdiff_t>(indice));
}

void Estoque::carregar() {
    // Uma falha de leitura bloqueia a gravacao para preservar o arquivo original.
    carregado = false;
    std::error_code erro;
    const bool existe = std::filesystem::exists(caminhoArquivo, erro);
    if (erro) {
        throw std::runtime_error("Nao foi possivel consultar o arquivo: " + erro.message());
    }
    if (!existe) {
        produtos.clear();
        carregado = true;
        return;
    }

    std::ifstream arquivo(caminhoArquivo);
    if (!arquivo) {
        throw std::runtime_error("Nao foi possivel abrir o arquivo de estoque para leitura.");
    }
    std::string linha;
    if (!std::getline(arquivo, linha) || linha != "ESTOQUE_V1") {
        throw std::runtime_error("Arquivo de estoque invalido: cabecalho/versao (linha 1).");
    }

    std::vector<std::unique_ptr<Produto>> lidos;
    std::size_t numeroLinha = 1;
    while (std::getline(arquivo, linha)) {
        ++numeroLinha;
        if (normalizar(linha).empty()) {
            continue;
        }
        try {
            std::istringstream registro(linha);
            std::string tipo;
            std::string nome;
            std::string cadastro;
            std::string especifico;
            long long preco = 0;
            int quantidade = 0;
            if (!(registro >> tipo >> std::quoted(nome) >> preco >> quantidade
                  >> std::quoted(cadastro) >> std::quoted(especifico))) {
                throw std::invalid_argument("campos ausentes ou invalidos");
            }
            registro >> std::ws;
            if (!registro.eof()) {
                throw std::invalid_argument("conteudo extra no registro");
            }

            std::unique_ptr<Produto> produto;
            if (tipo == "PERECIVEL") {
                produto = std::make_unique<ProdutoPerecivel>(
                    nome, preco, quantidade, Data(cadastro), Data(especifico));
            } else if (tipo == "NAO_PERECIVEL") {
                produto = std::make_unique<ProdutoNaoPerecivel>(
                    nome, preco, quantidade, Data(cadastro), especifico);
            } else {
                throw std::invalid_argument("tipo de produto desconhecido");
            }
            validarTotais(lidos, produto.get());
            lidos.push_back(std::move(produto));
        } catch (const std::exception& causa) {
            throw std::runtime_error("Arquivo de estoque invalido na linha " +
                                     std::to_string(numeroLinha) + ": " + causa.what());
        }
    }
    if (arquivo.bad()) {
        throw std::runtime_error("Falha ao ler o arquivo de estoque.");
    }
    produtos.swap(lidos);
    carregado = true;
}

void Estoque::salvar() const {
    if (!carregado) {
        throw std::runtime_error("Gravacao bloqueada: o estoque nao foi carregado com sucesso.");
    }
    const std::filesystem::path destino(caminhoArquivo);
    if (!destino.parent_path().empty()) {
        std::filesystem::create_directories(destino.parent_path());
    }
    // Grava primeiro ao lado do destino; somente depois substitui o arquivo.
    const auto sufixo = std::chrono::steady_clock::now().time_since_epoch().count();
    std::filesystem::path temporario = destino;
    temporario += ".tmp-" + std::to_string(sufixo);
    try {
        std::ofstream arquivo(temporario, std::ios::trunc);
        if (!arquivo) {
            throw std::runtime_error("Nao foi possivel criar o arquivo temporario de estoque.");
        }
        arquivo << "ESTOQUE_V1\n";
        for (const auto& produto : produtos) {
            produto->salvar(arquivo);
        }
        arquivo.flush();
        if (!arquivo) {
            throw std::runtime_error("Falha ao gravar o arquivo de estoque.");
        }
        arquivo.close();
        if (!arquivo) {
            throw std::runtime_error("Falha ao finalizar o arquivo de estoque.");
        }
        std::filesystem::rename(temporario, destino);
    } catch (...) {
        std::error_code ignorado;
        std::filesystem::remove(temporario, ignorado);
        throw;
    }
}

void Estoque::relatorio(std::ostream& saida) const {
    ResumoEstoque total;
    ResumoEstoque pereciveis;
    ResumoEstoque naoPereciveis;
    std::size_t estoqueBaixo = 0;
    for (const auto& produto : produtos) {
        ++total.cadastros;
        somarProduto(*produto, total.unidades, total.valor);
        ResumoEstoque& tipo = produto->tipo() == "PERECIVEL" ? pereciveis : naoPereciveis;
        ++tipo.cadastros;
        somarProduto(*produto, tipo.unidades, tipo.valor);
        if (produto->getQuantidade() <= 5) {
            ++estoqueBaixo;
        }
    }
    saida << "\n=== RELATORIO DE ESTOQUE ===\n";
    exibirResumo(saida, "Total", total);
    exibirResumo(saida, "Pereciveis", pereciveis);
    exibirResumo(saida, "Nao pereciveis", naoPereciveis);
    saida << "Cadastros com estoque baixo (ate 5 unidades): " << estoqueBaixo << '\n';
}
