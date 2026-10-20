#include "indice.h"

void IndiceCategoria::adicionar(const std::string& chave, int indiceFilme) {
    for (size_t i = 0; i < chaves.size(); i++) {
        if (chaves[i] == chave) {
            listas[i].push_back(indiceFilme);
            return;
        }
    }
    chaves.push_back(chave);
    listas.push_back(std::vector<int>{indiceFilme});
}

std::vector<int> IndiceCategoria::buscar(const std::string& chave) const {
    for (size_t i = 0; i < chaves.size(); i++) {
        if (minusculas(chaves[i]) == minusculas(chave)) {
            return listas[i];
        }
    }
    return std::vector<int>();
}

std::vector<int> intersecao(const std::vector<int>& a, const std::vector<int>& b) {
    std::vector<int> resultado;
    size_t i = 0;
    size_t j = 0;
    while (i < a.size() && j < b.size()) {
        if (a[i] == b[j]) {
            resultado.push_back(a[i]);
            i++;
            j++;
        } else if (a[i] < b[j]) {
            i++;
        } else {
            j++;
        }
    }
    return resultado;
}

std::vector<int> uniao(const std::vector<int>& a, const std::vector<int>& b) {
    std::vector<int> resultado;
    size_t i = 0;
    size_t j = 0;
    while (i < a.size() && j < b.size()) {
        if (a[i] == b[j]) {
            resultado.push_back(a[i]);
            i++;
            j++;
        } else if (a[i] < b[j]) {
            resultado.push_back(a[i]);
            i++;
        } else {
            resultado.push_back(b[j]);
            j++;
        }
    }
    while (i < a.size()) {
        resultado.push_back(a[i]);
        i++;
    }
    while (j < b.size()) {
        resultado.push_back(b[j]);
        j++;
    }
    return resultado;
}
