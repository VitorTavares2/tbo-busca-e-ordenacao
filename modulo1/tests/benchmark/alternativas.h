#ifndef ALTERNATIVAS_H
#define ALTERNATIVAS_H

#include <string>
#include <vector>
#include "../../src/modelos.h"

inline std::vector<int> intersecaoAnterior(const std::vector<int>& a, const std::vector<int>& b) {
    std::vector<int> resultado;
    for (int x : a) {
        for (int y : b) {
            if (x == y) {
                resultado.push_back(x);
                break;
            }
        }
    }
    return resultado;
}

inline std::vector<int> uniaoAnterior(const std::vector<int>& a, const std::vector<int>& b) {
    std::vector<int> resultado = a;
    for (int y : b) {
        bool existe = false;
        for (int x : a) {
            if (x == y) {
                existe = true;
                break;
            }
        }
        if (!existe) {
            resultado.push_back(y);
        }
    }
    return resultado;
}

struct ValorPosicao {
    int valor;
    int posicao;
};

inline void ordenarComMergeSort(std::vector<ValorPosicao>& lista, std::vector<ValorPosicao>& auxiliar, int inicio, int fim) {
    if (inicio >= fim) {
        return;
    }
    int meio = (inicio + fim) / 2;
    ordenarComMergeSort(lista, auxiliar, inicio, meio);
    ordenarComMergeSort(lista, auxiliar, meio + 1, fim);
    int i = inicio;
    int j = meio + 1;
    int k = inicio;
    while (i <= meio && j <= fim) {
        auxiliar[k++] = (lista[j].valor < lista[i].valor) ? lista[j++] : lista[i++];
    }
    while (i <= meio) {
        auxiliar[k++] = lista[i++];
    }
    while (j <= fim) {
        auxiliar[k++] = lista[j++];
    }
    for (int t = inicio; t <= fim; t++) {
        lista[t] = auxiliar[t];
    }
}

inline int buscaLinearPorCodigo(const std::vector<Filme>& filmes, const std::string& codigo, long long& comparacoes) {
    for (size_t i = 0; i < filmes.size(); i++) {
        comparacoes++;
        if (filmes[i].id == codigo) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

inline std::vector<int> buscaSequencialIntervalo(const std::vector<Filme>& filmes, bool porAno, int minimo, int maximo) {
    std::vector<int> resultado;
    for (size_t i = 0; i < filmes.size(); i++) {
        int valor = porAno ? filmes[i].ano : filmes[i].duracao;
        if (valor >= minimo && valor <= maximo) {
            resultado.push_back(static_cast<int>(i));
        }
    }
    return resultado;
}

inline std::vector<int> varrerTipoEGenero(const std::vector<Filme>& filmes, const std::string& tipo, const std::string& genero) {
    std::vector<int> resultado;
    for (size_t i = 0; i < filmes.size(); i++) {
        if (filmes[i].tipo != tipo) {
            continue;
        }
        for (const std::string& g : filmes[i].generos) {
            if (g == genero) {
                resultado.push_back(static_cast<int>(i));
                break;
            }
        }
    }
    return resultado;
}

class ArvoreContada {
public:
    ArvoreContada(const std::vector<Filme>& filmes, bool porAno) {
        std::vector<char> existe;
        for (const Filme& filme : filmes) {
            int valor = porAno ? filme.ano : filme.duracao;
            if (valor < 0) continue;
            if (valor >= static_cast<int>(existe.size())) existe.resize(valor + 1, 0);
            existe[valor] = 1;
        }
        for (size_t valor = 0; valor < existe.size(); valor++) {
            if (existe[valor]) nos.push_back({static_cast<int>(valor), -1, -1});
        }
        raiz = ligar(0, static_cast<int>(nos.size()) - 1);
    }

    int contarVisitas(int minimo, int maximo) const {
        int visitas = 0;
        visitar(raiz, minimo, maximo, visitas);
        return visitas;
    }

    int quantidade() const { return static_cast<int>(nos.size()); }

private:
    struct No { int valor; int esquerda; int direita; };
    std::vector<No> nos;
    int raiz = -1;

    int ligar(int inicio, int fim) {
        if (inicio > fim) return -1;
        int meio = (inicio + fim) / 2;
        nos[meio].esquerda = ligar(inicio, meio - 1);
        nos[meio].direita = ligar(meio + 1, fim);
        return meio;
    }

    void visitar(int no, int minimo, int maximo, int& visitas) const {
        if (no < 0) return;
        visitas++;
        if (nos[no].valor > minimo) visitar(nos[no].esquerda, minimo, maximo, visitas);
        if (nos[no].valor < maximo) visitar(nos[no].direita, minimo, maximo, visitas);
    }
};

#endif
