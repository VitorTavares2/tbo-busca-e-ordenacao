# Módulos 1 e 2: Buscas Categóricas, Compostas e por Intervalo

Programa em C++ que busca filmes e cinemas:

- **Módulo 1:** por **tipo** (ex: `movie`) e por **gênero** (ex: `Comedy`). Os filtros podem ser combinados com **E** ou **OU**.
- **Módulo 2:** por **duração** (ex: de 90 a 120 minutos) e por **ano de lançamento** (um ano ou um período). Também dá para achar os **cinemas** que exibem filmes de um ano ou período.

Para uma explicação detalhada de como o programa funciona por dentro, veja
**[FUNCIONAMENTO.md](FUNCIONAMENTO.md)**.

## O que você precisa

- Um compilador C++ com suporte a C++17. Exemplo: `g++` do [MinGW-w64](https://www.mingw-w64.org/) no Windows.

## Como compilar e rodar

Abra o terminal **dentro da pasta `modulo1`** e rode:

```powershell
g++ -O2 -std=c++17 -static src\*.cpp -o busca.exe
.\busca.exe
```

No Linux/macOS:

```bash
g++ -O2 -std=c++17 src/*.cpp -o busca
./busca
```

> **Importante:** o programa precisa ser executado de dentro da pasta
> `modulo1`, porque ele procura os arquivos em `dados/filmesCrop.txt` e
> `dados/cinemas.txt`. Se rodar de outra pasta, ele mostra `Erro:` dizendo
> qual arquivo não achou e fecha.

> **Windows:** o `-static` junta as bibliotecas do compilador dentro do
> `.exe`. Sem ele, em computadores que têm mais de um MinGW instalado, o
> programa pode fechar sozinho ao abrir (erro de memória) por misturar
> bibliotecas de versões diferentes.

## Exemplo de uso

```
Filmes carregados: 584121
Cinemas carregados: 400
Arvore de anos: 134 anos diferentes (altura 9)
Arvore de duracoes: 372 duracoes diferentes (altura 10)
Tempo de carregamento: 0.7 s

1 - Buscar filmes por tipo/genero
2 - Buscar cinemas que exibem filmes por tipo/genero
3 - Buscar filmes por duracao (em minutos)
4 - Buscar filmes por ano de lancamento
5 - Buscar cinemas que exibem filmes lancados em um ano ou periodo
0 - Sair
```

### Exemplo: tipo E gênero (Módulo 1)

```
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

### Exemplo: filmes de 90 a 120 minutos (Módulo 2)

```
Escolha uma opcao: 3
Duracao minima (em minutos): 90
Duracao maxima (Enter = igual a minima): 120
Total encontrado: 8501
 - Alassal Wa Almurr (movie, ano 1964, 90 min)
 ...
   ... e mais 8491 (so os 10 primeiros aparecem)
Veio do cache: nao
Tempo de busca: 0.017 ms
```

### Exemplo: só o ano 2000 (Módulo 2)

Aperte **Enter** no ano final para buscar um ano só:

```
Escolha uma opcao: 4
Ano inicial: 2000
Ano final (Enter = mesmo ano):
Total encontrado: 5192
```

### Exemplo: cinemas com filmes de 1990 a 2010 (Módulo 2)

```
Escolha uma opcao: 5
Ano inicial: 1990
Ano final (Enter = mesmo ano): 2010
Total de cinemas encontrados: 239
```

> Os valores de tipo e gênero diferenciam maiúsculas de minúsculas: `Comedy` funciona, `comedy` não.
> Filmes que o arquivo não informa o ano (ou a duração) não aparecem nas buscas por ano (ou duração).

## Se você digitar algo errado

O programa avisa com uma linha que começa com `Erro:` e pergunta de novo.
Ele não fecha nem repete o menu sem parar. Exemplos:

```
Escolha uma opcao: abc
Erro: "abc" nao e um numero inteiro. Digite so numeros, sem letras ou virgula.
Escolha uma opcao: 9
Erro: digite um numero entre 0 e 5.
```

## Como rodar os testes

Com o `g++` e o `bash` instalados (o Git para Windows já traz o `bash`), dentro da pasta `modulo1`:

```bash
bash tests/rodar_testes.sh
```

O script compila, roda todos os testes e grava o resultado em `tests/logs/`:

- `ultima_execucao.log`: cada verificação com o valor esperado, o obtido e o tempo.
- `volume_consultas.tsv`: as 703 buscas do teste de volume, com esperado, obtido e tempo de cada uma.

Leva cerca de 1 minuto. No final aparece `N/N verificacoes OK`.

## Estrutura de pastas

```
modulo1/
├── README.md            ← este arquivo (como rodar)
├── FUNCIONAMENTO.md     ← como o programa funciona por dentro
├── Doxyfile             ← configuração para gerar a documentação em HTML
├── dados/
│   ├── filmesCrop.txt   ← 584 mil filmes (colunas separadas por tabulação)
│   └── cinemas.txt      ← 400 cinemas (colunas separadas por vírgula)
├── src/
│   ├── modelos.h        ← quais dados são guardados de um filme e de um cinema
│   ├── texto.h/.cpp     ← cortar e limpar textos, converter texto em número
│   ├── entrada.h/.cpp   ← perguntas ao usuário, com conferência das respostas
│   ├── carregador.h/.cpp← ler os arquivos de dados
│   ├── indice.h/.cpp    ← índices por tipo/gênero e operações E / OU
│   ├── arvore.h/.cpp    ← árvore de ano e de duração (Módulo 2)
│   ├── cache.h/.cpp     ← guardar buscas já feitas
│   └── main.cpp         ← menu e buscas
└── tests/
    ├── rodar_testes.sh  ← roda todos os testes
    ├── teste_arvore.cpp ← testes da árvore
    ├── golden/          ← respostas do Módulo 1 antes do Módulo 2 (para comparar)
    └── logs/            ← resultado da última execução dos testes
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

> **Windows:** o Doxygen para Windows não abre arquivos quando o caminho da
> pasta tem acento (como `Notificações`). Se aparecer `could not open file`,
> copie a pasta `modulo1` para um caminho sem acento (ex: `C:	emp\modulo1`) e rode de lá.
