/**
 * @file main.cpp
 * @brief Ponto de entrada do programa: carrega os dados e mostra o menu de buscas no terminal.
 *
 * O que acontece quando o programa roda:
 *  1. Lê os arquivos de filmes e de cinemas (carregador.h).
 *  2. Monta os índices por tipo e por gênero (indice.h) e as árvores de ano e de duração (arvore.h).
 *  3. Mostra quanto tempo o carregamento levou.
 *  4. Mostra o menu e espera o usuário escolher uma busca.
 *  5. Em cada busca, mostra o resultado, se ele veio do cache e quanto tempo levou.
 *
 * Buscas disponíveis:
 *  - Módulo 1: filmes e cinemas por tipo e/ou gênero, combinados com E / OU.
 *  - Módulo 2: filmes por duração, filmes por ano e cinemas por ano de lançamento.
 */

#include <cctype>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include "modelos.h"
#include "carregador.h"
#include "indice.h"
#include "arvore.h"
#include "cache.h"
#include "entrada.h"

/// Quantos itens aparecem na tela em cada busca. O total encontrado sempre aparece por inteiro.
const size_t LIMITE_DE_LINHAS = 10;

/// Maior número de filtros aceito em uma busca do Módulo 1.
const int MAXIMO_DE_FILTROS = 10;

/// Faixa de anos aceita na busca por ano.
const int ANO_MINIMO = 0;
const int ANO_MAXIMO = 9999;

/// Faixa de duração (em minutos) aceita na busca por duração.
const int DURACAO_MINIMA = 0;
const int DURACAO_MAXIMA = 99999;

/**
 * @brief Um filtro digitado pelo usuário.
 *
 * Exemplo: o usuário escolhe "T" e digita `movie`.
 * Isso vira tipoCampo = `tipo` e valor = `movie`.
 */
struct Filtro {
    std::string tipoCampo;  ///< Onde procurar: `tipo` ou `genero`.
    std::string valor;      ///< O que procurar. Exemplo: `movie`, `Comedy`.
};

/**
 * @brief Devolve uma cópia do texto com todas as letras em maiúsculas.
 *
 * Exemplo: `"ou"` vira `"OU"`. Usado só para entender respostas como `t`, `T`, `ou`, `Ou`.
 */
std::string paraMaiusculas(std::string texto) {
    for (char& letra : texto) {
        letra = static_cast<char>(std::toupper(static_cast<unsigned char>(letra)));
    }
    return texto;
}

/**
 * @brief Quantos milissegundos se passaram desde o instante `inicio`.
 */
double milissegundosDesde(std::chrono::steady_clock::time_point inicio) {
    return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - inicio).count();
}

/**
 * @brief Pergunta ao usuário quantos filtros ele quer e lê cada um.
 *
 * Para cada filtro, o usuário responde:
 *  - `T` para filtrar por tipo ou `G` para filtrar por gênero (maiúscula ou minúscula).
 *  - o valor procurado. Exemplo: `movie` ou `Comedy`.
 *
 * Respostas inválidas mostram `Erro:` e a pergunta é feita de novo.
 *
 * @param filtros Recebe os filtros digitados, na ordem em que foram digitados.
 * @return `true` se todos os filtros foram lidos. `false` se a entrada acabou no meio.
 */
bool lerFiltros(std::vector<Filtro>& filtros) {
    int quantidade = 0;
    if (!lerInteiro("Quantos filtros? ", quantidade, 1, MAXIMO_DE_FILTROS)) {
        return false;
    }

    for (int i = 0; i < quantidade; i++) {
        Filtro filtro;
        std::string campo;
        while (true) {
            if (!lerLinha("Filtro " + std::to_string(i + 1) + " - tipo ou genero (T/G)? ", campo)) {
                return false;
            }
            campo = paraMaiusculas(campo);
            if (campo == "T" || campo == "G") {
                break;
            }
            std::cout << "Erro: digite T (para tipo) ou G (para genero)." << std::endl;
        }
        filtro.tipoCampo = (campo == "T") ? "tipo" : "genero";

        while (true) {
            if (!lerLinha("Valor: ", filtro.valor)) {
                return false;
            }
            if (!filtro.valor.empty()) {
                break;
            }
            std::cout << "Erro: o valor nao pode ficar vazio. Exemplo: movie (tipo) ou Comedy (genero)." << std::endl;
        }

        filtros.push_back(filtro);
    }
    return true;
}

