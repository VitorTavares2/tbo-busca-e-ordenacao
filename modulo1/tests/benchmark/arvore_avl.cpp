#include "arvore_avl.h"

int ArvoreAVL::alturaDe(int no) const {
    return no < 0 ? 0 : nos[no].altura;
}

void ArvoreAVL::atualizarAltura(int no) {
    int alturaEsquerda = alturaDe(nos[no].esquerda);
    int alturaDireita = alturaDe(nos[no].direita);
    nos[no].altura = 1 + (alturaEsquerda > alturaDireita ? alturaEsquerda : alturaDireita);
}

int ArvoreAVL::girarDireita(int no) {
    int filho = nos[no].esquerda;
    nos[no].esquerda = nos[filho].direita;
    nos[filho].direita = no;
    atualizarAltura(no);
    atualizarAltura(filho);
    return filho;
}

int ArvoreAVL::girarEsquerda(int no) {
    int filho = nos[no].direita;
    nos[no].direita = nos[filho].esquerda;
    nos[filho].esquerda = no;
    atualizarAltura(no);
    atualizarAltura(filho);
    return filho;
}

int ArvoreAVL::inserir(int no, int valor, int indiceFilme) {
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

void ArvoreAVL::adicionar(int valor, int indiceFilme) {
    raiz = inserir(raiz, valor, indiceFilme);
}

void ArvoreAVL::coletar(int no, int minimo, int maximo, std::vector<int>& resultado) const {
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

std::vector<int> ArvoreAVL::buscarIntervalo(int minimo, int maximo) const {
    std::vector<int> resultado;
    if (minimo <= maximo) {
        coletar(raiz, minimo, maximo, resultado);
    }
    return resultado;
}

int ArvoreAVL::quantidadeValores() const {
    return static_cast<int>(nos.size());
}

int ArvoreAVL::altura() const {
    return alturaDe(raiz);
}
