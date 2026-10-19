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
 * Como funciona: as duas listas estão em ordem crescente, então dá para andar
 * nas duas ao mesmo tempo, com um marcador em cada (`i` em `a`, `j` em `b`):
 *  - valores iguais: o filme está nas duas listas. Entra no resultado e os dois marcadores andam;
 *  - `a[i]` menor: como `b` só cresce daqui para frente, `a[i]` nunca vai aparecer em `b`. Só `i` anda;
 *  - `b[j]` menor: o mesmo raciocínio, só `j` anda.
 * Termina quando uma das listas acaba: o que sobrou na outra não pode estar nas duas.
 * Cada posição é olhada uma vez só.
 */
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

/**
 * Como funciona: anda nas duas listas ao mesmo tempo e, a cada passo, coloca
 * no resultado o MENOR dos dois valores da vez:
 *  - valores iguais: entra uma vez só e os dois marcadores andam (assim nada se repete);
 *  - senão, entra o menor e só o marcador dele anda.
 * Quando uma lista acaba, o resto da outra entra inteiro (já está em ordem).
 * O resultado continua em ordem crescente, o que permite combinar um terceiro filtro.
 */
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
