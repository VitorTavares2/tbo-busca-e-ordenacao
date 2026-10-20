#include "texto.h"
#include <cctype>

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

bool converterInteiro(const std::string& texto, int& valor) {
    const size_t MAXIMO_DE_DIGITOS = 9;
    std::string limpo = aparar(texto);
    bool negativo = !limpo.empty() && limpo[0] == '-';
    std::string digitos = negativo ? limpo.substr(1) : limpo;

    if (digitos.empty() || digitos.size() > MAXIMO_DE_DIGITOS) {
        return false;
    }
    int resultado = 0;
    for (char c : digitos) {
        if (c < '0' || c > '9') {
            return false;
        }
        resultado = resultado * 10 + (c - '0');
    }
    valor = negativo ? -resultado : resultado;
    return true;
}

std::string minusculas(std::string texto) {
    for (char& letra : texto) {
        letra = static_cast<char>(std::tolower(static_cast<unsigned char>(letra)));
    }
    return texto;
}
