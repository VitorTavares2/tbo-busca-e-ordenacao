/**
 * @file carregador.cpp
 * @brief Código que lê os arquivos de filmes e de cinemas. A explicação de uso está em carregador.h.
 */

#include "carregador.h"
#include "texto.h"
#include <fstream>

/**
 * Como funciona, passo a passo:
 *  1. Abre o arquivo e pula a primeira linha (a dos nomes das colunas).
 *  2. Lê UMA linha de cada vez. O arquivo tem uns 48 MB, então ler tudo de
 *     uma vez gastaria muita memória sem necessidade.
 *  3. Corta a linha nas tabulações. Exemplo de linha:
 *     `tt7917526  short  The Pretentious French Film  ...  2017  \N  3  Short`
 *  4. Guarda do filme só o que o programa usa: código, tipo, título, ano,
 *     duração e gêneros. As outras colunas são ignoradas para economizar
 *     memória, porque são quase 584 mil filmes.
 *  5. Coloca o filme no fim da lista.
 *
 * O que é `\N`: é como o arquivo escreve "sem informação". Quando aparece,
 * guardamos `-1` (para números) ou deixamos a lista de gêneros vazia.
 *
 * @warning Detalhe conhecido: a coluna de gêneros é a última da linha, então
 *          ela chega com aquele `\r` invisível do Windows no final (veja texto.cpp).
 *          A comparação com `\N` acontece ANTES da limpeza. Por isso, um filme
 *          sem gênero acaba com um gênero chamado `\N` em vez de lista vazia.
 */
std::vector<Filme> carregarFilmes(const std::string& caminho) {
    std::vector<Filme> filmes;
    std::ifstream arquivo(caminho);
    std::string linha;

    // Pula a primeira linha, que só tem os nomes das colunas.
    std::getline(arquivo, linha);

    while (std::getline(arquivo, linha)) {
        if (linha.empty()) {
            continue;
        }

        std::vector<std::string> campos = dividir(linha, '\t');
        if (campos.size() < 9) {
            continue;
        }

        Filme filme;
        filme.id = campos[0];
        filme.tipo = campos[1];
        filme.titulo = campos[2];

        // "\N" significa "sem informação". Nesse caso guardamos -1.
        filme.ano = (campos[5] == "\\N") ? -1 : std::stoi(campos[5]);
        filme.duracao = (campos[7] == "\\N") ? -1 : std::stoi(campos[7]);

        // "Action,Short" vira dois gêneros: "Action" e "Short".
        if (campos[8] != "\\N") {
            filme.generos = dividir(campos[8], ',');
            for (std::string& genero : filme.generos) {
                genero = aparar(genero);
            }
        }

        filmes.push_back(filme);
    }

    return filmes;
}

/**
 * Como funciona, passo a passo:
 *  1. Abre o arquivo e pula a primeira linha (a dos nomes das colunas).
 *  2. Lê uma linha de cada vez e corta nas vírgulas. Exemplo de linha:
 *     `cc00001, Cineplex Estrela, 347865, 478921, 12.50, tt8000001, tt8000023`
 *  3. As 5 primeiras partes viram código, nome, posição X, posição Y e preço.
 *  4. Todas as partes que sobram são os filmes em cartaz.
 *
 * Por que tanto aparar(): neste arquivo há um espaço depois de cada vírgula
 * (`", Cineplex"`). Sem a limpeza, o nome ficaria `" Cineplex Estrela"`, com
 * espaço na frente, e as comparações de texto falhariam.
 */
std::vector<Cinema> carregarCinemas(const std::string& caminho) {
    std::vector<Cinema> cinemas;
    std::ifstream arquivo(caminho);
    std::string linha;

    // Pula a primeira linha, que só tem os nomes das colunas.
    std::getline(arquivo, linha);

    while (std::getline(arquivo, linha)) {
        if (linha.empty()) {
            continue;
        }

        std::vector<std::string> campos = dividir(linha, ',');
        if (campos.size() < 6) {
            continue;
        }

        Cinema cinema;
        cinema.id = aparar(campos[0]);
        cinema.nome = aparar(campos[1]);
        cinema.x = std::stod(aparar(campos[2]));
        cinema.y = std::stod(aparar(campos[3]));
        cinema.preco = std::stod(aparar(campos[4]));

        // Da 6ª coluna em diante, cada parte é um filme em cartaz. A quantidade varia de cinema para cinema.
        for (size_t i = 5; i < campos.size(); i++) {
            cinema.filmeIds.push_back(aparar(campos[i]));
        }

        cinemas.push_back(cinema);
    }

    return cinemas;
}
