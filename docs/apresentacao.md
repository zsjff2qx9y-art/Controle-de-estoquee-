# Roteiro da apresentação — até 10 minutos

## 0:00–1:00 — Problema e tema

Apresente o tema 23, controle de estoque. Explique que o sistema guarda produtos perecíveis e não perecíveis, com nome, preço, quantidade e data de cadastro. O primeiro tipo tem validade; o segundo tem categoria.

Informe o uso de IA conforme [a declaração](uso-de-ia.md): ela auxiliou a modelagem, a implementação, os testes e a documentação. Explique o código com suas próprias palavras e descreva as verificações que você efetivamente realizou.

## 1:00–2:30 — Modelagem

Abra o [diagrama de classes](diagrama-classes.md). Mostre:

- `Produto`, a classe abstrata com quatro atributos privados.
- `Data`, usada por composição dentro de `Produto`.
- As duas subclasses, seus atributos privados e os métodos sobrescritos.
- `Estoque`, responsável por armazenar objetos e executar o CRUD.
- `Aplicacao`, responsável pela conversa com o usuário.

Explique a diferença: herança representa “é um produto”; composição representa “tem uma data”.

## 2:30–5:30 — Demonstração do CRUD

Compile antes da apresentação e inicie com um arquivo separado, para não alterar o estoque de uso habitual:

```sh
make
./build/estoque /tmp/estoque-apresentacao.txt
```

Se esse arquivo já existir, ele será carregado; use outro nome para uma demonstração vazia.

1. Cadastre um perecível chamado `Leite integral`, com preço `6,50`, quantidade `12` e uma validade futura.
2. Cadastre outro produto chamado `Leite em pó`, com preço `18,90` e quantidade `3`.
3. Cadastre um não perecível chamado `Copo de vidro`, com preço `8,00`, quantidade `8` e categoria `Utensílios`.
4. Liste todos os produtos e observe os campos específicos de cada tipo.
5. Pesquise `Leite`. Mostre os candidatos e selecione pelo índice exibido.
6. Altere a quantidade de um produto e pesquise novamente para conferir.
7. Remova um produto usando a mesma pesquisa.

Os índices acompanham as posições atuais do vetor e podem mudar após uma remoção. Escolha sempre um dos índices apresentados pela pesquisa atual.

## 5:30–6:30 — Relatório

Abra a opção 6. Mostre a quantidade de produtos cadastrados, as unidades e o valor total. Aponte os totais por tipo e o indicador de estoque baixo, até 5 unidades.

Explique que a listagem apresenta registros individuais, enquanto o relatório resume os dados para apoiar a conferência do estoque.

## 6:30–7:30 — Persistência

Saia pela opção 7 e abra novamente com o mesmo caminho. Mostre que os produtos voltam com os tipos e campos específicos preservados.

Explique que as operações do menu atuam no vetor. O arquivo é lido no início e gravado ao encerrar. Um identificador em cada registro permite reconstruir a subclasse correta a partir de um único arquivo.

## 7:30–9:00 — Polimorfismo e código

Abra os cabeçalhos e a implementação do estoque. Explique:

- O vetor armazena `unique_ptr<Produto>` e pode conter os dois tipos concretos.
- Os métodos virtuais `tipo`, `exibir` e `salvar` usam a implementação de cada subclasse.
- O destrutor virtual permite a destruição correta por um ponteiro da superclasse.
- A pesquisa é reutilizada para exibir, alterar e remover e retorna o índice selecionado.
- Os preços são guardados em centavos inteiros.

## 9:00–10:00 — Verificação e perguntas

Mostre o comando `make test` e descreva somente os casos efetivamente cobertos pelos testes do repositório. Demonstre uma entrada inválida, como quantidade negativa ou data inexistente, para mostrar a validação.

Prepare-se para explicar por que o relatório é diferente da listagem, como nomes duplicados são tratados e como os dados polimórficos cabem em um único arquivo.
