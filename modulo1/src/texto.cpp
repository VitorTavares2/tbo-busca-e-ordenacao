#include "texto.h"

std::vector<std::string> dividir(const std::string& texto, char delimitador) {
    std::vector<std::string> partes;
    std::string atual;
    for (char c : texto) {
        if (c == delimitador) {
            partes.push_back(atual);
            atual.clear();
        } else {
            atual += c;
        }
    }
    partes.push_back(atual);
    return partes;
}

std::string aparar(const std::string& texto) {
    size_t inicio = 0;
    size_t fim = texto.size();
    while (inicio < fim && (texto[inicio] == ' ' || texto[inicio] == '\r' || texto[inicio] == '\n')) {
        inicio++;
    }
    while (fim > inicio && (texto[fim - 1] == ' ' || texto[fim - 1] == '\r' || texto[fim - 1] == '\n')) {
        fim--;
    }
    return texto.substr(inicio, fim - inicio);
}
