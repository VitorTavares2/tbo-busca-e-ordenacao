/**
 * @file indice.cpp
 * @brief Código do índice por categoria e das operações E / OU. A explicação de uso está em indice.h.
 */

#include "indice.h"

/**
 * Como funciona:
 *  1. Procura a categoria na lista de nomes.
 *  2. Achou: coloca a posição do filme no fim da lista dessa categoria.
 *  3. Não achou: cria a categoria nova, já com esse filme dentro.
 *
 * Os filmes são adicionados em ordem (0, 1, 2...) quando o programa abre.
 * Por isso cada lista fica naturalmente em ordem crescente.
 */
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

/**
 * Como funciona: procura a categoria pelo nome e devolve uma cópia da lista dela.
 * Se não encontrar, devolve uma lista vazia.
 */
std::vector<int> IndiceCategoria::buscar(const std::string& chave) const {
    for (size_t i = 0; i < chaves.size(); i++) {
        if (chaves[i] == chave) {
            return listas[i];
        }
    }
    return std::vector<int>();
}

/**
 * Como funciona: para cada posição de `a`, procura a mesma posição em `b`.
 * Se achar, ela entra no resultado. O `break` para a procura assim que acha,
 * porque não precisa continuar olhando o resto de `b`.
 */
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

/**
 * Como funciona:
 *  1. Começa o resultado com tudo que está em `a`.
 *  2. Para cada posição de `b`, verifica se ela já estava em `a`.
 *  3. Se não estava, adiciona no fim do resultado.
 * Assim nenhuma posição aparece repetida.
 */
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
