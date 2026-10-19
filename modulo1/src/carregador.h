#ifndef CARREGADOR_H
#define CARREGADOR_H

#include <string>
#include <vector>
#include "modelos.h"

std::vector<Filme> carregarFilmes(const std::string& caminho, int& linhasIgnoradas);

std::vector<Cinema> carregarCinemas(const std::string& caminho, int& linhasIgnoradas);

void resolverFilmesDosCinemas(std::vector<Cinema>& cinemas, const std::vector<Filme>& filmes);

#endif
