/**
 * @file alternativas.h
 * @brief Soluções que consideramos e NÃO usamos no programa. Existem só para o benchmark comparar.
 *
 * Cada função aqui resolve o mesmo problema de uma função do programa, de outro jeito:
 *  - intersecaoAnterior() / uniaoAnterior(): versão do E / OU antes da correção (lista contra lista inteira);
 *  - ordenarComMergeSort(): ordenar (valor, posição) em vez de usar baldes;
 *  - buscaLinearPorCodigo(): achar um filme pelo código olhando um por um, em vez de busca binária;
 *  - buscaSequencialIntervalo(): achar filmes de um intervalo olhando os 584 mil, em vez da árvore;
 *  - varrerTipoEGenero(): responder "tipo E gênero" sem índice nenhum.
 */

#ifndef ALTERNATIVAS_H
#define ALTERNATIVAS_H

#include <string>
#include <vector>
#include "../../src/modelos.h"

/// E antes da correção: para cada item de `a`, procura em `b` desde o começo.
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

/// OU antes da correção: copia `a` e procura cada item de `b` em `a` inteira.
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

/// Um filme com o número dele (ano ou duração), para ordenar.
struct ValorPosicao {
    int valor;
    int posicao;
};

/// Merge sort feito à mão (o enunciado proíbe std::sort). Ordena por valor e é estável.
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

/// Acha a posição de um código olhando os filmes um por um. Conta as comparações feitas.
inline int buscaLinearPorCodigo(const std::vector<Filme>& filmes, const std::string& codigo, long long& comparacoes) {
    for (size_t i = 0; i < filmes.size(); i++) {
        comparacoes++;
        if (filmes[i].id == codigo) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

/// Acha os filmes com ano (ou duração) entre `minimo` e `maximo` olhando os 584 mil.
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

/// Responde "tipo E gênero" olhando os 584 mil filmes, sem usar índice.
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

/**
 * Cópia da ArvoreNumerica (baldes + montar pelo meio + coletar) que, em vez de
 * copiar filmes, conta quantos nós a busca visita. Serve só para medir a poda.
 */
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
