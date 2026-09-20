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
        if (chaves[i] == chave) {
            return listas[i];
        }
    }
    return std::vector<int>();
}

std::vector<int> intersecao(const std::vector<int>& a, const std::vector<int>& b) {
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

std::vector<int> uniao(const std::vector<int>& a, const std::vector<int>& b) {
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
