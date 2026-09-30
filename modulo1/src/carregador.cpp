/**
 * @file carregador.cpp
 * @brief Código que lê os arquivos de filmes e de cinemas. A explicação de uso está em carregador.h.
 */

#include "carregador.h"
#include "texto.h"
#include <fstream>
#include <stdexcept>

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
 * Por que a coluna de gêneros é limpa com aparar() ANTES de comparar com `\N`:
 * ela é a última da linha, então em arquivos salvos no Windows ela chega com
 * um `\r` invisível no final (`\N\r`). Sem a limpeza, a comparação falharia
 * e o filme ficaria com um gênero chamado `\N`.
 *
 * Linhas com defeito (poucas colunas, ano ou duração que não é número) são
 * puladas e contadas em `linhasIgnoradas`, para o programa avisar o usuário
 * em vez de parar no meio.
 *
 * O arquivo é aberto em modo "binário" (`std::ios::binary`). Assim o `\r` do
 * Windows chega igual em qualquer computador e a limpeza com aparar() sempre
 * é necessária e sempre testada.
 */
std::vector<Filme> carregarFilmes(const std::string& caminho, int& linhasIgnoradas) {
    std::vector<Filme> filmes;
    linhasIgnoradas = 0;
    std::ifstream arquivo(caminho, std::ios::binary);
    if (!arquivo.is_open()) {
        throw std::runtime_error("nao consegui abrir o arquivo de filmes: " + caminho);
    }
    std::string linha;

    // Pula a primeira linha, que só tem os nomes das colunas.
    std::getline(arquivo, linha);

    while (std::getline(arquivo, linha)) {
        if (aparar(linha).empty()) {
            continue;
        }

        std::vector<std::string> campos = dividir(linha, '\t');
        if (campos.size() < 9) {
            linhasIgnoradas++;
            continue;
        }
        campos[8] = aparar(campos[8]);

        Filme filme;
        filme.id = campos[0];
        filme.tipo = campos[1];
        filme.titulo = campos[2];
        filme.ano = -1;
        filme.duracao = -1;

        // "\N" significa "sem informação". Nesse caso o valor fica -1.
        bool anoValido = (campos[5] == "\\N") || converterInteiro(campos[5], filme.ano);
        bool duracaoValida = (campos[7] == "\\N") || converterInteiro(campos[7], filme.duracao);
        if (!anoValido || !duracaoValida) {
            linhasIgnoradas++;
            continue;
        }

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
 *
 * Linhas com defeito (poucas colunas, posição ou preço que não é número) são
 * puladas e contadas em `linhasIgnoradas`.
 */
std::vector<Cinema> carregarCinemas(const std::string& caminho, int& linhasIgnoradas) {
    std::vector<Cinema> cinemas;
    linhasIgnoradas = 0;
    std::ifstream arquivo(caminho, std::ios::binary);
    if (!arquivo.is_open()) {
        throw std::runtime_error("nao consegui abrir o arquivo de cinemas: " + caminho);
    }
    std::string linha;

    // Pula a primeira linha, que só tem os nomes das colunas.
    std::getline(arquivo, linha);

    while (std::getline(arquivo, linha)) {
        if (aparar(linha).empty()) {
            continue;
        }

        std::vector<std::string> campos = dividir(linha, ',');
        if (campos.size() < 6) {
            linhasIgnoradas++;
            continue;
        }

        Cinema cinema;
        cinema.id = aparar(campos[0]);
        cinema.nome = aparar(campos[1]);
        try {
            cinema.x = std::stod(aparar(campos[2]));
            cinema.y = std::stod(aparar(campos[3]));
            cinema.preco = std::stod(aparar(campos[4]));
        } catch (const std::logic_error&) {
            linhasIgnoradas++;
            continue;
        }

        // Da 6ª coluna em diante, cada parte é um filme em cartaz. A quantidade varia de cinema para cinema.
        for (size_t i = 5; i < campos.size(); i++) {
            cinema.filmeIds.push_back(aparar(campos[i]));
        }

        cinemas.push_back(cinema);
    }

    return cinemas;
}

/**
 * Como funciona:
 *  1. Confere uma vez se a lista de filmes está em ordem de código.
 *  2. Para cada código de filme de cada cinema, acha a posição do filme:
 *     - lista em ordem: busca binária. Olha o filme do meio; se o código
 *       procurado é menor, descarta a metade de cima; se é maior, a de baixo.
 *       Repete até achar ou acabar (uns 20 passos para 584 mil filmes).
 *     - lista fora de ordem: olha um por um.
 *  3. Código que não existe fica com `-1`.
 */
void resolverFilmesDosCinemas(std::vector<Cinema>& cinemas, const std::vector<Filme>& filmes) {
    bool emOrdem = true;
    for (size_t i = 1; i < filmes.size(); i++) {
        if (filmes[i - 1].id > filmes[i].id) {
            emOrdem = false;
            break;
        }
    }

    for (Cinema& cinema : cinemas) {
        cinema.filmePosicoes.clear();
        for (const std::string& codigo : cinema.filmeIds) {
            int posicao = -1;
            if (emOrdem) {
                int inicio = 0;
                int fim = static_cast<int>(filmes.size()) - 1;
                while (inicio <= fim) {
                    int meio = inicio + (fim - inicio) / 2;
                    if (filmes[meio].id == codigo) {
                        posicao = meio;
                        break;
                    }
                    if (filmes[meio].id < codigo) {
                        inicio = meio + 1;
                    } else {
                        fim = meio - 1;
                    }
                }
            } else {
                for (size_t i = 0; i < filmes.size(); i++) {
                    if (filmes[i].id == codigo) {
                        posicao = static_cast<int>(i);
                        break;
                    }
                }
            }
            cinema.filmePosicoes.push_back(posicao);
        }
    }
}