/**
 * @brief Pergunta se os filtros devem ser combinados com E ou com OU.
 *
 * Com um filtro só, não há nada a combinar. Nesse caso a pergunta não é feita
 * e o operador vira `E`.
 *
 * @param quantidadeFiltros Quantos filtros o usuário digitou.
 * @param operador          Recebe `E` ou `OU` (sempre em maiúsculas).
 * @return `true` se há um operador. `false` se a entrada acabou.
 */
bool operadorEscolhido(int quantidadeFiltros, std::string& operador) {
    if (quantidadeFiltros <= 1) {
        operador = "E";
        return true;
    }
    while (true) {
        std::string resposta;
        if (!lerLinha("Combinar filtros com E ou OU? ", resposta)) {
            return false;
        }
        resposta = paraMaiusculas(resposta);
        if (resposta == "E" || resposta == "OU") {
            operador = resposta;
            return true;
        }
        std::cout << "Erro: digite E ou OU." << std::endl;
    }
}

/**
 * @brief Monta o texto que descreve uma busca, usado como chave no cache.
 *
 * Exemplo: busca de filmes, operação E, filtros tipo=movie e genero=Comedy
 * gera `filme|E|tipo=movie|genero=Comedy`.
 *
 * @param filtros  Filtros digitados.
 * @param operador `E` ou `OU`.
 * @param alvo     `filme` ou `cinema`. Entra na chave para que uma busca de
 *                 filmes e uma de cinemas com os mesmos filtros não se misturem.
 * @return A chave montada.
 */
std::string montarChaveCache(const std::vector<Filtro>& filtros, const std::string& operador, const std::string& alvo) {
    std::string chave = alvo + "|" + operador;
    for (const Filtro& filtro : filtros) {
        chave += "|" + filtro.tipoCampo + "=" + filtro.valor;
    }
    return chave;
}

/**
 * @brief Monta a chave de cache de uma busca por intervalo numérico.
 *
 * Exemplo: filmes com ano entre 2000 e 2010 gera `filme|ano=2000..2010`.
 *
 * @param alvo    `filme` ou `cinema`.
 * @param campo   `ano` ou `duracao`.
 * @param minimo  Começo do intervalo.
 * @param maximo  Fim do intervalo.
 */
std::string montarChaveIntervalo(const std::string& alvo, const std::string& campo, int minimo, int maximo) {
    return alvo + "|" + campo + "=" + std::to_string(minimo) + ".." + std::to_string(maximo);
}

/**
 * @brief Aplica todos os filtros e devolve as posições dos filmes que passam neles.
 *
 * Como funciona:
 *  1. Pega no índice a lista do primeiro filtro. Ela é o resultado inicial.
 *  2. Para cada filtro seguinte, pega a lista dele e junta com o resultado:
 *     - operador `OU`: usa uniao(), que soma as listas.
 *     - operador `E`: usa intersecao(), que fica só com o que está nas duas.
 *
 * Exemplo: filtros `tipo=movie` e `genero=Comedy` com `E` devolve
 * os filmes que são do tipo movie E do gênero Comedy.
 *
 * @param filtros      Filtros digitados.
 * @param operador     `E` ou `OU`.
 * @param indiceTipo   Índice por tipo.
 * @param indiceGenero Índice por gênero.
 * @return Posições dos filmes encontrados. Volta vazia se não houver filtros.
 */
std::vector<int> aplicarFiltros(const std::vector<Filtro>& filtros, const std::string& operador,
                                 const IndiceCategoria& indiceTipo, const IndiceCategoria& indiceGenero) {
    std::vector<int> resultado;
    for (size_t i = 0; i < filtros.size(); i++) {
        const IndiceCategoria& indice = (filtros[i].tipoCampo == "tipo") ? indiceTipo : indiceGenero;
        std::vector<int> parcial = indice.buscar(filtros[i].valor);

        if (i == 0) {
            resultado = parcial;
        } else if (operador == "OU") {
            resultado = uniao(resultado, parcial);
        } else {
            resultado = intersecao(resultado, parcial);
        }
    }
    return resultado;
}

