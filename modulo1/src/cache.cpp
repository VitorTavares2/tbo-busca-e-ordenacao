/**
 * @file cache.cpp
 * @brief Código do cache de buscas. A explicação de uso está em cache.h.
 */

#include "cache.h"

/**
 * Como funciona: compara a chave pedida com cada chave guardada, uma por uma.
 * Se encontrar uma igual, copia o resultado correspondente e devolve `true`.
 */
bool CacheConsultas::buscar(const std::string& chave, std::vector<int>& resultado) const {
    for (size_t i = 0; i < chaves.size(); i++) {
        if (chaves[i] == chave) {
            resultado = resultados[i];
            return true;
        }
    }
    return false;
}

/**
 * Como funciona: coloca a chave e o resultado no fim das duas listas, na mesma
 * posição. É isso que mantém `chaves[i]` ligada a `resultados[i]`.
 */
void CacheConsultas::adicionar(const std::string& chave, const std::vector<int>& resultado) {
    chaves.push_back(chave);
    resultados.push_back(resultado);
}
