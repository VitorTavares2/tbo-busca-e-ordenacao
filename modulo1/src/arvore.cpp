/**
 * @file arvore.cpp
 * @brief Código da árvore de números. A explicação de uso está em arvore.h.
 */

#include "arvore.h"

/**
 * Se o balde do número ainda não existe, a lista de baldes cresce até ele.
 * Depois o filme vai para o fim do balde.
 */
void ArvoreNumerica::adicionar(int valor, int indiceFilme) {
    if (valor < 0) {
        return;
    }
    if (valor >= static_cast<int>(filmesPorValor.size())) {
        filmesPorValor.resize(valor + 1);
    }
    filmesPorValor[valor].push_back(indiceFilme);
}

/**
 * Como funciona:
 *  1. Percorre os baldes do 0 para cima. Cada balde que tem filme vira uma
 *     entrada em `nos`. Por isso `nos` fica em ordem crescente de número,
 *     sem precisar ordenar nada.
 *  2. Chama ligar() na lista inteira para escolher o topo e os filhos.
 */
void ArvoreNumerica::montar() {
    nos.clear();
    for (size_t valor = 0; valor < filmesPorValor.size(); valor++) {
        if (!filmesPorValor[valor].empty()) {
            No novo;
            novo.valor = static_cast<int>(valor);
            novo.esquerda = -1;
            novo.direita = -1;
            nos.push_back(novo);
        }
    }
    raiz = ligar(0, static_cast<int>(nos.size()) - 1);
}

/**
 * Liga as entradas de `nos[inicio..fim]` (já em ordem) numa árvore e devolve a posição do topo.
 *
 * O do meio vira o topo. A metade de baixo é ligada do mesmo jeito e vira o
 * lado esquerdo; a metade de cima vira o lado direito. Exemplo com 7 números:
 *
 *   posições:  0   1   2  [3]  4   5   6      → o 3 é o topo
 *   esquerda:  0  [1]  2                      → o 1 é o filho esquerdo do 3
 *   direita:                   4  [5]  6      → o 5 é o filho direito do 3
 *
 * Quando `inicio` passa de `fim`, o pedaço está vazio e não há nó (`-1`).
 */
int ArvoreNumerica::ligar(int inicio, int fim) {
    if (inicio > fim) {
        return -1;
    }
    int meio = (inicio + fim) / 2;
    nos[meio].esquerda = ligar(inicio, meio - 1);
    nos[meio].direita = ligar(meio + 1, fim);
    return meio;
}

/**
 * Altura de uma entrada: 1 (ela mesma) mais a altura do lado mais alto.
 * Posição `-1` (sem nó) conta como 0.
 */
int ArvoreNumerica::alturaDe(int no) const {
    if (no < 0) {
        return 0;
    }
    int alturaEsquerda = alturaDe(nos[no].esquerda);
    int alturaDireita = alturaDe(nos[no].direita);
    // ternário: condicao ? valor_se_verdadeiro : valor_se_falso. Pega o lado mais alto.
    return 1 + (alturaEsquerda > alturaDireita ? alturaEsquerda : alturaDireita);
}

/**
 * Como funciona: visita a árvore da esquerda para a direita (números do menor
 * para o maior), mas só entra nos lados que podem ter algo dentro do intervalo:
 *  - só desce para a esquerda se o número atual é MAIOR que `minimo`
 *    (se for igual ou menor, tudo à esquerda é pequeno demais);
 *  - só desce para a direita se o número atual é MENOR que `maximo`.
 * Os números que caem dentro do intervalo têm o balde de filmes copiado para o resultado.
 */
void ArvoreNumerica::coletar(int no, int minimo, int maximo, std::vector<int>& resultado) const {
    if (no < 0) {
        return;
    }
    const No& atual = nos[no];
    if (atual.valor > minimo) {
        coletar(atual.esquerda, minimo, maximo, resultado);
    }
    if (atual.valor >= minimo && atual.valor <= maximo) {
        const std::vector<int>& filmes = filmesPorValor[atual.valor];
        resultado.insert(resultado.end(), filmes.begin(), filmes.end());
    }
    if (atual.valor < maximo) {
        coletar(atual.direita, minimo, maximo, resultado);
    }
}

std::vector<int> ArvoreNumerica::buscarIntervalo(int minimo, int maximo) const {
    std::vector<int> resultado;
    if (minimo <= maximo) {
        coletar(raiz, minimo, maximo, resultado);
    }
    return resultado;
}

int ArvoreNumerica::quantidadeValores() const {
    return static_cast<int>(nos.size());
}

int ArvoreNumerica::altura() const {
    return alturaDe(raiz);
}
