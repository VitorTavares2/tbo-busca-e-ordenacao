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
 * Filmes com o mesmo número (por exemplo, vários filmes de 90 minutos) ficam
 * juntos na MESMA entrada da árvore. A entrada guarda a lista de posições deles.
 *
 * A árvore é do tipo AVL: depois de cada inclusão ela se reorganiza sozinha
 * para não ficar torta. Sem isso, números que chegam em ordem (1, 2, 3, 4...)
 * formariam uma "escada" e a busca voltaria a ser lenta.
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
     * Se o número ainda não está na árvore, uma entrada nova é criada.
     * Se já está, o filme é colocado no fim da lista daquela entrada.
     *
     * @param valor        O número a guardar. Exemplo: `2008` (ano) ou `90` (minutos).
     * @param indiceFilme  Posição do filme na lista de filmes.
     *
     * @note Filmes sem ano ou sem duração (valor `-1`) não devem ser adicionados.
     *       Quem chama esta função é que faz essa checagem.
     */
    void adicionar(int valor, int indiceFilme);

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
     * @brief Diz quantos números DIFERENTES estão guardados.
     *
     * Exemplo: três filmes de 90 min e um de 100 min contam como 2 números.
     */
    int quantidadeValores() const;

    /**
     * @brief Diz quantos "andares" a árvore tem. Árvore vazia tem 0.
     *
     * Quanto menor, mais rápida é a busca. Com 1000 números diferentes, uma
     * árvore bem arrumada tem uns 10 andares. Uma torta teria 1000.
     */
    int altura() const;

private:
    /// Uma entrada da árvore: um número e os filmes que têm esse número.
    struct No {
        int valor;               ///< O número desta entrada.
        std::vector<int> filmes; ///< Posições dos filmes que têm este número.
        int esquerda;            ///< Posição do nó com números menores. `-1` se não houver.
        int direita;             ///< Posição do nó com números maiores. `-1` se não houver.
        int altura;              ///< Andares desta entrada para baixo. Uma entrada sem filhos tem 1.
    };

    std::vector<No> nos;  ///< Todas as entradas da árvore.
    int raiz = -1;        ///< Posição da entrada de cima. `-1` enquanto a árvore está vazia.

    int alturaDe(int no) const;
    void atualizarAltura(int no);
    int girarDireita(int no);
    int girarEsquerda(int no);
    int inserir(int no, int valor, int indiceFilme);
    void coletar(int no, int minimo, int maximo, std::vector<int>& resultado) const;
};

#endif
