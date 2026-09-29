/**
 * @file main.cpp
 * @brief Ponto de entrada do programa: carrega os dados e mostra o menu de buscas no terminal.
 *
 * O que acontece quando o programa roda:
 *  1. Lê os arquivos de filmes e de cinemas (carregador.h).
 *  2. Monta os índices por tipo e por gênero (indice.h).
 *  3. Mostra quanto tempo o carregamento levou.
 *  4. Mostra o menu e espera o usuário escolher uma busca.
 *  5. Em cada busca, mostra o resultado, se ele veio do cache e quanto tempo levou.
 */

#include <iostream>
#include <chrono>
#include "modelos.h"
#include "carregador.h"
#include "indice.h"
#include "cache.h"

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
 * @brief Pergunta ao usuário quantos filtros ele quer e lê cada um.
 *
 * Para cada filtro, o usuário responde:
 *  - `T` para filtrar por tipo. Qualquer outra resposta vira filtro por gênero.
 *  - o valor procurado. Exemplo: `movie` ou `Comedy`.
 *
 * @return Os filtros digitados, na ordem em que foram digitados.
 *
 * @note O valor é lido até o primeiro espaço. Valores com espaço no meio não funcionam.
 */
std::vector<Filtro> lerFiltros() {
    int quantidade;
    std::cout << "Quantos filtros? ";
    std::cin >> quantidade;

    std::vector<Filtro> filtros;
    for (int i = 0; i < quantidade; i++) {
        Filtro filtro;
        std::cout << "Filtro " << (i + 1) << " - tipo ou genero (T/G)? ";
        std::string campo;
        std::cin >> campo;
        filtro.tipoCampo = (campo == "T" || campo == "t") ? "tipo" : "genero";

        std::cout << "Valor: ";
        std::cin >> filtro.valor;

        filtros.push_back(filtro);
    }
    return filtros;
}

/**
 * @brief Pergunta se os filtros devem ser combinados com E ou com OU.
 *
 * Com um filtro só, não há nada a combinar. Nesse caso a pergunta não é feita
 * e a função devolve `E`.
 *
 * @param quantidadeFiltros Quantos filtros o usuário digitou.
 * @return O texto digitado (`E`, `OU`...) ou `E` quando há só um filtro.
 */
std::string operadorEscolhido(int quantidadeFiltros) {
    if (quantidadeFiltros <= 1) {
        return "E";
    }
    std::cout << "Combinar filtros com E ou OU? ";
    std::string operador;
    std::cin >> operador;
    return operador;
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
 * @brief Aplica todos os filtros e devolve as posições dos filmes que passam neles.
 *
 * Como funciona:
 *  1. Pega no índice a lista do primeiro filtro. Ela é o resultado inicial.
 *  2. Para cada filtro seguinte, pega a lista dele e junta com o resultado:
 *     - operador `OU` (ou `ou`): usa uniao(), que soma as listas.
 *     - qualquer outro operador: usa intersecao(), que fica só com o que está nas duas.
 *
 * Exemplo: filtros `tipo=movie` e `genero=Comedy` com `E` devolve
 * os filmes que são do tipo movie E do gênero Comedy.
 *
 * @param filtros      Filtros digitados.
 * @param operador     `E` ou `OU`. Qualquer texto diferente de `OU`/`ou` é tratado como `E`.
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
        } else if (operador == "OU" || operador == "ou") {
            resultado = uniao(resultado, parcial);
        } else {
            resultado = intersecao(resultado, parcial);
        }
    }
    return resultado;
}

/**
 * @brief Mostra na tela o total de filmes encontrados e os 10 primeiros.
 *
 * Só 10 aparecem porque uma busca como `tipo=tvEpisode` encontra mais de
 * 470 mil filmes, e imprimir todos travaria o terminal.
 *
 * @param indices Posições dos filmes encontrados.
 * @param filmes  Lista completa de filmes.
 */
