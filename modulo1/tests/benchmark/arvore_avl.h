#ifndef ARVORE_AVL_H
#define ARVORE_AVL_H

#include <vector>

class ArvoreAVL {
public:
    void adicionar(int valor, int indiceFilme);

    std::vector<int> buscarIntervalo(int minimo, int maximo) const;

    int quantidadeValores() const;

    int altura() const;

private:
    struct No {
        int valor;
        std::vector<int> filmes;
        int esquerda;
        int direita;
        int altura;
    };

    std::vector<No> nos;
    int raiz = -1;

    int alturaDe(int no) const;
    void atualizarAltura(int no);
    int girarDireita(int no);
    int girarEsquerda(int no);
    int inserir(int no, int valor, int indiceFilme);
    void coletar(int no, int minimo, int maximo, std::vector<int>& resultado) const;
};

#endif
