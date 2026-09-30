# Como o programa funciona (Módulos 1 e 2)

Este documento explica, passo a passo, o que o programa faz e o papel de
cada arquivo. Não é preciso saber C++ para acompanhar.

Para saber como compilar e rodar, veja o [README.md](README.md).

---

## 1. O que o programa faz

O programa lê duas bases de dados:

| Arquivo | O que tem | Quantidade |
|---|---|---|
| `dados/filmesCrop.txt` | Filmes, séries, episódios, curtas etc. | 584.121 |
| `dados/cinemas.txt` | Cinemas e os filmes que cada um exibe | 400 |

Depois ele deixa o usuário fazer cinco tipos de busca:

| Opção | Busca | Exemplo |
|---|---|---|
| 1 | Filmes por **tipo e/ou gênero** (Módulo 1) | "filmes do tipo `movie` E do gênero `Comedy`" |
| 2 | Cinemas que exibem filmes por **tipo e/ou gênero** (Módulo 1) | "cinemas que exibem algum `Documentary`" |
| 3 | Filmes por **duração** (Módulo 2) | "filmes de 90 a 120 minutos" |
| 4 | Filmes por **ano de lançamento** (Módulo 2) | "filmes lançados entre 2000 e 2010" |
| 5 | Cinemas que exibem filmes lançados em um **ano ou período** (Módulo 2) | "cinemas que exibem algum filme de 2000" |

Em toda busca o programa mostra:
- quantos resultados encontrou;
- os 10 primeiros (mais uma linha dizendo quantos ficaram de fora);
- se o resultado veio do cache (explicado na seção 6);
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
 │ 2. Montar os índices     │  indice.cpp (tipo e gênero)
 │    e as árvores          │  arvore.cpp (ano e duração)
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

## 3. Índices: como a busca por tipo e gênero fica rápida

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

## 4. Árvores: como a busca por ano e por duração fica rápida (Módulo 2)

Ano e duração são **números**, e a pergunta agora é "de tal número até tal
número". Uma lista por categoria não serve: seriam centenas de listas para
juntar. Para isso existe a **árvore** (`arvore.h`).

### Como a árvore é organizada

Cada "caixinha" da árvore guarda um número (por exemplo, 90 minutos) e a
lista dos filmes que têm esse número. Números **menores** ficam à esquerda
e números **maiores** ficam à direita:

```
                 90
               /    \
            60       120
           /  \     /   \
         45   75  100   150
```

### Como uma busca funciona

Buscar filmes de **95 a 130** minutos:

1. Começa no **90**. Como 90 é menor que 95, tudo à esquerda também é menor.
   Essa metade inteira é ignorada.
2. Vai para o **120**. Está dentro de 95..130, então os filmes dele entram no resultado.
3. Olha a esquerda do 120, o **100**. Também está dentro. Entra.
4. Olha a direita do 120, o **150**. Passou de 130. Não entra.

O programa visitou 4 caixinhas em vez de olhar todas. Com os dados reais, a
árvore de durações tem 372 valores diferentes e 10 "andares", então qualquer
busca visita poucas dezenas de caixinhas.

### Por que a árvore se arruma sozinha

Se os números chegassem em ordem (1, 2, 3, 4...), a árvore viraria uma
"escada" de um lado só e a busca voltaria a ser lenta. Por isso a árvore é
do tipo **AVL**: depois de cada inclusão ela confere se um lado ficou 2
andares mais alto que o outro e, se ficou, gira as caixinhas para
equilibrar. Nos testes, 1000 números em ordem formam uma árvore de 10
andares, não de 1000.

### O que entra na árvore

- Só filmes que **têm** o ano (ou a duração). Quando o arquivo traz `\N`
  (sem informação), o filme **não** entra. Por isso uma busca por ano não
  devolve os 24.818 filmes sem ano, e uma busca por duração não devolve os
  393.409 filmes sem duração.
- Filmes com o mesmo número ficam na mesma caixinha. Exemplo: todos os
  filmes de 90 minutos.

### Em que ordem o resultado aparece

Do **menor** número para o **maior** (filmes de 1885 primeiro, 2026 por
último). Dentro do mesmo número, na ordem do arquivo.