/**
 * @brief Escreve um número, ou `?` quando o arquivo não trazia essa informação (valor `-1`).
 */
std::string numeroOuInterrogacao(int numero) {
    return numero < 0 ? "?" : std::to_string(numero);
}

/**
 * @brief Mostra na tela o total de filmes encontrados e os 10 primeiros.
 *
 * Só 10 aparecem porque uma busca como `tipo=tvEpisode` encontra mais de
 * 470 mil filmes, e imprimir todos travaria o terminal. Quando há mais, uma
 * linha final diz quantos ficaram de fora.
 *
 * Cada linha mostra: título, tipo, ano e duração. Ano ou duração que o arquivo
 * não informa aparecem como `?`.
 *
 * @param indices Posições dos filmes encontrados.
 * @param filmes  Lista completa de filmes.
 * @param dica    Frase mostrada quando nada é encontrado, para ajudar o usuário a achar o erro.
 */
void imprimirFilmes(const std::vector<int>& indices, const std::vector<Filme>& filmes, const std::string& dica) {
    std::cout << "Total encontrado: " << indices.size() << std::endl;
    if (indices.empty()) {
        std::cout << dica << std::endl;
        return;
    }
    size_t limite = indices.size() < LIMITE_DE_LINHAS ? indices.size() : LIMITE_DE_LINHAS;
    for (size_t i = 0; i < limite; i++) {
        const Filme& filme = filmes[indices[i]];
        std::cout << " - " << filme.titulo << " (" << filme.tipo << ", ano " << numeroOuInterrogacao(filme.ano)
                  << ", " << numeroOuInterrogacao(filme.duracao) << " min)" << std::endl;
    }
    if (indices.size() > limite) {
        std::cout << "   ... e mais " << (indices.size() - limite) << " (so os " << LIMITE_DE_LINHAS << " primeiros aparecem)" << std::endl;
    }
}

/**
 * @brief Descobre quais cinemas exibem pelo menos um dos filmes procurados.
 *
 * Como funciona:
 *  1. Cria uma lista de marcações com um espaço para cada filme (0 = não procurado).
 *  2. Marca com 1 cada filme procurado.
 *  3. Para cada cinema, olha a posição de cada filme que ele exibe
 *     (Cinema::filmePosicoes) e confere a marcação. Parou no primeiro que está marcado.
 *
 * Assim, o trabalho não cresce com a quantidade de filmes procurados: são uma
 * passada para marcar e uma passada pelos cinemas.
 *
 * @param indicesFilmes Posições dos filmes procurados.
 * @param totalFilmes   Quantos filmes existem no total.
 * @param cinemas       Lista completa de cinemas (com filmePosicoes já preenchido).
 * @return Posições dos cinemas encontrados, na ordem do arquivo.
 */
std::vector<int> cinemasQueExibem(const std::vector<int>& indicesFilmes, size_t totalFilmes, const std::vector<Cinema>& cinemas) {
    std::vector<char> procurado(totalFilmes, 0);
    for (int indice : indicesFilmes) {
        procurado[indice] = 1;
    }

    std::vector<int> encontrados;
    for (size_t i = 0; i < cinemas.size(); i++) {
        for (int posicao : cinemas[i].filmePosicoes) {
            if (posicao >= 0 && procurado[posicao]) {
                encontrados.push_back(static_cast<int>(i));
                break;
            }
        }
    }
    return encontrados;
}

/**
 * @brief Mostra o total de cinemas encontrados e os 10 primeiros, com o preço do ingresso.
 *
 * @param indices Posições dos cinemas encontrados.
 * @param cinemas Lista completa de cinemas.
 * @param dica    Frase mostrada quando nada é encontrado.
 */
