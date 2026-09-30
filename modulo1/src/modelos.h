/**
 * @file modelos.h
 * @brief Define quais informações o programa guarda de cada filme e de cada cinema.
 *
 * Cada filme e cada cinema fica guardado na memória UMA vez só, dentro de uma
 * lista. O resto do programa não copia o filme inteiro. Ele anota só a
 * posição dele na lista (por exemplo, "filme de posição 42").
 * Assim economizamos memória e evitamos ter duas cópias do mesmo filme.
 */

#ifndef MODELOS_H
#define MODELOS_H

#include <string>
#include <vector>

/**
 * @brief Os dados de um filme, lidos de uma linha do arquivo `filmesCrop.txt`.
 *
 * Exemplo. A linha do arquivo:
 * `tt7917526  short  The Pretentious French Film  ...  2017  \N  3  Short`
 * vira: id = `tt7917526`, tipo = `short`, ano = 2017, duracao = 3, generos = `Short`.
 *
 * Quando o arquivo não informa o ano ou a duração, guardamos `-1`.
 * Usamos `-1` e não `0` porque `0` poderia ser confundido com um valor de verdade.
 */
struct Filme {
    std::string id;                    ///< Código único do filme. Exemplo: `tt7917518`.
    std::string tipo;                  ///< Tipo do título. Exemplos: `movie` (filme), `tvEpisode` (episódio de série), `short` (curta).
    std::string titulo;                ///< Nome do filme, como aparece na tela.
    int ano;                           ///< Ano de lançamento. Vale `-1` quando não informado.
    int duracao;                       ///< Duração em minutos. Vale `-1` quando não informada.
    std::vector<std::string> generos;  ///< Lista de gêneros. Exemplo: `Action`, `Comedy`. Fica vazia quando não informada.
};

/**
 * @brief Os dados de um cinema, lidos de uma linha do arquivo `cinemas.txt`.
 *
 * Exemplo. A linha do arquivo:
 * `cc00002, CineArt Palace, 123456, 789012, 10.00, tt8000034, tt8000078`
 * vira: id = `cc00002`, nome = `CineArt Palace`, preco = 10.00,
 * filmeIds = `tt8000034` e `tt8000078`.
 *
 * A lista de filmes em cartaz guarda os códigos exatamente como estão no
 * arquivo. Alguns desses códigos não existem no arquivo de filmes. Corrigir
 * isso é tarefa de outro módulo (Módulo 4), então aqui eles ficam como vieram.
 */
struct Cinema {
    std::string id;                    ///< Código único do cinema. Exemplo: `cc00001`.
    std::string nome;                  ///< Nome do cinema.
    double x;                          ///< Posição no mapa (eixo X). Ainda não usada. Fica para o Módulo 3.
    double y;                          ///< Posição no mapa (eixo Y). Ainda não usada. Fica para o Módulo 3.
    double preco;                      ///< Preço do ingresso, em reais.
    std::vector<std::string> filmeIds; ///< Códigos dos filmes que este cinema está exibindo.
    std::vector<int> filmePosicoes;    ///< Para cada código de `filmeIds`, a posição do filme na lista de filmes. `-1` se o código não existe. Preenchido por resolverFilmesDosCinemas().
};

#endif
