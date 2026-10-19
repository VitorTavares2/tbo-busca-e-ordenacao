#include "entrada.h"
#include "texto.h"
#include <iostream>

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