void imprimirCinemas(const std::vector<int>& indices, const std::vector<Cinema>& cinemas, const std::string& dica) {
    std::cout << "Total de cinemas encontrados: " << indices.size() << std::endl;
    if (indices.empty()) {
        std::cout << dica << std::endl;
        return;
    }
    size_t limite = indices.size() < LIMITE_DE_LINHAS ? indices.size() : LIMITE_DE_LINHAS;
    for (size_t i = 0; i < limite; i++) {
        const Cinema& cinema = cinemas[indices[i]];
        std::cout << " - " << cinema.nome << " (R$ " << std::fixed << std::setprecision(2) << cinema.preco << ")" << std::endl;
        std::cout.unsetf(std::ios::fixed);
    }
    if (indices.size() > limite) {
        std::cout << "   ... e mais " << (indices.size() - limite) << " (so os " << LIMITE_DE_LINHAS << " primeiros aparecem)" << std::endl;
    }
}

/**
 * @brief Mostra as duas últimas linhas de toda busca: se veio do cache e quanto tempo levou.
 *
 * @param veioDoCache `true` se o resultado já estava guardado de uma busca igual.
 * @param milissegundos Tempo da busca, sem contar o tempo de digitação.
 */
void imprimirRodape(bool veioDoCache, double milissegundos) {
    std::cout << "Veio do cache: " << (veioDoCache ? "sim" : "nao") << std::endl;
    std::cout << "Tempo de busca: " << std::fixed << std::setprecision(3) << milissegundos << " ms" << std::endl;
    std::cout.unsetf(std::ios::fixed);
}

/**
 * @brief Opção 1 do menu: busca filmes por tipo e/ou gênero.
 *
 * Como funciona:
 *  1. Lê os filtros e o operador (E / OU).
 *  2. Monta a chave e procura no cache.
 *  3. Se não estava no cache, aplica os filtros e guarda o resultado no cache.
 *  4. Mostra o resultado, se veio do cache e o tempo gasto.
 *
 * O tempo medido começa DEPOIS que o usuário termina de digitar. Assim o
 * tempo de digitação não entra na conta.
 *
 * @param filmes       Lista completa de filmes.
 * @param indiceTipo   Índice por tipo.
 * @param indiceGenero Índice por gênero.
 * @param cache        Cache de buscas. É alterado quando a busca é nova.
 */
void buscarFilmes(const std::vector<Filme>& filmes, const IndiceCategoria& indiceTipo,
                   const IndiceCategoria& indiceGenero, CacheConsultas& cache) {
    std::vector<Filtro> filtros;
    std::string operador;
    if (!lerFiltros(filtros) || !operadorEscolhido(static_cast<int>(filtros.size()), operador)) {
        return;
    }
    std::string chave = montarChaveCache(filtros, operador, "filme");

    auto inicio = std::chrono::steady_clock::now();

    std::vector<int> resultado;
    bool veioDoCache = cache.buscar(chave, resultado);
    if (!veioDoCache) {
        resultado = aplicarFiltros(filtros, operador, indiceTipo, indiceGenero);
        cache.adicionar(chave, resultado);
    }

    double milissegundos = milissegundosDesde(inicio);

    imprimirFilmes(resultado, filmes, "Nenhum resultado. Dica: maiusculas e minusculas importam (use Comedy, nao comedy).");
    imprimirRodape(veioDoCache, milissegundos);
}

/**
 * @brief Opção 2 do menu: busca cinemas que exibem filmes de um tipo e/ou gênero.
 *
 * Como funciona:
 *  1. Lê os filtros e acha os filmes que passam neles, do mesmo jeito da opção 1.
 *  2. Olha cada cinema e verifica se ele exibe pelo menos um desses filmes (cinemasQueExibem()).
 *  3. Mostra os cinemas encontrados, se veio do cache e o tempo gasto.
 *
 * O cache aqui guarda os FILMES encontrados, não os cinemas. Então, mesmo
 * quando a busca vem do cache, o passo 2 é refeito (ele é rápido).
 *
 * @param filmes       Lista completa de filmes.
 * @param cinemas      Lista completa de cinemas.
 * @param indiceTipo   Índice por tipo.
 * @param indiceGenero Índice por gênero.
 * @param cache        Cache de buscas. É alterado quando a busca é nova.
 */
