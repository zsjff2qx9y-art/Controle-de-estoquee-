# Controle de estoque em C++

Projeto de console para a disciplina **Linguagem de Programação I — 2026.2**. Tema escolhido: **23. Controle de estoque**.

O sistema cadastra produtos perecíveis e não perecíveis, pesquisa nomes completos ou seus prefixos, permite alterações e remoções e apresenta um resumo do estoque. Os registros permanecem em um vetor polimórfico durante a execução e são persistidos em um único arquivo ao sair.

## Compilar e executar

Requisitos: compilador com suporte a C++17, como GCC, e GNU Make. Não há bibliotecas externas.

```sh
make
./build/estoque
```

No Linux, instale `g++` e `make` pelo gerenciador de pacotes da sua distribuição. No Windows, é possível usar o terminal do WSL com essas mesmas ferramentas.

Por padrão, o programa lê e grava `dados/estoque.txt`, relativo ao diretório em que for executado. Para escolher outro arquivo:

```sh
./build/estoque /tmp/meu-estoque.txt
```

Para executar os testes automatizados, também é necessário Python 3:

```sh
make test
```

Os 10 testes de integração passaram na verificação do projeto. Eles cobrem os dois tipos de produto, carga e gravação, totais do relatório, busca por prefixo e nomes com espaços, nomes repetidos e seleção de índice, alterações e remoções, cancelamentos, validação de entradas e datas, fim da entrada, escape de texto e preservação de arquivos diante de erros de leitura ou gravação.

## Usar o sistema

| Opção | Operação |
| --- | --- |
| 1 | Inserir um produto perecível ou não perecível |
| 2 | Listar todos os produtos e seus índices |
| 3 | Exibir um produto pesquisando pelo nome ou início do nome |
| 4 | Alterar um produto localizado pela mesma pesquisa |
| 5 | Remover um produto localizado pela mesma pesquisa |
| 6 | Exibir relatório com totais e indicadores do estoque |
| 7 | Salvar os dados e sair |

Os nomes aceitam espaços. Por exemplo, depois de cadastrar `Leite integral` e `Leite em pó`, pesquise `Leite`: o programa apresenta os candidatos com seus índices para você escolher. O método de pesquisa que seleciona um registro é reutilizado nas opções 3, 4 e 5. A pesquisa ignora diferenças entre maiúsculas e minúsculas nas letras ASCII; os caracteres acentuados devem ser escritos como no cadastro. Os índices começam em 0 e acompanham a posição atual no vetor. Ao alterar, informe novamente todos os campos; a data de cadastro é preservada.

Um exemplo de cadastro perecível:

| Campo | Exemplo |
| --- | --- |
| Tipo | Perecível |
| Nome | Leite integral |
| Preço unitário | 6,50 |
| Quantidade | 12 |
| Validade | 2026-12-15 |

A data de cadastro é preenchida automaticamente. Datas usam o formato `AAAA-MM-DD` e são validadas, incluindo anos bissextos. Valores monetários são armazenados como centavos inteiros, evitando erros de arredondamento binário. A quantidade deve ser inteira entre 0 e 100.000.000; o preço deve ficar entre R$ 0,01 e R$ 10.000.000,00, sem separador de milhar na entrada. O nome aceita até 120 bytes e a categoria até 80 bytes; ambos são obrigatórios e não aceitam caracteres de controle. As datas aceitam anos de 0001 a 9999. Totais que excedam a capacidade de `long long` são recusados.

O relatório resume o número de produtos cadastrados, a quantidade de unidades e o valor do estoque, apresenta totais por tipo e conta os cadastros com estoque baixo, definido como até 5 unidades. Ele tem finalidade distinta da listagem de registros.

## Organização e requisitos do trabalho

| Requisito | Implementação |
| --- | --- |
| CRUD com as sete opções solicitadas | Interface em `Aplicacao` e operações de `Estoque` |
| Nome completo ou prefixo, incluindo espaços | Pesquisa de nomes e seleção de índice quando há vários candidatos |
| Reutilização de um método que retorna o índice | `Aplicacao::pesquisar`, chamado para exibir, alterar e remover |
| Modelagem UML | [Diagrama de classes](docs/diagrama-classes.md), [PDF](docs/diagrama-classes.pdf), [SVG](docs/diagrama-classes.svg) e fontes editáveis Mermaid/DOT |
| Cada classe em arquivo `.hpp` separado | Diretório `include/`; implementações em `src/` |
| Objeto principal com quatro atributos privados | `Produto`: nome, preço em centavos, quantidade e data de cadastro |
| Composição com uma classe própria | `Produto` contém um objeto `Data` |
| Uma superclasse e duas subclasses | `Produto`, `ProdutoPerecivel` e `ProdutoNaoPerecivel` |
| Atributos privados nas subclasses | Validade em `ProdutoPerecivel` e categoria em `ProdutoNaoPerecivel` |
| Vetor com polimorfismo | `std::vector<std::unique_ptr<Produto>>` em `Estoque` |
| Sobrescrita de método | As subclasses sobrescrevem `tipo`, `exibir` e `salvar` |
| Classe responsável pelo CRUD | `Estoque`; `main` apenas inicia a aplicação |
| Relatório de resumo | Quantidades e valores agregados, além de estoque baixo |
| Execução até o usuário sair | Laço do menu em `Aplicacao::executar` |
| Persistência polimórfica em um único arquivo | Identificador de tipo em cada registro de `dados/estoque.txt` |
| Carga ao iniciar e gravação ao encerrar | `Estoque::carregar` e `Estoque::salvar` |
| Operações em memória | Inclusões, consultas, alterações e remoções atuam no vetor |

```text
include/      Declarações das classes
src/          Implementações e ponto de entrada
tests/        Testes automatizados
docs/         UML, roteiro da apresentação e declaração de uso de IA
dados/        Local padrão do arquivo de persistência
Makefile      Compilação e execução dos testes
```

`std::unique_ptr` permite armazenar objetos de subclasses no mesmo vetor com propriedade exclusiva e liberação automática de memória. O destrutor virtual de `Produto` garante a destruição correta desses objetos.

## Persistência e cuidados de uso

Os dois tipos de produto são gravados no mesmo arquivo de texto, identificados pelo tipo. Os campos de texto são delimitados com aspas e as datas são escritas em formato ISO. Ao iniciar, os registros são reconstruídos como objetos da subclasse correspondente. Durante a execução, o arquivo não substitui o vetor de objetos.

Ao salvar, o programa prepara um arquivo temporário antes de substituir o arquivo definitivo. Para fazer uma cópia de segurança, encerre o programa pela opção 7 e copie `dados/estoque.txt`. Alterações ainda em memória podem ser perdidas caso o processo seja encerrado à força. Evite abrir duas instâncias usando o mesmo arquivo: a última gravação substitui o conteúdo anterior.

O escopo é o controle de estoque de uma aplicação acadêmica local. Não inclui autenticação, vendas ou acesso simultâneo por vários usuários.

## Entrega e apresentação

- Código-fonte: este repositório.
- Diagrama: [PDF para entrega](docs/diagrama-classes.pdf), [imagem SVG](docs/diagrama-classes.svg) e [documentação UML](docs/diagrama-classes.md).
- Roteiro de até 10 minutos: [docs/apresentacao.md](docs/apresentacao.md).
- Transparência sobre IA: [docs/uso-de-ia.md](docs/uso-de-ia.md).

IA utilizada como apoio na elaboração e revisão do projeto, incluindo código, testes e documentação.
