# Como o Módulo 1 funciona

Este documento explica, passo a passo, o que o programa faz e o papel de
cada arquivo. Não é preciso saber C++ para acompanhar.

Para saber como compilar e rodar, veja o [README.md](README.md).

---

## 1. O que o programa faz

O programa lê duas bases de dados:

| Arquivo | O que tem | Quantidade |
|---|---|---|
| `dados/filmesCrop.txt` | Filmes, séries, episódios, curtas etc. | ~584 mil |
| `dados/cinemas.txt` | Cinemas e os filmes que cada um exibe | 399 |

Depois ele deixa o usuário fazer dois tipos de busca:

1. **Buscar filmes** por tipo e/ou gênero.
   Exemplo: "filmes do tipo `movie` E do gênero `Comedy`".
2. **Buscar cinemas** que exibem filmes de um tipo e/ou gênero.
   Exemplo: "cinemas que exibem algum filme do gênero `Documentary`".

Em toda busca o programa mostra:
- quantos resultados encontrou;
- se o resultado veio do cache (explicado na seção 4);
- quanto tempo a busca levou.

---

## 2. O caminho completo, do início ao fim

```
 ┌──────────────────────────┐
 │ 1. Ler os arquivos       │  carregador.cpp
 │    (filmes e cinemas)    │
 └────────────┬─────────────┘
              ▼
 ┌──────────────────────────┐
 │ 2. Montar os índices     │  indice.cpp
 │    (por tipo e gênero)   │
 └────────────┬─────────────┘
              ▼
 ┌──────────────────────────┐
 │ 3. Mostrar o tempo de    │  main.cpp
 │    carregamento          │
 └────────────┬─────────────┘
              ▼
 ┌──────────────────────────┐
 │ 4. Menu: o usuário       │  main.cpp
 │    escolhe uma busca     │◄───────┐
 └────────────┬─────────────┘        │
              ▼                      │
 ┌──────────────────────────┐        │
 │ 5. Já fez essa busca?    │ cache  │
 │    Sim → usa o resultado │        │
 │    Não → calcula e guarda│        │
 └────────────┬─────────────┘        │
              ▼                      │
 ┌──────────────────────────┐        │
 │ 6. Mostra resultado      │────────┘
 │    e tempo               │
 └──────────────────────────┘
```

---

## 3. Índices: como a busca fica rápida

Sem índice, para achar "todos os filmes de comédia" o programa teria que
olhar os 584 mil filmes, um por um, **toda vez**.

Com índice, isso é feito **uma vez só**, quando o programa abre. Ele monta
uma lista para cada categoria:

```
Índice por TIPO                     Índice por GÊNERO
────────────────────────────        ────────────────────────────
movie     → 12, 40, 51, ...         Comedy      → 3, 17, 42, ...
tvEpisode → 1, 2, 5, 6, ...         Drama       → 1, 2, 7, ...
short     → 0, 3, 8, ...            Documentary → 5, 9, 88, ...
```

Os números são as **posições** dos filmes na lista de filmes. O filme não
é copiado. Só a posição dele é anotada.

Um filme com mais de um gênero entra em mais de uma lista. Um filme
`Action,Short` aparece em `Action` e em `Short`.

Na hora da busca, o programa só pega a lista pronta.

### Tipos que existem na base

| Tipo | Quantidade |
|---|---|
| `tvEpisode` | 474.707 |
| `short` | 48.454 |
| `movie` | 25.328 |
| `tvSeries` | 12.499 |
| `video` | 11.870 |
| `tvMovie` | 5.209 |
| `tvMiniSeries` | 2.849 |
| `videoGame` | 1.259 |
| `tvSpecial` | 1.148 |
| `tvShort` | 798 |

---

## 4. Combinando filtros: E / OU

Quando o usuário digita mais de um filtro, o programa pergunta como juntar:

**E**: fica só com o que aparece nas **duas** listas.

```
movie  → {1, 4, 7}
Comedy → {4, 7, 9}
movie E Comedy → {4, 7}
```

**OU**: junta tudo das **duas** listas, sem repetir.

```
Comedy → {1, 4}
Drama  → {4, 9}
Comedy OU Drama → {1, 4, 9}
```

Com três filtros ou mais, a combinação vai sendo feita um filtro de cada vez,
da esquerda para a direita, sempre com o mesmo operador.

> Qualquer resposta diferente de `OU`/`ou` é tratada como **E**.

---

## 5. Cache: não repetir uma busca já feita

Toda busca vira um texto que a descreve (a "chave"). Exemplo:

```
filme|E|tipo=movie|genero=Comedy
```

- **Primeira vez** que essa busca é feita: o programa calcula o resultado e
  guarda junto com a chave.
- **Da segunda vez em diante**: o programa encontra a chave guardada e
  devolve o resultado sem recalcular. Na tela aparece `Veio do cache: sim`.

A palavra `filme` ou `cinema` no começo da chave impede que uma busca de
filmes e uma busca de cinemas com os mesmos filtros se misturem.

---

## 6. Busca de cinemas

Os cinemas não têm tipo nem gênero. Quem tem são os filmes. Então a busca
de cinemas é feita em duas etapas:

