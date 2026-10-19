#ifndef ENTRADA_H
#define ENTRADA_H

#include <string>

bool lerLinha(const std::string& pergunta, std::string& texto);

bool lerInteiro(const std::string& pergunta, int& valor, int minimo, int maximo, const int* padrao = nullptr);

#endif
