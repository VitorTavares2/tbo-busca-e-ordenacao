/**
 * @file indice.h
 * @brief Índice de filmes por categoria (tipo ou gênero) e as funções que combinam resultados.
 *
 * O que é um índice aqui: uma lista pronta que diz, para cada categoria,
 * quais filmes pertencem a ela. Exemplo:
 *
 *     "Comedy"      -> filmes nas posições 3, 17, 42, ...
 *     "Documentary" -> filmes nas posições 5, 9, 88, ...
 *
 * Essas listas são montadas uma vez, quando o programa abre. Depois, buscar
 * "todos os filmes de comédia" é só pegar a lista pronta. Não é preciso olhar
 * os 584 mil filmes de novo a cada busca.
 */

#ifndef INDICE_H
#define INDICE_H

#include <string>
#include <vector>

/**
 * @brief Guarda, para cada categoria, a lista de posições dos filmes daquela categoria.
 *
 * O programa cria dois índices deste tipo:
 *  - um por **tipo**   (`movie`, `tvEpisode`, `short`...)
 *  - um por **gênero** (`Comedy`, `Drama`, `Action`...)
 *
 * Como os dados ficam guardados: duas listas lado a lado.
 * `chaves[i]` é o nome da categoria e `listas[i]` são as posições dos filmes dela.
 *
 *     chaves = { "Drama",      "Short",   ... }
 *     listas = { {1, 2, 7..},  {0, 3..},  ... }
 */
class IndiceCategoria {
public:
    /**
     * @brief Anota que o filme da posição `indiceFilme` pertence à categoria `chave`.
     *
     * Se a categoria ainda não existe no índice, ela é criada na hora.
     *
     * @param chave        Nome da categoria. Exemplo: `Comedy`.
     * @param indiceFilme  Posição do filme na lista de filmes.
     *
     * @note Para achar a categoria, o código olha as categorias uma por uma.
     *       Isso é rápido aqui porque existem só 10 tipos e poucas dezenas de gêneros.
     */
    void adicionar(const std::string& chave, int indiceFilme);

    /**
     * @brief Devolve as posições de todos os filmes de uma categoria.
     *
     * @param chave Nome da categoria. Diferencia maiúsculas de minúsculas:
     *              `Comedy` funciona, `comedy` não encontra nada.
     * @return Cópia da lista de posições. Volta vazia se a categoria não existir.
     */
    std::vector<int> buscar(const std::string& chave) const;

private:
    std::vector<std::string> chaves;       ///< Nomes das categorias, na ordem em que apareceram.
    std::vector<std::vector<int>> listas;  ///< `listas[i]` = posições dos filmes da categoria `chaves[i]`.
};

/**
 * @brief Operação **E**: devolve só as posições que estão nas DUAS listas.
 *
 * Exemplo: `a = {1, 4, 7}` e `b = {4, 7, 9}` devolve `{4, 7}`.
 * Uso: "filmes que são `movie` E `Comedy`".
 *
 * @param a Primeira lista de posições.
 * @param b Segunda lista de posições.
 * @return Posições presentes em `a` e em `b`, na ordem de `a`.
 *
 * @note Velocidade: para cada item de `a`, procura em `b` inteira. Com listas
 *       grandes (ex: 474 mil episódios de TV) isso fica lento. Como as listas
 *       já saem do índice em ordem crescente, dá para fazer isso bem mais
 *       rápido andando nas duas listas ao mesmo tempo.
 */
std::vector<int> intersecao(const std::vector<int>& a, const std::vector<int>& b);

/**
 * @brief Operação **OU**: devolve as posições que estão em pelo menos UMA das listas, sem repetir.
 *
 * Exemplo: `a = {1, 4}` e `b = {4, 9}` devolve `{1, 4, 9}`.
 * Uso: "filmes que são `Comedy` OU `Drama`".
 *
 * @param a Primeira lista de posições.
 * @param b Segunda lista de posições.
 * @return Tudo de `a`, seguido dos itens de `b` que não estavam em `a`.
 *
 * @note O resultado pode sair fora de ordem crescente. Exemplo: `a = {5}` e `b = {2}` devolve `{5, 2}`.
 * @note Velocidade: mesmo problema da intersecao(). Cada item de `b` é procurado em `a` inteira.
 */
std::vector<int> uniao(const std::vector<int>& a, const std::vector<int>& b);

#endif
