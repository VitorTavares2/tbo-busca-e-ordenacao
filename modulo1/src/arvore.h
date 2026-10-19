/**
 * @file arvore.h
 * @brief Árvore que guarda números (ano, duração) e acha rapidamente todos os filmes dentro de um intervalo.
 *
 * Para que serve: responder perguntas como "filmes com duração entre 90 e 120
 * minutos" ou "filmes lançados entre 2000 e 2010" sem olhar os 584 mil filmes
 * um por um.
 *
 * Como a árvore funciona, com um exemplo de durações:
 *
 *                 90
 *               /    \
 *            60       120
 *           /  \     /   \
 *         45   75  100   150
 *
 *  - Tudo que está à ESQUERDA de um número é menor que ele.
 *  - Tudo que está à DIREITA é maior.
 *  - Para achar o intervalo 95..130, o programa começa no 90. Como 90 é menor
 *    que 95, ele ignora a esquerda inteira e vai só para a direita. Isso
 *    descarta metade dos dados a cada passo.
 *
 * A árvore é usada em duas fases:
 *  1. **Separar** (`adicionar`): cada filme vai para o "balde" do seu número.
 *     O balde 90 guarda todos os filmes de 90 minutos. Como os baldes ficam
 *     numa lista na posição do próprio número, eles já estão em ordem crescente.
 *  2. **Montar** (`montar`): com os números já em ordem, o do MEIO vira o topo
 *     da árvore, a metade menor vira o lado esquerdo e a metade maior vira o
 *     lado direito. O mesmo é feito em cada metade, até não sobrar nada.
 *
 * Como o topo é sempre o meio, os dois lados têm o mesmo tamanho (ou diferem
 * em 1). A árvore já nasce equilibrada e nunca precisa ser reorganizada,
 * porque depois do carregamento nenhum filme novo entra.
 */

#ifndef ARVORE_H
#define ARVORE_H

#include <vector>

/**
 * @brief Guarda, para cada número, a lista de posições dos filmes que têm esse número.
 *
 * O programa cria duas árvores deste tipo: uma para o **ano** e outra para a **duração**.
 *
 * @note Os nós ficam dentro de uma lista (`nos`) e se ligam por posição, não
 *       por ponteiro. Isso evita ter que apagar cada nó na mão.
 */
class ArvoreNumerica {
public:
    /**
     * @brief Anota que o filme da posição `indiceFilme` tem o número `valor`.
     *
     * O filme é colocado no fim do balde daquele número. A árvore só passa a
     * enxergar o filme depois de `montar()`.
     *
     * @param valor        O número a guardar. Exemplo: `2008` (ano) ou `90` (minutos).
     * @param indiceFilme  Posição do filme na lista de filmes.
     *
     * @note Filmes sem ano ou sem duração (valor `-1`) não devem ser adicionados.
     *       Quem chama esta função é que faz essa checagem. Valores negativos
     *       são ignorados.
     */
    void adicionar(int valor, int indiceFilme);

    /**
     * @brief Monta a árvore com todos os números adicionados até agora.
     *
     * Deve ser chamada uma vez, depois de adicionar todos os filmes e antes da
     * primeira busca. Se for chamada de novo, a árvore é montada do zero.
     */
    void montar();

    /**
     * @brief Devolve as posições de todos os filmes cujo número está entre `minimo` e `maximo`.
     *
     * As duas pontas ENTRAM no resultado. Para buscar um valor só (ex: o ano
     * 2000), use o mesmo número nos dois: `buscarIntervalo(2000, 2000)`.
     *
     * @param minimo Menor número aceito.
     * @param maximo Maior número aceito.
     * @return Posições dos filmes encontrados, do MENOR número para o MAIOR.
     *         Dentro do mesmo número, na ordem em que foram adicionados.
     *         Volta vazia se nada for encontrado ou se `minimo` for maior que `maximo`.
     */
    std::vector<int> buscarIntervalo(int minimo, int maximo) const;

    /**
     * @brief Diz quantos números DIFERENTES estão na árvore montada.
     *
     * Exemplo: três filmes de 90 min e um de 100 min contam como 2 números.
     */
    int quantidadeValores() const;

    /**
     * @brief Diz quantos "andares" a árvore tem. Árvore vazia tem 0.
     *
     * Quanto menor, mais rápida é a busca. Com 1000 números diferentes, uma
     * árvore montada pelo meio tem 10 andares. Uma torta teria 1000.
     */
    int altura() const;

private:
    /// Uma entrada da árvore. Os filmes dela ficam em `filmesPorValor[valor]`.
    struct No {
        int valor;    ///< O número desta entrada.
        int esquerda; ///< Posição do nó com números menores. `-1` se não houver.
        int direita;  ///< Posição do nó com números maiores. `-1` se não houver.
    };

    std::vector<std::vector<int>> filmesPorValor; ///< Os baldes: `filmesPorValor[90]` = filmes de 90.
    std::vector<No> nos;  ///< Todas as entradas da árvore, em ordem crescente de número.
    int raiz = -1;        ///< Posição da entrada de cima. `-1` enquanto a árvore está vazia.

    int ligar(int inicio, int fim);
    int alturaDe(int no) const;
    void coletar(int no, int minimo, int maximo, std::vector<int>& resultado) const;
};

#endif
