#ifndef CACHE_H
#define CACHE_H

#include <string>
#include <vector>

class CacheConsultas {
public:
    bool buscar(const std::string& chave, std::vector<int>& resultado) const;
    void adicionar(const std::string& chave, const std::vector<int>& resultado);

private:
    std::vector<std::string> chaves;
    std::vector<std::vector<int>> resultados;
};

#endif