### Buscar um valor só

Digite o valor inicial e aperte Enter no final. O programa entende que o
final é igual ao inicial. Exemplo: ano inicial `2000`, ano final em branco
= só filmes de 2000.

---

## 5. Combinando filtros: E / OU

Quando o usuário digita mais de um filtro (opções 1 e 2), o programa
pergunta como juntar:

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

> Só `E` e `OU` são aceitos (maiúsculas ou minúsculas). Qualquer outra
> resposta mostra `Erro:` e a pergunta é feita de novo.

---

## 6. Cache: não repetir uma busca já feita

Toda busca vira um texto que a descreve (a "chave"). Exemplos:

```
filme|E|tipo=movie|genero=Comedy      (busca por tipo e gênero)
filme|ano=2000..2010                  (filmes lançados de 2000 a 2010)
cinema|ano=2000..2000                 (cinemas com filmes de 2000)
```

- **Primeira vez** que essa busca é feita: o programa calcula o resultado e
  guarda junto com a chave.
- **Da segunda vez em diante**: o programa encontra a chave guardada e
  devolve o resultado sem recalcular. Na tela aparece `Veio do cache: sim`.

A palavra `filme` ou `cinema` no começo da chave impede que uma busca de
filmes e uma busca de cinemas com os mesmos filtros se misturem.

---

## 7. Busca de cinemas

Os cinemas não têm tipo, gênero nem ano. Quem tem são os filmes. Então a
busca de cinemas (opções 2 e 5) é feita em duas etapas:

1. Acha os filmes que passam nos filtros (igual à busca de filmes).
2. Confere quais cinemas exibem **pelo menos um** desses filmes.

```
Filtro: ano 2000
   │
   ▼
Filmes encontrados: posições 40, 512, 9031, ...
   │
   ▼
cc00002 exibe o filme da posição 512 → entra no resultado
cc00005 não exibe nenhum           → fica de fora
```

### O truque que deixa a etapa 2 rápida

Quando o programa abre, ele descobre **uma vez só** em que posição da lista
está cada filme de cada cinema (`resolverFilmesDosCinemas`). Como o arquivo
de filmes já está em ordem de código, isso é feito por **busca binária**:
olha o filme do meio; se o código procurado é menor, descarta a metade de
cima; se é maior, descarta a de baixo. São uns 20 passos para 584 mil filmes.

Na hora da busca, o programa só marca os filmes encontrados numa lista de
"sim/não" e olha, para cada cinema, se algum filme dele está marcado. O
trabalho não cresce com a quantidade de filmes encontrados.

> A parte de achar os cinemas leva menos de 2 milissegundos, mesmo quando a
> busca de filmes encontra 559 mil filmes (ano 1885 a 2026). O que ainda pode
> demorar é juntar filtros grandes com E / OU (veja as limitações, no fim).

---

## 8. Erros de digitação: o que o usuário vê

O programa nunca trava nem repete o menu sem parar. Toda resposta inválida
gera uma linha começando com `Erro:` e a pergunta é feita de novo:

| O que foi digitado | O que aparece |
|---|---|
| `abc` onde se espera um número | `Erro: "abc" nao e um numero inteiro. Digite so numeros, sem letras ou virgula.` |
| Número fora da faixa (ex: opção `9` no menu) | `Erro: digite um numero entre 0 e 5.` |
| Duração mínima maior que a máxima | `Erro: o valor minimo (120) nao pode ser maior que o maximo (90). Digite os dois de novo.` |
| Letra que não é `T` nem `G` | `Erro: digite T (para tipo) ou G (para genero).` |
| Valor de filtro em branco | `Erro: o valor nao pode ficar vazio. ...` |
| Operador que não é `E` nem `OU` | `Erro: digite E ou OU.` |

Outros casos:

- **Busca sem resultado**: além de `Total encontrado: 0`, o programa dá uma
  dica (por exemplo, que maiúsculas e minúsculas importam: `Comedy`, não `comedy`).
