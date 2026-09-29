# Módulo 1: Buscas Categóricas e Compostas

Programa em C++ que busca filmes e cinemas por **tipo** (ex: `movie`) e por
**gênero** (ex: `Comedy`). Os filtros podem ser combinados com **E** ou **OU**.

Para uma explicação detalhada de como o programa funciona por dentro, veja
**[FUNCIONAMENTO.md](FUNCIONAMENTO.md)**.

## O que você precisa

- Um compilador C++ com suporte a C++17. Exemplo: `g++` do [MinGW-w64](https://www.mingw-w64.org/) no Windows.

## Como compilar e rodar

Abra o terminal **dentro da pasta `modulo1`** e rode:

```powershell
g++ -O2 -std=c++17 src\*.cpp -o busca.exe
.\busca.exe
```

No Linux/macOS:

```bash
g++ -O2 -std=c++17 src/*.cpp -o busca
./busca
```

> **Importante:** o programa precisa ser executado de dentro da pasta
> `modulo1`, porque ele procura os arquivos em `dados/filmesCrop.txt` e
> `dados/cinemas.txt`. Se rodar de outra pasta, ele abre com 0 filmes carregados.

## Exemplo de uso

```
Filmes carregados: ...
Cinemas carregados: 399
Tempo de carregamento: ... s

1 - Buscar filmes por tipo/genero
2 - Buscar cinemas que exibem filmes por tipo/genero
0 - Sair
Escolha uma opcao: 1
Quantos filtros? 2
Filtro 1 - tipo ou genero (T/G)? T
Valor: movie
Filtro 2 - tipo ou genero (T/G)? G
Valor: Comedy
Combinar filtros com E ou OU? E
```

Resultado: os filmes do tipo `movie` **e** do gênero `Comedy`. Se a mesma
busca for feita de novo, aparece `Veio do cache: sim`.

> Os valores diferenciam maiúsculas de minúsculas: `Comedy` funciona, `comedy` não.

## Estrutura de pastas

```
modulo1/
├── README.md            ← este arquivo (como rodar)
├── FUNCIONAMENTO.md     ← como o programa funciona por dentro
├── Doxyfile             ← configuração para gerar a documentação em HTML
├── dados/
│   ├── filmesCrop.txt   ← ~584 mil filmes (colunas separadas por tabulação)
│   └── cinemas.txt      ← 399 cinemas (colunas separadas por vírgula)
└── src/
    ├── modelos.h        ← quais dados são guardados de um filme e de um cinema
    ├── texto.h/.cpp     ← cortar e limpar textos
    ├── carregador.h/.cpp← ler os arquivos de dados
    ├── indice.h/.cpp    ← índices por tipo/gênero e operações E / OU
    ├── cache.h/.cpp     ← guardar buscas já feitas
    └── main.cpp         ← menu e buscas
```

## Documentação do código

Todo o código-fonte é comentado no padrão **Doxygen**, o formato mais comum
de documentação em C++:

- Cada arquivo começa com `@file` e `@brief`, dizendo para que ele serve.
- Nos arquivos `.h` fica **o que** cada função faz: `@brief`, `@param` (o que
  recebe), `@return` (o que devolve), `@note` e `@warning` (cuidados).
- Nos arquivos `.cpp` fica **como** cada função faz, passo a passo.

Para gerar um site HTML navegável a partir desses comentários, instale o
[Doxygen](https://www.doxygen.nl/) e rode, dentro de `modulo1`:

```bash
doxygen Doxyfile
```

O resultado fica em `docs/html/index.html`.
