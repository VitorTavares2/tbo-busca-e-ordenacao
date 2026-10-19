#include "carregador.h"
#include "texto.h"
#include <fstream>
#include <stdexcept>

std::vector<Filme> carregarFilmes(const std::string& caminho, int& linhasIgnoradas) {
    std::vector<Filme> filmes;
    linhasIgnoradas = 0;
    std::ifstream arquivo(caminho, std::ios::binary);
    if (!arquivo.is_open()) {
        throw std::runtime_error("nao consegui abrir o arquivo de filmes: " + caminho);
    }
    std::string linha;

    std::getline(arquivo, linha);

    while (std::getline(arquivo, linha)) {
        if (aparar(linha).empty()) {
            continue;
        }

        std::vector<std::string> campos = dividir(linha, '\t');
        if (campos.size() < 9) {
            linhasIgnoradas++;
            continue;
        }
        campos[8] = aparar(campos[8]);

        Filme filme;
        filme.id = campos[0];
        filme.tipo = campos[1];
        filme.titulo = campos[2];
        filme.ano = -1;
        filme.duracao = -1;

        bool anoValido = (campos[5] == "\\N") || converterInteiro(campos[5], filme.ano);
        bool duracaoValida = (campos[7] == "\\N") || converterInteiro(campos[7], filme.duracao);
        if (!anoValido || !duracaoValida) {
            linhasIgnoradas++;
            continue;
        }

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

std::vector<Cinema> carregarCinemas(const std::string& caminho, int& linhasIgnoradas) {
    std::vector<Cinema> cinemas;
    linhasIgnoradas = 0;
    std::ifstream arquivo(caminho, std::ios::binary);
    if (!arquivo.is_open()) {
        throw std::runtime_error("nao consegui abrir o arquivo de cinemas: " + caminho);
    }
    std::string linha;

    std::getline(arquivo, linha);

    while (std::getline(arquivo, linha)) {
        if (aparar(linha).empty()) {
            continue;
        }

        std::vector<std::string> campos = dividir(linha, ',');
        if (campos.size() < 6) {
            linhasIgnoradas++;
            continue;
        }

        Cinema cinema;
        cinema.id = aparar(campos[0]);
        cinema.nome = aparar(campos[1]);
        try {
            cinema.x = std::stod(aparar(campos[2]));
            cinema.y = std::stod(aparar(campos[3]));
            cinema.preco = std::stod(aparar(campos[4]));
        } catch (const std::logic_error&) {
            linhasIgnoradas++;
            continue;
        }

        for (size_t i = 5; i < campos.size(); i++) {
            cinema.filmeIds.push_back(aparar(campos[i]));
        }

        cinemas.push_back(cinema);
    }

    return cinemas;
}

void resolverFilmesDosCinemas(std::vector<Cinema>& cinemas, const std::vector<Filme>& filmes) {
    bool emOrdem = true;
    for (size_t i = 1; i < filmes.size(); i++) {
        if (filmes[i - 1].id > filmes[i].id) {
            emOrdem = false;
            break;
        }
    }

    for (Cinema& cinema : cinemas) {
        cinema.filmePosicoes.clear();
        for (const std::string& codigo : cinema.filmeIds) {
            int posicao = -1;
            if (emOrdem) {
                int inicio = 0;
                int fim = static_cast<int>(filmes.size()) - 1;
                while (inicio <= fim) {
                    int meio = inicio + (fim - inicio) / 2;
                    if (filmes[meio].id == codigo) {
                        posicao = meio;
                        break;
                    }
                    if (filmes[meio].id < codigo) {
                        inicio = meio + 1;
                    } else {
                        fim = meio - 1;
                    }
                }
            } else {
                for (size_t i = 0; i < filmes.size(); i++) {
                    if (filmes[i].id == codigo) {
                        posicao = static_cast<int>(i);
                        break;
                    }
                }
            }
            cinema.filmePosicoes.push_back(posicao);
        }
    }
}