void imprimirFilmes(const std::vector<int>& indices, const std::vector<Filme>& filmes) {
    std::cout << "Total encontrado: " << indices.size() << std::endl;
    size_t limite = indices.size() < 10 ? indices.size() : 10;
    for (size_t i = 0; i < limite; i++) {
        const Filme& filme = filmes[indices[i]];
        std::cout << " - " << filme.titulo << " (" << filme.tipo << ", " << filme.ano << ")" << std::endl;
    }
}

/**
 * @brief Diz se um cinema exibe pelo menos um dos filmes procurados.
 *
 * @param cinema        O cinema a verificar.
 * @param filmeIdsAlvo  Códigos dos filmes procurados. Exemplo: `tt8000001`.
 * @return `true` assim que encontra o primeiro filme em comum, `false` se não houver nenhum.
 *
 * @note Velocidade: compara cada filme do cinema com cada filme procurado.
 *       Se a busca encontrar muitos filmes (centenas de mil), isso fica lento.
 */
bool cinemaExibeAlgumFilme(const Cinema& cinema, const std::vector<std::string>& filmeIdsAlvo) {
    for (const std::string& idExibido : cinema.filmeIds) {
        for (const std::string& idAlvo : filmeIdsAlvo) {
            if (idExibido == idAlvo) {
                return true;
            }
        }
    }
    return false;
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
    std::vector<Filtro> filtros = lerFiltros();
    std::string operador = operadorEscolhido(static_cast<int>(filtros.size()));
    std::string chave = montarChaveCache(filtros, operador, "filme");

    auto inicio = std::chrono::steady_clock::now();

    std::vector<int> resultado;
    bool veioDoCache = cache.buscar(chave, resultado);
    if (!veioDoCache) {
        resultado = aplicarFiltros(filtros, operador, indiceTipo, indiceGenero);
        cache.adicionar(chave, resultado);
    }

    auto fim = std::chrono::steady_clock::now();
    double milissegundos = std::chrono::duration<double, std::milli>(fim - inicio).count();

    imprimirFilmes(resultado, filmes);
    std::cout << "Veio do cache: " << (veioDoCache ? "sim" : "nao") << std::endl;
    std::cout << "Tempo de busca: " << milissegundos << " ms" << std::endl;
}

/**
 * @brief Opção 2 do menu: busca cinemas que exibem filmes de um tipo e/ou gênero.
 *
 * Como funciona:
 *  1. Lê os filtros e acha os filmes que passam neles, do mesmo jeito da opção 1.
 *  2. Transforma as posições desses filmes nos códigos deles (ex: `tt8000001`).
 *  3. Olha cada cinema e verifica se ele exibe pelo menos um desses filmes.
 *  4. Mostra os cinemas encontrados, se veio do cache e o tempo gasto.
 *
 * O cache aqui guarda os FILMES encontrados, não os cinemas. Então, mesmo
 * quando a busca vem do cache, os passos 2 e 3 são refeitos.
 *
 * @param filmes       Lista completa de filmes.
 * @param cinemas      Lista completa de cinemas.
 * @param indiceTipo   Índice por tipo.
 * @param indiceGenero Índice por gênero.
 * @param cache        Cache de buscas. É alterado quando a busca é nova.
 */