void buscarCinemas(const std::vector<Filme>& filmes, const std::vector<Cinema>& cinemas,
                    const IndiceCategoria& indiceTipo, const IndiceCategoria& indiceGenero, CacheConsultas& cache) {
    std::vector<Filtro> filtros;
    std::string operador;
    if (!lerFiltros(filtros) || !operadorEscolhido(static_cast<int>(filtros.size()), operador)) {
        return;
    }
    std::string chave = montarChaveCache(filtros, operador, "cinema");

    auto inicio = std::chrono::steady_clock::now();

    std::vector<int> indicesFilmes;
    bool veioDoCache = cache.buscar(chave, indicesFilmes);
    if (!veioDoCache) {
        indicesFilmes = aplicarFiltros(filtros, operador, indiceTipo, indiceGenero);
        cache.adicionar(chave, indicesFilmes);
    }

    std::vector<int> cinemasEncontrados = cinemasQueExibem(indicesFilmes, filmes.size(), cinemas);

    double milissegundos = milissegundosDesde(inicio);

    imprimirCinemas(cinemasEncontrados, cinemas, "Nenhum cinema exibe filmes com esses filtros. Dica: maiusculas e minusculas importam (use Comedy, nao comedy).");
    imprimirRodape(veioDoCache, milissegundos);
}

/**
 * @brief Pergunta o começo e o fim de um intervalo de números.
 *
 * O fim pode ficar em branco (só Enter). Nesse caso ele é igual ao começo,
 * o que serve para buscar um valor exato (ex: só o ano 2000).
 * Se o começo for maior que o fim, mostra `Erro:` e pergunta os dois de novo.
 *
 * @param perguntaMinimo Texto da pergunta do começo. Exemplo: `"Ano inicial: "`.
 * @param perguntaMaximo Texto da pergunta do fim.
 * @param limiteInferior Menor número aceito.
 * @param limiteSuperior Maior número aceito.
 * @param minimo         Recebe o começo do intervalo.
 * @param maximo         Recebe o fim do intervalo.
 * @return `true` se o intervalo foi lido. `false` se a entrada acabou.
 */
bool lerIntervalo(const std::string& perguntaMinimo, const std::string& perguntaMaximo,
                  int limiteInferior, int limiteSuperior, int& minimo, int& maximo) {
    while (true) {
        if (!lerInteiro(perguntaMinimo, minimo, limiteInferior, limiteSuperior)) {
            return false;
        }
        if (!lerInteiro(perguntaMaximo, maximo, limiteInferior, limiteSuperior, &minimo)) {
            return false;
        }
        if (minimo <= maximo) {
            return true;
        }
        std::cout << "Erro: o valor minimo (" << minimo << ") nao pode ser maior que o maximo (" << maximo
                  << "). Digite os dois de novo." << std::endl;
    }
}

/**
 * @brief Procura na árvore (com ajuda do cache) os filmes cujo número está no intervalo.
 *
 * @param arvore        Árvore do campo (ano ou duração).
 * @param chave         Chave de cache desta busca (veja montarChaveIntervalo()).
 * @param minimo        Começo do intervalo.
 * @param maximo        Fim do intervalo.
 * @param cache         Cache de buscas. É alterado quando a busca é nova.
 * @param veioDoCache   Recebe `true` se o resultado já estava guardado.
 * @return Posições dos filmes encontrados, do menor número para o maior.
 */
std::vector<int> buscarIntervaloComCache(const ArvoreNumerica& arvore, const std::string& chave, int minimo, int maximo,
                                          CacheConsultas& cache, bool& veioDoCache) {
    std::vector<int> resultado;
    veioDoCache = cache.buscar(chave, resultado);
    if (!veioDoCache) {
        resultado = arvore.buscarIntervalo(minimo, maximo);
        cache.adicionar(chave, resultado);
    }
    return resultado;
}

/**
 * @brief Opção 3 do menu: busca filmes com duração entre dois valores (em minutos).
 *
 * Exemplo: mínimo 90 e máximo 120 devolve os filmes de 90 a 120 minutos, as duas pontas incluídas.
 * Filmes sem duração informada nunca aparecem aqui.
 *
 * @param filmes          Lista completa de filmes.
 * @param arvoreDuracao   Árvore de durações.
 * @param cache           Cache de buscas.
 */
