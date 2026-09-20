# Módulo 1 — Buscas Categóricas e Compostas

Implementação em C++ do Módulo 1 do Projeto 1 (Busca e Ordenação): consulta de
filmes e cinemas por tipo e por gênero, com filtros combináveis usando `E`/`OU`.

## Estrutura

- `src/modelos.h` — structs `Filme` e `Cinema`.
- `src/texto.h` / `texto.cpp` — funções de divisão de texto (`dividir`, `aparar`), usadas pra ler os arquivos sem depender de bibliotecas externas.
- `src/carregador.h` / `carregador.cpp` — leitura de `dados/filmesCrop.txt` (TSV) e `dados/cinemas.txt` (CSV).
- `src/indice.h` / `indice.cpp` — `IndiceCategoria` (vetor de índices por tipo/gênero, construído no carregamento) e as funções `intersecao`/`uniao` usadas pra combinar filtros.
- `src/cache.h` / `cache.cpp` — `CacheConsultas`, guarda o resultado das últimas buscas pra não repetir o processamento.
- `src/main.cpp` — menu interativo no console.

## Como funciona

1. No carregamento, cada filme é indexado por `tipo` (ex: `movie`, `tvEpisode`) e por cada `gênero` que possui — isso evita varrer a base inteira a cada busca.
2. O usuário monta uma busca com um ou mais filtros (`tipo` ou `gênero`) e escolhe como combiná-los (`E` = interseção, `OU` = união).
3. O resultado é guardado no cache com uma chave baseada nos filtros/operador — se a mesma busca for repetida, o resultado sai do cache em vez de ser recalculado.
4. A busca de cinemas reaproveita o mesmo filtro de filmes: acha os filmes que atendem aos critérios e depois verifica quais cinemas exibem pelo menos um deles.
5. O tempo de carregamento e o tempo de cada busca são sempre exibidos.

## Compilar e rodar

```powershell
cd "C:\Users\Vitor\Documents\Sistema Notificações SydleOne\major\tbo\projeto1\modulo1"
g++ -O2 -std=c++17 src\*.cpp -o busca.exe
.\busca.exe
```

O programa espera ser executado a partir da pasta `modulo1` (ele lê `dados/filmesCrop.txt` e `dados/cinemas.txt` com caminho relativo).

## Exemplo de uso

```
1 - Buscar filmes por tipo/genero
Quantos filtros? 2
Filtro 1 - tipo ou genero (T/G)? T
Valor: movie
Filtro 2 - tipo ou genero (T/G)? G
Valor: Comedy
Combinar filtros com E ou OU? E
```

Retorna os filmes do tipo `movie` **e** gênero `Comedy`.