- **Arquivo de dados não encontrado**: `Erro: nao consegui abrir o arquivo de filmes: dados/filmesCrop.txt`
  e o programa termina com código 1. Antes ele abria com "0 filmes carregados" sem avisar.
- **Linhas com defeito nos arquivos** (colunas faltando, ano que não é número etc.):
  são puladas, e ao abrir aparece `Aviso: linhas com defeito foram puladas (filmes: X, cinemas: Y).`
  Antes, uma linha assim derrubava o programa inteiro.
- **A entrada acaba no meio** (Ctrl+Z no Windows / Ctrl+D no Linux, ou um arquivo de respostas que termina): o programa encerra com `Entrada encerrada. Ate logo.`

---

## 9. O que cada arquivo faz

| Arquivo | O que faz |
|---|---|
| `src/modelos.h` | Define quais informações são guardadas de um filme (`Filme`) e de um cinema (`Cinema`). |
| `src/texto.h` / `texto.cpp` | Funções de texto: `dividir` corta uma linha nos separadores; `aparar` tira espaços das pontas; `converterInteiro` transforma texto em número só se for número de verdade. |
| `src/entrada.h` / `entrada.cpp` | Faz as perguntas ao usuário e confere as respostas (`lerLinha`, `lerInteiro`). |
| `src/carregador.h` / `carregador.cpp` | Lê os dois arquivos de dados e liga cada filme de cada cinema à sua posição na lista. |
| `src/indice.h` / `indice.cpp` | `IndiceCategoria` guarda as listas por categoria. `intersecao` faz o **E** e `uniao` faz o **OU**. |
| `src/arvore.h` / `arvore.cpp` | `ArvoreNumerica` guarda ano e duração e acha intervalos rapidamente (Módulo 2). |
| `src/cache.h` / `cache.cpp` | `CacheConsultas` guarda buscas já feitas e seus resultados. |
| `src/main.cpp` | Carrega tudo, monta os índices e as árvores, mostra o menu e executa as buscas. |
| `tests/teste_arvore.cpp` | Testes da árvore, um por regra. |
| `tests/rodar_testes.sh` | Roda todos os testes e grava o resultado em `tests/logs/`. |

### Funções do `main.cpp`

| Função | O que faz |
|---|---|
| `lerFiltros` | Pergunta quantos filtros e lê cada um (tipo ou gênero + valor). |
| `operadorEscolhido` | Pergunta se é **E** ou **OU**. Com um filtro só, nem pergunta. |
| `lerIntervalo` | Pergunta o começo e o fim de um intervalo de números. Fim em branco = igual ao começo. |
| `montarChaveCache` / `montarChaveIntervalo` | Montam o texto que identifica a busca no cache. |
| `aplicarFiltros` | Busca cada filtro no índice e combina os resultados com E/OU. |
| `buscarIntervaloComCache` | Procura um intervalo na árvore, usando o cache. |
| `cinemasQueExibem` | Diz quais cinemas exibem pelo menos um filme de uma lista. |
| `imprimirFilmes` / `imprimirCinemas` | Mostram o total e os 10 primeiros resultados. |
| `imprimirRodape` | Mostra "veio do cache" e o tempo da busca. |
| `buscarFilmes` | Opção 1 do menu. |
| `buscarCinemas` | Opção 2 do menu. |
| `buscarPorDuracao` | Opção 3 do menu. |
| `buscarPorAno` | Opção 4 do menu. |
| `buscarCinemasPorAno` | Opção 5 do menu. |
| `main` | Início do programa. |

---

## 10. Formato dos arquivos de dados

### `filmesCrop.txt`: colunas separadas por **tabulação**

```
tconst     titleType  primaryTitle                  ...  startYear  endYear  runtimeMinutes  genres
tt7917526  short      The Pretentious French Film   ...  2017       \N       3               Short
```

- `\N` quer dizer "sem informação". O programa guarda `-1` (ano e duração) ou
  deixa a lista de gêneros vazia. Na tela, `-1` aparece como `?`.
- As colunas `originalTitle`, `isAdult` e `endYear` são lidas, mas não guardadas.
- Dos 584.121 filmes: 24.818 não têm ano, 393.409 não têm duração e 44.681 não têm gênero.

