#ifndef ARVORE_H
#define ARVORE_H

#include <vector>

class ArvoreNumerica {
public:
    void adicionar(int valor, int indiceFilme);

    void montar();

    std::vector<int> buscarIntervalo(int minimo, int maximo) const;

    int quantidadeValores() const;

    int altura() const;

private:
    struct No {
        int valor;
        int esquerda;
        int direita;
    };

    std::vector<std::vector<int>> filmesPorValor;
    std::vector<No> nos;
    int raiz = -1;

    int ligar(int inicio, int fim);
    int alturaDe(int no) const;
    void coletar(int no, int minimo, int maximo, std::vector<int>& resultado) const;
};

#endif