void buscarPorDuracao(const std::vector<Filme>& filmes, const ArvoreNumerica& arvoreDuracao, CacheConsultas& cache) {
    int minimo = 0;
    int maximo = 0;
    if (!lerIntervalo("Duracao minima (em minutos): ", "Duracao maxima (Enter = igual a minima): ",
                      DURACAO_MINIMA, DURACAO_MAXIMA, minimo, maximo)) {
        return;
    }

    auto inicio = std::chrono::steady_clock::now();
    bool veioDoCache = false;
    std::vector<int> resultado = buscarIntervaloComCache(arvoreDuracao, montarChaveIntervalo("filme", "duracao", minimo, maximo),
                                                         minimo, maximo, cache, veioDoCache);
    double milissegundos = milissegundosDesde(inicio);

    imprimirFilmes(resultado, filmes, "Nenhum filme com essa duracao. Dica: filmes sem duracao informada no arquivo nao entram na busca.");
    imprimirRodape(veioDoCache, milissegundos);
}

/**
 * @brief Opção 4 do menu: busca filmes lançados em um ano ou em um intervalo de anos.
 *
 * Para um ano só, digite o ano inicial e aperte Enter no ano final.
 * Filmes sem ano informado nunca aparecem aqui.
 *
 * @param filmes      Lista completa de filmes.
 * @param arvoreAno   Árvore de anos.
 * @param cache       Cache de buscas.
 */
void buscarPorAno(const std::vector<Filme>& filmes, const ArvoreNumerica& arvoreAno, CacheConsultas& cache) {
    int minimo = 0;
    int maximo = 0;
    if (!lerIntervalo("Ano inicial: ", "Ano final (Enter = mesmo ano): ", ANO_MINIMO, ANO_MAXIMO, minimo, maximo)) {
        return;
    }

    auto inicio = std::chrono::steady_clock::now();
    bool veioDoCache = false;
    std::vector<int> resultado = buscarIntervaloComCache(arvoreAno, montarChaveIntervalo("filme", "ano", minimo, maximo),
                                                         minimo, maximo, cache, veioDoCache);
    double milissegundos = milissegundosDesde(inicio);

    imprimirFilmes(resultado, filmes, "Nenhum filme nesse periodo. Dica: a base tem filmes de 1885 a 2026.");
    imprimirRodape(veioDoCache, milissegundos);
}

/**
 * @brief Opção 5 do menu: busca cinemas que exibem algum filme lançado em um ano ou intervalo de anos.
 *
 * Como funciona: acha os filmes do período na árvore de anos e depois
 * confere quais cinemas exibem pelo menos um deles (cinemasQueExibem()).
 *
 * @param filmes      Lista completa de filmes.
 * @param cinemas     Lista completa de cinemas.
 * @param arvoreAno   Árvore de anos.
 * @param cache       Cache de buscas. Guarda os FILMES do período, como na opção 2.
 */
void buscarCinemasPorAno(const std::vector<Filme>& filmes, const std::vector<Cinema>& cinemas,
                          const ArvoreNumerica& arvoreAno, CacheConsultas& cache) {
    int minimo = 0;
    int maximo = 0;
    if (!lerIntervalo("Ano inicial: ", "Ano final (Enter = mesmo ano): ", ANO_MINIMO, ANO_MAXIMO, minimo, maximo)) {
        return;
    }

    auto inicio = std::chrono::steady_clock::now();
    bool veioDoCache = false;
    std::vector<int> indicesFilmes = buscarIntervaloComCache(arvoreAno, montarChaveIntervalo("cinema", "ano", minimo, maximo),
                                                             minimo, maximo, cache, veioDoCache);
    std::vector<int> cinemasEncontrados = cinemasQueExibem(indicesFilmes, filmes.size(), cinemas);
    double milissegundos = milissegundosDesde(inicio);

    imprimirCinemas(cinemasEncontrados, cinemas, "Nenhum cinema exibe filmes desse periodo.");
    imprimirRodape(veioDoCache, milissegundos);
}

/**
 * @brief Início do programa.
 *
 * Como funciona:
 *  1. Carrega filmes e cinemas da pasta `dados/`. Por isso o programa precisa
 *     ser executado de dentro da pasta `modulo1`. Se um arquivo não abrir,
 *     mostra `Erro:` dizendo qual e termina com código 1.
 *  2. Monta os índices: cada filme entra no índice do seu tipo e no índice de
 *     cada um dos seus gêneros. Um filme `Action,Short` entra em `Action` e em `Short`.
 *     Filmes com ano e/ou duração informados entram também nas árvores de ano e de duração.
 *  3. Mostra quantos registros foram carregados, avisos e o tempo gasto.
 *  4. Repete o menu até o usuário digitar `0` (ou a entrada acabar).
 *
 * @return 0 quando o programa termina normalmente. 1 se não conseguiu carregar os dados.
 */
