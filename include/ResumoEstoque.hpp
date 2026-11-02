#pragma once

#include <cstddef>

// Estrutura auxiliar do relatorio; nao representa um cadastro persistido.
struct ResumoEstoque {
    std::size_t cadastros = 0;
    long long unidades = 0;
    long long valor = 0;
};
