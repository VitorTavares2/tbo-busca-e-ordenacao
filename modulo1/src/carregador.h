#ifndef CARREGADOR_H
#define CARREGADOR_H

#include <string>
#include <vector>
#include "modelos.h"

std::vector<Filme> carregarFilmes(const std::string& caminho);
std::vector<Cinema> carregarCinemas(const std::string& caminho);

#endif
