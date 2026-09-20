#ifndef INDICE_H
#define INDICE_H

#include <string>
#include <vector>

class IndiceCategoria {
public:
    void adicionar(const std::string& chave, int indiceFilme);
    std::vector<int> buscar(const std::string& chave) const;

private:
    std::vector<std::string> chaves;
    std::vector<std::vector<int>> listas;
};

std::vector<int> intersecao(const std::vector<int>& a, const std::vector<int>& b);
std::vector<int> uniao(const std::vector<int>& a, const std::vector<int>& b);

#endif
