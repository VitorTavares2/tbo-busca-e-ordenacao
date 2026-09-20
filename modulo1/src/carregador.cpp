#include "carregador.h"
#include "texto.h"
#include <fstream>

std::vector<Filme> carregarFilmes(const std::string& caminho) {
    std::vector<Filme> filmes;
    std::ifstream arquivo(caminho);
    std::string linha;

    std::getline(arquivo, linha);

    while (std::getline(arquivo, linha)) {
        if (linha.empty()) {
            continue;
        }

        std::vector<std::string> campos = dividir(linha, '\t');
        if (campos.size() < 9) {
            continue;
        }

        Filme filme;
        filme.id = campos[0];
        filme.tipo = campos[1];
        filme.titulo = campos[2];

        filme.ano = (campos[5] == "\\N") ? -1 : std::stoi(campos[5]);
        filme.duracao = (campos[7] == "\\N") ? -1 : std::stoi(campos[7]);

        if (campos[8] != "\\N") {
            filme.generos = dividir(campos[8], ',');
            for (std::string& genero : filme.generos) {
                genero = aparar(genero);
            }
        }

        filmes.push_back(filme);
    }

    return filmes;
}

std::vector<Cinema> carregarCinemas(const std::string& caminho) {
    std::vector<Cinema> cinemas;
    std::ifstream arquivo(caminho);
    std::string linha;

    std::getline(arquivo, linha);

    while (std::getline(arquivo, linha)) {
        if (linha.empty()) {
            continue;
        }

        std::vector<std::string> campos = dividir(linha, ',');
        if (campos.size() < 6) {
            continue;
        }

        Cinema cinema;
        cinema.id = aparar(campos[0]);
        cinema.nome = aparar(campos[1]);
        cinema.x = std::stod(aparar(campos[2]));
        cinema.y = std::stod(aparar(campos[3]));
        cinema.preco = std::stod(aparar(campos[4]));

        for (size_t i = 5; i < campos.size(); i++) {
            cinema.filmeIds.push_back(aparar(campos[i]));
        }

        cinemas.push_back(cinema);
    }

    return cinemas;
}
