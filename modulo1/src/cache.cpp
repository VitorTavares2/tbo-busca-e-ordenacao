#include "cache.h"

bool CacheConsultas::buscar(const std::string& chave, std::vector<int>& resultado) const {
    for (size_t i = 0; i < chaves.size(); i++) {
        if (chaves[i] == chave) {
            resultado = resultados[i];
            return true;
        }
    }
    return false;
}

void CacheConsultas::adicionar(const std::string& chave, const std::vector<int>& resultado) {
    chaves.push_back(chave);
    resultados.push_back(resultado);
}
