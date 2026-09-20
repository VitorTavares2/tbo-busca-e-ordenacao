#ifndef MODELOS_H
#define MODELOS_H

#include <string>
#include <vector>

struct Filme {
    std::string id;
    std::string tipo;
    std::string titulo;
    int ano;
    int duracao;
    std::vector<std::string> generos;
};

struct Cinema {
    std::string id;
    std::string nome;
    double x;
    double y;
    double preco;
    std::vector<std::string> filmeIds;
};

#endif
