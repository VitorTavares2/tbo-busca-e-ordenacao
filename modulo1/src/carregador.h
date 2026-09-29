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
 * @param caminho Onde está o arquivo, por exemplo `dados/filmesCrop.txt`.
 * @return A lista de filmes, na mesma ordem do arquivo. A posição de cada
 *         filme nessa lista (0, 1, 2...) é o "número do filme" que o resto
 *         do programa usa para se referir a ele.
 *
 * @warning Se o arquivo não for encontrado, a lista volta vazia e nenhum erro aparece.
 * @warning Se o ano ou a duração tiverem algo que não seja número (nem `\N`),
 *          o programa para com erro.
 * @note Linhas vazias ou incompletas (menos de 9 colunas) são ignoradas.
 */
std::vector<Filme> carregarFilmes(const std::string& caminho);

/**
 * @brief Lê o arquivo de cinemas e devolve a lista de todos eles.
 *
 * O arquivo é uma tabela em que as colunas são separadas por vírgula. As 5
 * primeiras colunas são código, nome, posição X, posição Y e preço. Da 6ª
 * coluna em diante vem a lista de filmes em cartaz, que muda de tamanho de
 * cinema para cinema. A primeira linha (nomes das colunas) é pulada.
 *
 * @param caminho Onde está o arquivo, por exemplo `dados/cinemas.txt`.
 * @return A lista de cinemas, na mesma ordem do arquivo.
 *
 * @warning Se o arquivo não for encontrado, a lista volta vazia e nenhum erro aparece.
 * @warning Um nome de cinema que tivesse vírgula seria cortado no lugar errado.
 *          Hoje nenhum cinema da base tem vírgula no nome.
 */
std::vector<Cinema> carregarCinemas(const std::string& caminho);

#endif
