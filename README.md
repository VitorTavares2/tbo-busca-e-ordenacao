# Projeto 1 — Busca e Ordenação

Disciplina: Busca e Ordenação. Professor: Tadeu Zubaran.

## Introdução

Aplicar os conceitos de busca e ordenação estudados na disciplina para
organizar uma base de dados contendo informações sobre filmes e cinemas.
Na avaliação, as soluções são apresentadas em forma de seminário ao professor
e aos colegas. O tempo de execução (carregamento e busca) deve sempre ser
exibido — é preferível um carregamento mais lento do que uma consulta lenta.

O projeto é dividido em um "cardápio" de módulos opcionais: o aluno escolhe
quais implementar, e a nota leva em conta a qualidade e a dificuldade das
tarefas escolhidas.

## Módulo 1 — Buscas Categóricas e Compostas

- Consultar filmes ou cinemas por tipo (ex: `tvEpisode`).
- Consultar filmes ou cinemas por gênero (ex: `Comedy`, `Documentary`).
- Combinar filtros livremente com operadores lógicos `E`/`OU`.
- Desafios técnicos:
  - Pré-processamento: criar vetores de índices no carregamento para otimizar a consulta.
  - Filtros compostos eficientes: interseção/união de conjuntos em memória, sem varrer a base repetidamente.
  - Cache de consultas: estrutura que guarda o histórico de buscas complexas recentes, retornando resultados repetidos em O(1).

## Módulo 2 — Buscas de Intervalo Numérico

- Consultar filmes por duração específica (entre limites em minutos).
- Consultar filmes ou cinemas por ano de lançamento (ano específico ou intervalo).
- Desafio técnico: indexar os atributos numéricos (Duração, Ano) em uma árvore, pra pesquisa mais rápida que busca sequencial.

## Módulo 3 — Consultas Espaciais

- Consultar cinemas até uma distância máxima de uma coordenada base `(X, Y)` informada pelo usuário.
- Desafio técnico: preparar os dados em memória no carregamento pra evitar comparação exaustiva (filtro de proximidade espacial).

## Módulo 4 — Regra de Inconsistência de IDs

- Ao carregar os dados, se um cinema referenciar um código de filme que não existe, associar ao filme existente com o código **maior mais próximo**.
- Desafio técnico: busca do sucessor via busca binária, encontrando esse código em tempo O(1) (conforme o enunciado).

## Módulo 5 — Funcionalidades e Desafios Extras

- Consultar cinemas por preço (até um limite máximo).
- Exibição parcial (ex: "os 5 filmes mais longos", "os 10 cinemas mais baratos").
- Combinar filtros livremente com `E`/`OU`.
- Ordenação multicritério (ex: Preço → Ano → Duração) com algoritmos de ordenação estáveis.
- Filtros compostos eficientes, evitando varredura exaustiva ou revisita de uma entrada na memória.
- Evitar aliasing: cada entrada de filme/cinema fica em memória uma única vez, ordenada logicamente por vários critérios (não fisicamente duplicada).

## Considerações importantes

- O trabalho deve ser feito em **C ou C++**.
- Proibido usar funções prontas de assuntos da disciplina — algoritmos de ordenação ou hash devem ser implementados do zero.
- Estruturas de dados simples, como `vector` do C++, podem ser usadas.

## Entrega

Duas etapas:

- **Etapa 1** (5 pontos): apresentação do andamento do trabalho ao professor.
- **Etapa 2** (25 pontos): entrega final —
  - Upload do código e apresentação no Google Classroom.
  - Apresentação de seminário para os colegas.
  - Relatório explicando o que foi feito, decisões de projeto e avaliação de desempenho.

A apresentação deve explicar como o trabalho foi feito, quais módulos foram escolhidos, as decisões de projeto, e demonstrar o código funcionando com exemplos práticos, justificando o uso das estruturas (árvores, heaps, hashes, etc.).

## Avaliação

- Clareza e corretude do código e da explicação.
- Qualidade e justificativa técnica dos módulos escolhidos.
- Boas práticas de programação.
- Qualidade da apresentação e participação nas apresentações dos outros grupos.

**Não copiar código — plágio recebe nota 0.**

## Base de dados fornecida

- `cinemas(1).txt` — CSV separado por vírgula, ~399 registros. Colunas: `Cinema_ID, Nome_do_Cinema, Coordenada_X, Coordenada_Y, Preço_Ingresso, Filmes_Em_Exibição` (a última coluna é uma lista variável de IDs de filme `tt########`).
- `filmesCrop.txt` — TSV (separado por tab), ~48MB. Colunas: `tconst, titleType, primaryTitle, originalTitle, isAdult, startYear, endYear, runtimeMinutes, genres` (dataset no estilo IMDb, valores ausentes como `\N`).

## Status

Aguardando definição de quais módulos serão escolhidos e início da implementação.