int main() {
    auto inicioCarregamento = std::chrono::steady_clock::now();

    std::vector<Filme> filmes;
    std::vector<Cinema> cinemas;
    int filmesIgnorados = 0;
    int cinemasIgnorados = 0;
    try {
        filmes = carregarFilmes("dados/filmesCrop.txt", filmesIgnorados);
        cinemas = carregarCinemas("dados/cinemas.txt", cinemasIgnorados);
    } catch (const std::runtime_error& problema) {
        std::cout << "Erro: " << problema.what() << std::endl;
        std::cout << "Dica: execute o programa de dentro da pasta modulo1, onde existe a pasta dados." << std::endl;
        return 1;
    }
    resolverFilmesDosCinemas(cinemas, filmes);

    IndiceCategoria indiceTipo;
    IndiceCategoria indiceGenero;
    ArvoreNumerica arvoreAno;
    ArvoreNumerica arvoreDuracao;
    for (size_t i = 0; i < filmes.size(); i++) {
        int posicao = static_cast<int>(i);
        indiceTipo.adicionar(filmes[i].tipo, posicao);
        for (const std::string& genero : filmes[i].generos) {
            indiceGenero.adicionar(genero, posicao);
        }
        if (filmes[i].ano >= 0) {
            arvoreAno.adicionar(filmes[i].ano, posicao);
        }
        if (filmes[i].duracao >= 0) {
            arvoreDuracao.adicionar(filmes[i].duracao, posicao);
        }
    }

    double segundosCarregamento = milissegundosDesde(inicioCarregamento) / 1000.0;

    std::cout << "Filmes carregados: " << filmes.size() << std::endl;
    std::cout << "Cinemas carregados: " << cinemas.size() << std::endl;
    if (filmesIgnorados > 0 || cinemasIgnorados > 0) {
        std::cout << "Aviso: linhas com defeito foram puladas (filmes: " << filmesIgnorados
                  << ", cinemas: " << cinemasIgnorados << ")." << std::endl;
    }
    std::cout << "Arvore de anos: " << arvoreAno.quantidadeValores() << " anos diferentes (altura " << arvoreAno.altura() << ")" << std::endl;
    std::cout << "Arvore de duracoes: " << arvoreDuracao.quantidadeValores() << " duracoes diferentes (altura " << arvoreDuracao.altura() << ")" << std::endl;
    std::cout << "Tempo de carregamento: " << segundosCarregamento << " s" << std::endl;

    CacheConsultas cache;
    int opcao = -1;

    while (opcao != 0) {
        std::cout << std::endl;
        std::cout << "1 - Buscar filmes por tipo/genero" << std::endl;
        std::cout << "2 - Buscar cinemas que exibem filmes por tipo/genero" << std::endl;
        std::cout << "3 - Buscar filmes por duracao (em minutos)" << std::endl;
        std::cout << "4 - Buscar filmes por ano de lancamento" << std::endl;
        std::cout << "5 - Buscar cinemas que exibem filmes lancados em um ano ou periodo" << std::endl;
        std::cout << "0 - Sair" << std::endl;
        if (!lerInteiro("Escolha uma opcao: ", opcao, 0, 5)) {
            std::cout << "Entrada encerrada. Ate logo." << std::endl;
            break;
        }

        if (opcao == 1) {
            buscarFilmes(filmes, indiceTipo, indiceGenero, cache);
        } else if (opcao == 2) {
            buscarCinemas(filmes, cinemas, indiceTipo, indiceGenero, cache);
        } else if (opcao == 3) {
            buscarPorDuracao(filmes, arvoreDuracao, cache);
        } else if (opcao == 4) {
            buscarPorAno(filmes, arvoreAno, cache);
        } else if (opcao == 5) {
            buscarCinemasPorAno(filmes, cinemas, arvoreAno, cache);
        }
    }

    return 0;
}
