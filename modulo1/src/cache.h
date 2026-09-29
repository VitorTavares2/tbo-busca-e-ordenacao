/**
 * @file cache.h
 * @brief Memória das buscas já feitas, para não refazer o mesmo cálculo.
 *
 * O que é o cache: quando o usuário faz uma busca, o programa guarda o
 * resultado junto com uma descrição dela (a "chave"). Se a mesma busca for
 * feita de novo, o resultado guardado é devolvido na hora, sem recalcular.
 *
 * Exemplo de chave: `filme|E|tipo=movie|genero=Comedy`
 * (quer dizer: busca de filmes, operação E, tipo movie, gênero Comedy).
 */

#ifndef CACHE_H
#define CACHE_H

#include <string>
#include <vector>

/**
 * @brief Guarda pares "descrição da busca -> resultado".
 *
 * Como os dados ficam guardados: duas listas lado a lado.
 * `chaves[i]` é a descrição da busca e `resultados[i]` é o que ela encontrou.
 *
 * @note Limitações atuais:
 *  - Para achar uma busca, o código olha as buscas guardadas uma por uma.
 *    O enunciado pede que isso seja instantâneo (O(1)), o que exigiria uma
 *    tabela hash feita à mão.
 *  - Nada é apagado. A memória cresce a cada busca nova enquanto o programa roda.
 *  - Mudar a ordem dos filtros gera outra chave. `movie E Comedy` e
 *    `Comedy E movie` são guardadas como buscas diferentes, apesar de darem o mesmo resultado.
 */
class CacheConsultas {
public:
    /**
     * @brief Procura uma busca já feita.
     *
     * @param chave     Descrição da busca (montada por montarChaveCache() no main.cpp).
     * @param resultado Se a busca for encontrada, o resultado guardado é copiado para cá.
     *                  Se não for, esta variável não é alterada.
     * @return `true` se a busca já estava guardada, `false` se é uma busca nova.
     */
    bool buscar(const std::string& chave, std::vector<int>& resultado) const;

    /**
     * @brief Guarda o resultado de uma busca nova.
     *
     * @param chave     Descrição da busca.
     * @param resultado O que a busca encontrou (posições de filmes).
     *
     * @note Não verifica se a chave já existe. Quem chama deve usar buscar() antes.
     */
    void adicionar(const std::string& chave, const std::vector<int>& resultado);

private:
    std::vector<std::string> chaves;           ///< Descrições das buscas guardadas.
    std::vector<std::vector<int>> resultados;  ///< `resultados[i]` = resultado da busca `chaves[i]`.
};

#endif
