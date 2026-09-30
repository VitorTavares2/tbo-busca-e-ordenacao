/**
 * @file arvore.cpp
 * @brief Código da árvore de números. A explicação de uso está em arvore.h.
 */

#include "arvore.h"

/**
 * Altura de uma entrada. Posição `-1` (sem nó) conta como 0.
 */
int ArvoreNumerica::alturaDe(int no) const {
    return no < 0 ? 0 : nos[no].altura;
}

/**
 * A altura de uma entrada é 1 (ela mesma) mais a altura do lado mais alto.
 */
void ArvoreNumerica::atualizarAltura(int no) {
    int alturaEsquerda = alturaDe(nos[no].esquerda);
    int alturaDireita = alturaDe(nos[no].direita);
    nos[no].altura = 1 + (alturaEsquerda > alturaDireita ? alturaEsquerda : alturaDireita);
}

/**
 * Usada quando o lado ESQUERDO ficou alto demais. O filho da esquerda sobe e o
 * nó atual desce para a direita dele.
 *
 *        no                filho
 *       /  \              /     \
 *    filho  C    ->      A      no
 *    /  \                      /  \
 *   A    B                    B    C
 *
 * Os números continuam em ordem: A < filho < B < no < C.
 *
 * @return A posição da entrada que agora fica no topo.
 */
int ArvoreNumerica::girarDireita(int no) {
    int filho = nos[no].esquerda;
    nos[no].esquerda = nos[filho].direita;
    nos[filho].direita = no;
    atualizarAltura(no);
    atualizarAltura(filho);
    return filho;
}

/**
 * O espelho de girarDireita(). Usada quando o lado DIREITO ficou alto demais.
 */
int ArvoreNumerica::girarEsquerda(int no) {
    int filho = nos[no].direita;
    nos[no].direita = nos[filho].esquerda;
    nos[filho].esquerda = no;
    atualizarAltura(no);
    atualizarAltura(filho);
    return filho;
}

/**
 * Como funciona:
 *  1. Se chegou num lugar vazio, cria a entrada nova aqui.
 *  2. Se o número é igual ao da entrada, só coloca o filme na lista dela.
 *  3. Se é menor, desce para a esquerda. Se é maior, desce para a direita.
 *  4. Na volta da descida, confere se um lado ficou 2 andares mais alto que o
 *     outro. Se ficou, gira para equilibrar. São 4 casos:
 *       - esquerda alta, e o filho da esquerda pesa para a esquerda: 1 giro.
 *       - esquerda alta, e o filho da esquerda pesa para a direita: 2 giros.
 *       - direita alta: os mesmos dois casos, espelhados.
 *
 * Quem chama guarda o valor devolvido, porque o topo pode mudar depois de um giro.
 */
int ArvoreNumerica::inserir(int no, int valor, int indiceFilme) {
    if (no < 0) {
        No novo;
        novo.valor = valor;
        novo.filmes.push_back(indiceFilme);
        novo.esquerda = -1;
        novo.direita = -1;
        novo.altura = 1;
        nos.push_back(novo);
        return static_cast<int>(nos.size()) - 1;
    }

    if (valor == nos[no].valor) {
        nos[no].filmes.push_back(indiceFilme);
        return no;
    }

    if (valor < nos[no].valor) {
        int novaEsquerda = inserir(nos[no].esquerda, valor, indiceFilme);
        nos[no].esquerda = novaEsquerda;
    } else {
        int novaDireita = inserir(nos[no].direita, valor, indiceFilme);
        nos[no].direita = novaDireita;
    }

    atualizarAltura(no);
    int diferenca = alturaDe(nos[no].esquerda) - alturaDe(nos[no].direita);

    if (diferenca > 1) {
        int esquerda = nos[no].esquerda;
        if (alturaDe(nos[esquerda].esquerda) < alturaDe(nos[esquerda].direita)) {
            nos[no].esquerda = girarEsquerda(esquerda);
        }
        return girarDireita(no);
    }
    if (diferenca < -1) {
        int direita = nos[no].direita;
        if (alturaDe(nos[direita].direita) < alturaDe(nos[direita].esquerda)) {
            nos[no].direita = girarDireita(direita);
        }
        return girarEsquerda(no);
    }
    return no;
}

void ArvoreNumerica::adicionar(int valor, int indiceFilme) {
    raiz = inserir(raiz, valor, indiceFilme);
}

/**
 * Como funciona: visita a árvore da esquerda para a direita (números do menor
 * para o maior), mas só entra nos lados que podem ter algo dentro do intervalo:
 *  - só desce para a esquerda se o número atual é MAIOR que `minimo`
 *    (se for igual ou menor, tudo à esquerda é pequeno demais);
 *  - só desce para a direita se o número atual é MENOR que `maximo`.
 * Os números que caem dentro do intervalo têm a lista de filmes copiada para o resultado.
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
        resultado.insert(resultado.end(), atual.filmes.begin(), atual.filmes.end());
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
