/**
 * @file texto.cpp
 * @brief Código das funções dividir() e aparar(). A explicação de uso está em texto.h.
 */

#include "texto.h"

/**
 * Como funciona: lê o texto letra por letra e junta as letras num pedaço.
 * Quando encontra o separador, guarda o pedaço e começa um novo.
 * No final, guarda também o último pedaço, que não termina em separador.
 */
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

/**
 * Como funciona: anda do começo para frente e do fim para trás, pulando
 * espaços e quebras de linha. Depois pega só o trecho do meio.
 *
 * Por que isso importa: os arquivos foram salvos no Windows, onde cada linha
 * termina com dois caracteres invisíveis (`\r` e `\n`). A leitura de linha do
 * C++ remove só o `\n`. O `\r` que sobra é removido aqui.
 */
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
