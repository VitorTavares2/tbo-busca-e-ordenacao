/**
 * @file entrada.cpp
 * @brief Código das perguntas ao usuário. A explicação de uso está em entrada.h.
 */

#include "entrada.h"
#include "texto.h"
#include <iostream>

/**
 * Como funciona: escreve a pergunta, lê a linha inteira e tira os espaços das pontas.
 * Se a leitura falhar (entrada acabou), devolve `false`.
 */
bool lerLinha(const std::string& pergunta, std::string& texto) {
    std::cout << pergunta << std::flush;
    std::string linha;
    if (!std::getline(std::cin, linha)) {
        std::cout << std::endl;
        return false;
    }
    texto = aparar(linha);
    return true;
}

/**
 * Como funciona: repete até dar certo.
 *  1. Lê uma linha. Se a entrada acabou, desiste (`false`).
 *  2. Linha vazia: usa o `padrao`, se existir.
 *  3. Se não for número, explica e pergunta de novo.
 *  4. Se o número estiver fora da faixa, diz qual é a faixa e pergunta de novo.
 */
bool lerInteiro(const std::string& pergunta, int& valor, int minimo, int maximo, const int* padrao) {
    while (true) {
        std::string texto;
        if (!lerLinha(pergunta, texto)) {
            return false;
        }
        if (texto.empty() && padrao != nullptr) {
            valor = *padrao;
            return true;
        }
        int lido = 0;
        if (!converterInteiro(texto, lido)) {
            std::cout << "Erro: \"" << texto << "\" nao e um numero inteiro. Digite so numeros, sem letras ou virgula." << std::endl;
            continue;
        }
        if (lido < minimo || lido > maximo) {
            std::cout << "Erro: digite um numero entre " << minimo << " e " << maximo << "." << std::endl;
            continue;
        }
        valor = lido;
        return true;
    }
}
