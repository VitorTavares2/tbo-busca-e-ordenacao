#include "arvore.h"

void ArvoreNumerica::adicionar(int valor, int indiceFilme) {
    if (valor < 0) {
        return;
    }
    if (valor >= static_cast<int>(filmesPorValor.size())) {
        filmesPorValor.resize(valor + 1);
    }
    filmesPorValor[valor].push_back(indiceFilme);
}

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

int ArvoreNumerica::ligar(int inicio, int fim) {
    if (inicio > fim) {
        return -1;
    }
    int meio = (inicio + fim) / 2;
    nos[meio].esquerda = ligar(inicio, meio - 1);
    nos[meio].direita = ligar(meio + 1, fim);
    return meio;
}

int ArvoreNumerica::alturaDe(int no) const {
    if (no < 0) {
        return 0;
    }
    int alturaEsquerda = alturaDe(nos[no].esquerda);
    int alturaDireita = alturaDe(nos[no].direita);
    return 1 + (alturaEsquerda > alturaDireita ? alturaEsquerda : alturaDireita);
}

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
