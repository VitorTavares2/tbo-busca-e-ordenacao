/**
 * @file carregador.h
 * @brief Lê os arquivos de filmes e de cinemas e coloca tudo na memória.
 *
 * Isso acontece uma vez só, quando o programa abre. A regra do trabalho é
 * esta: tudo bem demorar um pouco para abrir, desde que as buscas depois sejam rápidas.
 */

#ifndef CARREGADOR_H
#define CARREGADOR_H

#include <string>
#include <vector>
#include "modelos.h"

/**
 * @brief Lê o arquivo de filmes e devolve a lista de todos eles.
 *
 * O arquivo é uma tabela em que as colunas são separadas por tabulação.
 * A primeira linha traz só os nomes das colunas e é pulada.
 *
 * @param caminho          Onde está o arquivo, por exemplo `dados/filmesCrop.txt`.
 * @param linhasIgnoradas  Recebe quantas linhas foram puladas por estarem com
 *                         defeito (menos de 9 colunas, ou ano/duração que não
 *                         é número nem `\N`). Linhas totalmente vazias não contam.
 * @return A lista de filmes, na mesma ordem do arquivo. A posição de cada
 *         filme nessa lista (0, 1, 2...) é o "index do filme" que o resto
 *         do programa usa para se referir a ele.
 *
 * @throws std::runtime_error Se o arquivo não puder ser aberto. A mensagem
 *         já diz qual arquivo faltou.
 * @note Um filme sem gênero (`\N` no arquivo) fica com a lista de gêneros vazia.
 */
std::vector<Filme> carregarFilmes(const std::string& caminho, int& linhasIgnoradas);

/**
 * @brief Lê o arquivo de cinemas e devolve a lista de todos eles.
 *
 * O arquivo é uma tabela em que as colunas são separadas por vírgula. As 5
 * primeiras colunas são código, nome, posição X, posição Y e preço. Da 6ª
 * coluna em diante vem a lista de filmes em cartaz, que muda de tamanho de
 * cinema para cinema. A primeira linha (nomes das colunas) é pulada.
 *
 * @param caminho          Onde está o arquivo, por exemplo `dados/cinemas.txt`.
 * @param linhasIgnoradas  Recebe quantas linhas foram puladas por estarem com
 *                         defeito (menos de 6 colunas, ou posição/preço que não é número).
 * @return A lista de cinemas, na mesma ordem do arquivo.
 *
 * @throws std::runtime_error Se o arquivo não puder ser aberto.
 * @warning Um nome de cinema que tivesse vírgula seria cortado no lugar errado.
 *          Hoje nenhum cinema da base tem vírgula no nome.
 */
std::vector<Cinema> carregarCinemas(const std::string& caminho, int& linhasIgnoradas);

/**
 * @brief Descobre, para cada filme em cartaz de cada cinema, em que posição ele está na lista de filmes.
 *
 * Por que isso existe: o cinema guarda o CÓDIGO do filme (`tt8000001`), mas as
 * buscas devolvem POSIÇÕES. Convertendo uma vez só, na abertura, cada busca de
 * cinemas deixa de comparar textos e passa a consultar uma marcação pronta.
 *
 * Preenche `Cinema::filmePosicoes`. Um código que não existe na lista de
 * filmes recebe `-1` (tratar esses códigos é o Módulo 4).
 *
 * @param cinemas Lista de cinemas. É alterada.
 * @param filmes  Lista de filmes, na ordem do arquivo.
 *
 * @note Se a lista de filmes estiver em ordem de código (o arquivo fornecido
 *       está), cada código é achado por busca binária: a cada passo metade
 *       dos filmes é descartada. Se não estiver, a busca é feita um por um
 *       (mais lenta, mas também correta).
 */
void resolverFilmesDosCinemas(std::vector<Cinema>& cinemas, const std::vector<Filme>& filmes);

#endif