1. Acha os filmes que passam nos filtros (igual à busca de filmes).
2. Olha cada um dos 399 cinemas e verifica se ele exibe **pelo menos um**
   desses filmes.

```
Filtro: genero=Documentary
   │
   ▼
Filmes encontrados: tt8000034, tt8000102, ...
   │
   ▼
cc00002 exibe tt8000034 → entra no resultado
cc00005 não exibe nenhum → fica de fora
```

---

## 7. O que cada arquivo faz

| Arquivo | O que faz |
|---|---|
| `src/modelos.h` | Define quais informações são guardadas de um filme (`Filme`) e de um cinema (`Cinema`). |
| `src/texto.h` / `texto.cpp` | Duas funções de texto: `dividir` corta uma linha nos separadores; `aparar` tira espaços das pontas. |
| `src/carregador.h` / `carregador.cpp` | Lê os dois arquivos de dados e devolve as listas de filmes e de cinemas. |
| `src/indice.h` / `indice.cpp` | `IndiceCategoria` guarda as listas por categoria. `intersecao` faz o **E** e `uniao` faz o **OU**. |
| `src/cache.h` / `cache.cpp` | `CacheConsultas` guarda buscas já feitas e seus resultados. |
| `src/main.cpp` | Carrega tudo, monta os índices, mostra o menu e executa as buscas. |

### Funções do `main.cpp`

| Função | O que faz |
|---|---|
| `lerFiltros` | Pergunta quantos filtros e lê cada um (tipo ou gênero + valor). |
| `operadorEscolhido` | Pergunta se é **E** ou **OU**. Com um filtro só, nem pergunta. |
| `montarChaveCache` | Monta o texto que identifica a busca no cache. |
| `aplicarFiltros` | Busca cada filtro no índice e combina os resultados com E/OU. |
| `imprimirFilmes` | Mostra o total e os 10 primeiros filmes encontrados. |
| `cinemaExibeAlgumFilme` | Diz se um cinema exibe pelo menos um filme da lista. |
| `buscarFilmes` | Opção 1 do menu. |
| `buscarCinemas` | Opção 2 do menu. |
| `main` | Início do programa. |

---

## 8. Formato dos arquivos de dados

### `filmesCrop.txt`: colunas separadas por **tabulação**

```
tconst     titleType  primaryTitle                  ...  startYear  endYear  runtimeMinutes  genres
tt7917526  short      The Pretentious French Film   ...  2017       \N       3               Short
```

- `\N` quer dizer "sem informação". O programa guarda `-1` nesses casos.
- As colunas `originalTitle`, `isAdult` e `endYear` são lidas, mas não guardadas.

### `cinemas.txt`: colunas separadas por **vírgula**

```
Cinema_ID, Nome_do_Cinema,   Coordenada_X, Coordenada_Y, Preço_Ingresso, Filmes_Em_Exibição
cc00002,   CineArt Palace,   123456,       789012,       10.00,          tt8000034, tt8000078
```

- Da 6ª coluna em diante, cada valor é um filme em exibição. A quantidade varia por cinema.
- As coordenadas ainda não são usadas. Ficam para o Módulo 3 (busca por distância).

---

## 9. Limitações conhecidas

Pontos que funcionam, mas podem ser melhorados. Estão anotados no código
com `@note` ou `@warning`.

| Onde | O que acontece | Efeito |
|---|---|---|
| `intersecao` / `uniao` | Cada item de uma lista é procurado na outra lista inteira. | Filtros com listas grandes (ex: `tvEpisode`, com 474 mil) ficam lentos. Como as listas já estão em ordem crescente, dá para percorrer as duas ao mesmo tempo, bem mais rápido. |
| `uniao` | O resultado pode sair fora de ordem crescente. | Nenhum erro hoje, mas impede a melhoria acima em buscas com 3 ou mais filtros. |
| `CacheConsultas` | A busca no cache olha as chaves uma por uma. | O enunciado pede tempo O(1), o que exige uma tabela hash feita à mão. |
| `CacheConsultas` | Nada é apagado do cache. | A memória cresce a cada busca nova. |
| Chave do cache | `movie E Comedy` e `Comedy E movie` geram chaves diferentes. | A mesma busca pode ser calculada duas vezes. |
| `buscarCinemas` | O cache guarda os filmes, não os cinemas. | Mesmo vindo do cache, a verificação dos cinemas é refeita. |
| `cinemaExibeAlgumFilme` | Compara cada filme do cinema com cada filme encontrado. | Lento quando a busca de filmes encontra muitos resultados. |
| `carregarFilmes` | Filme sem gênero (`\N`) fica com um gênero chamado `\N`. | Por causa do `\r` do fim de linha do Windows, a comparação com `\N` falha. Afeta 44.681 filmes. Uma busca por gênero `\N` devolve todos eles. |
| `carregarFilmes` / `carregarCinemas` | Se o arquivo não for encontrado, a lista volta vazia sem aviso. | O programa abre com "0 filmes carregados". |
| Menu | Digitar letras onde se espera número. | O menu fica se repetindo sem parar. |
| Busca de cinemas | Dos 2.002 códigos de filme citados pelos cinemas, 395 não existem na base de filmes. | Esses filmes nunca são encontrados pela busca. Resolver isso é o objetivo do Módulo 4. |