void buscarCinemas(const std::vector<Filme>& filmes, const std::vector<Cinema>& cinemas,
                    const IndiceCategoria& indiceTipo, const IndiceCategoria& indiceGenero, CacheConsultas& cache) {
    std::vector<Filtro> filtros = lerFiltros();
    std::string operador = operadorEscolhido(static_cast<int>(filtros.size()));
    std::string chave = montarChaveCache(filtros, operador, "cinema");

    auto inicio = std::chrono::steady_clock::now();

    std::vector<int> indicesFilmes;
    bool veioDoCache = cache.buscar(chave, indicesFilmes);
    if (!veioDoCache) {
        indicesFilmes = aplicarFiltros(filtros, operador, indiceTipo, indiceGenero);
        cache.adicionar(chave, indicesFilmes);
    }

    // Os cinemas guardam códigos de filme (texto), não posições. Por isso a conversão.
    std::vector<std::string> filmeIdsAlvo;
    for (int indice : indicesFilmes) {
        filmeIdsAlvo.push_back(filmes[indice].id);
    }

    std::vector<int> cinemasEncontrados;
    for (size_t i = 0; i < cinemas.size(); i++) {
        if (cinemaExibeAlgumFilme(cinemas[i], filmeIdsAlvo)) {
            cinemasEncontrados.push_back(static_cast<int>(i));
        }
    }

    auto fim = std::chrono::steady_clock::now();
    double milissegundos = std::chrono::duration<double, std::milli>(fim - inicio).count();

    std::cout << "Total de cinemas encontrados: " << cinemasEncontrados.size() << std::endl;
    for (int indice : cinemasEncontrados) {
        const Cinema& cinema = cinemas[indice];
        std::cout << " - " << cinema.nome << " (R$ " << cinema.preco << ")" << std::endl;
    }
    std::cout << "Veio do cache: " << (veioDoCache ? "sim" : "nao") << std::endl;
    std::cout << "Tempo de busca: " << milissegundos << " ms" << std::endl;
}

/**
 * @brief Início do programa.
 *
 * Como funciona:
 *  1. Carrega filmes e cinemas da pasta `dados/`. Por isso o programa precisa
 *     ser executado de dentro da pasta `modulo1`.
 *  2. Monta os índices: cada filme entra no índice do seu tipo e no índice de
 *     cada um dos seus gêneros. Um filme `Action,Short` entra em `Action` e em `Short`.
 *  3. Mostra quantos registros foram carregados e o tempo gasto.
 *  4. Repete o menu até o usuário digitar `0`.
 *
 * @return 0 quando o programa termina normalmente.
 *
 * @warning Se o usuário digitar letras no menu em vez de um número, a leitura
 *          falha e o menu fica se repetindo sem parar.
 */
int main() {
    auto inicioCarregamento = std::chrono::steady_clock::now();

    std::vector<Filme> filmes = carregarFilmes("dados/filmesCrop.txt");
    std::vector<Cinema> cinemas = carregarCinemas("dados/cinemas.txt");

    IndiceCategoria indiceTipo;
    IndiceCategoria indiceGenero;
    for (size_t i = 0; i < filmes.size(); i++) {
        indiceTipo.adicionar(filmes[i].tipo, static_cast<int>(i));
        for (const std::string& genero : filmes[i].generos) {
            indiceGenero.adicionar(genero, static_cast<int>(i));
        }
    }

    auto fimCarregamento = std::chrono::steady_clock::now();
    double segundosCarregamento = std::chrono::duration<double>(fimCarregamento - inicioCarregamento).count();

    std::cout << "Filmes carregados: " << filmes.size() << std::endl;
    std::cout << "Cinemas carregados: " << cinemas.size() << std::endl;
    std::cout << "Tempo de carregamento: " << segundosCarregamento << " s" << std::endl;

    CacheConsultas cache;
    int opcao = -1;

    while (opcao != 0) {
        std::cout << std::endl;
        std::cout << "1 - Buscar filmes por tipo/genero" << std::endl;
        std::cout << "2 - Buscar cinemas que exibem filmes por tipo/genero" << std::endl;
        std::cout << "0 - Sair" << std::endl;
        std::cout << "Escolha uma opcao: ";
        std::cin >> opcao;

        if (opcao == 1) {
            buscarFilmes(filmes, indiceTipo, indiceGenero, cache);
        } else if (opcao == 2) {
            buscarCinemas(filmes, cinemas, indiceTipo, indiceGenero, cache);
        }
    }

    return 0;
}
