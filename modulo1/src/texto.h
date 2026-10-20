#ifndef TEXTO_H
#define TEXTO_H

#include <string>
#include <vector>

std::vector<std::string> dividir(const std::string& texto, char delimitador);

std::string aparar(const std::string& texto);

bool converterInteiro(const std::string& texto, int& valor);

std::string minusculas(std::string texto);

#endif