### `cinemas.txt`: colunas separadas por **vírgula**

```
Cinema_ID, Nome_do_Cinema,   Coordenada_X, Coordenada_Y, Preço_Ingresso, Filmes_Em_Exibição
cc00002,   CineArt Palace,   123456,       789012,       10.00,          tt8000034, tt8000078
```

- Da 6ª coluna em diante, cada valor é um filme em exibição. A quantidade varia por cinema.
- As coordenadas ainda não são usadas. Ficam para o Módulo 3 (busca por distância).
- O arquivo tem **400** cinemas, não 399: duas linhas têm o mesmo código `cc00399`
  (`MetroFame Luxe` e `IFCINE Luxe`). O programa carrega as duas como cinemas diferentes.

---

## 11. Como foi testado

O script `tests/rodar_testes.sh` faz seis verificações. O valor "esperado" de
cada uma **não vem do programa**: vem de contas feitas com `awk` direto nos
arquivos de dados, para que um erro do programa não passe despercebido.

| Etapa | O que confere |
|---|---|
| Árvore | 20 verificações das regras da árvore (pontas do intervalo, valores repetidos, ordem, equilíbrio com 1000 números em ordem). |
| Módulo 1 igual ao de antes | 8 buscas do Módulo 1 dão os mesmos totais e os mesmos primeiros resultados da versão anterior (`tests/golden/`). |
| Tabela de casos | 18 buscas de uma consulta só (ano, duração e cinemas por ano), incluindo intervalos vazios e pontas. |
| Erros de digitação | 21 casos: letras, negativos, mínimo maior que máximo, operador inválido, pasta `dados` ausente, arquivo com final de linha do Windows, linhas com defeito. |
| Cascata | Várias buscas na mesma sessão onde uma depende da outra. Exemplo: `2000..2009` + `2010..2019` tem que dar o mesmo total de `2000..2019`. |
| Volume | 552 buscas de filmes e 151 de cinemas numa sessão só, cada uma conferida com o `awk`. O tempo de cada busca fica em `tests/logs/volume_consultas.tsv`. |

---

## 12. Limitações conhecidas

Pontos que funcionam, mas podem ser melhorados. Estão anotados no código
com `@note` ou `@warning`.

| Onde | O que acontece | Efeito |
|---|---|---|
| `intersecao` / `uniao` | Cada item de uma lista é procurado na outra lista inteira. | Filtros com listas grandes ficam lentos. Medido: `tvEpisode` E `Drama` levou cerca de 18 segundos. Como as listas já estão em ordem crescente, dá para percorrer as duas ao mesmo tempo, bem mais rápido. |
| `uniao` | O resultado pode sair fora de ordem crescente. | Nenhum erro hoje, mas impede a melhoria acima em buscas com 3 ou mais filtros. |
| Módulo 2 | Buscas por ano/duração não se combinam com tipo/gênero. | Não dá para pedir "comédias de 2000 a 2010". Combinar filtros livremente é do Módulo 5. |
| Resultado do Módulo 2 | Sai ordenado pelo ano (ou duração), não por outro critério. | Ordenar por outros critérios é do Módulo 5. |
| `CacheConsultas` | A busca no cache olha as chaves uma por uma. | O enunciado pede tempo O(1), o que exigiria uma tabela hash feita à mão. |
| `CacheConsultas` | Nada é apagado do cache. | A memória cresce a cada busca nova. |
| Chave do cache | `movie E Comedy` e `Comedy E movie` geram chaves diferentes. | A mesma busca pode ser calculada duas vezes. |
| `buscarCinemas` / `buscarCinemasPorAno` | O cache guarda os filmes, não os cinemas. | Mesmo vindo do cache, a verificação dos cinemas é refeita (é rápida). |
| Valores de filtro | Maiúsculas e minúsculas importam (`Comedy`, não `comedy`). | Busca sem resultado. O programa avisa com uma dica. |
| Busca de cinemas | Dos 2.009 códigos de filme citados pelos cinemas, 397 não existem na base de filmes. | Esses filmes nunca são encontrados pela busca. Resolver isso é o objetivo do Módulo 4. |
