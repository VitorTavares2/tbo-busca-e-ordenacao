/**
 * @file teste_arvore.cpp
 * @brief Testes da ArvoreNumerica (arvore.h). Cada teste mostra uma regra que a árvore precisa cumprir.
 *
 * Como rodar: `bash tests/rodar_testes.sh` (ou veja o README).
 * Se algum teste falhar, o programa diz qual foi e termina com erro.
 */

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "../src/arvore.h"

static int falhas = 0;
static int total = 0;

static void conferir(bool condicao, const std::string& nome) {
    total++;
    if (!condicao) {
        falhas++;
        std::cout << "FALHOU: " << nome << std::endl;
    }
}

static void arvore_vazia_nao_acha_nada() {
    ArvoreNumerica arvore;
    arvore.montar();
    conferir(arvore.buscarIntervalo(0, 100).empty(), "arvore vazia devolve lista vazia");
    conferir(arvore.quantidadeValores() == 0, "arvore vazia tem 0 valores");
    conferir(arvore.altura() == 0, "arvore vazia tem altura 0");
}

static void valor_exato_acha_so_aquele_valor() {
    ArvoreNumerica arvore;
    arvore.adicionar(2000, 0);
    arvore.adicionar(2001, 1);
    arvore.adicionar(1999, 2);
    arvore.montar();
    std::vector<int> achados = arvore.buscarIntervalo(2000, 2000);
    conferir(achados == std::vector<int>{0}, "valor exato devolve so o filme daquele valor");
}

static void valores_repetidos_ficam_na_mesma_entrada() {
    ArvoreNumerica arvore;
    arvore.adicionar(90, 4);
    arvore.adicionar(90, 7);
    arvore.adicionar(90, 9);
    arvore.montar();
    conferir(arvore.buscarIntervalo(90, 90) == (std::vector<int>{4, 7, 9}), "tres filmes de 90 min voltam na ordem em que entraram");
    conferir(arvore.quantidadeValores() == 1, "valor repetido conta uma vez so");
}

static void intervalo_inclui_as_duas_pontas() {
    ArvoreNumerica arvore;
    for (int valor = 10; valor <= 50; valor += 10) {
        arvore.adicionar(valor, valor);
    }
    arvore.montar();
    conferir(arvore.buscarIntervalo(20, 40) == (std::vector<int>{20, 30, 40}), "intervalo 20..40 inclui 20 e 40");
    conferir(arvore.buscarIntervalo(21, 39) == (std::vector<int>{30}), "intervalo 21..39 pega so o 30");
}

static void resultado_sai_em_ordem_crescente_do_valor() {
    ArvoreNumerica arvore;
    arvore.adicionar(300, 0);
    arvore.adicionar(100, 1);
    arvore.adicionar(200, 2);
    arvore.montar();
    conferir(arvore.buscarIntervalo(0, 1000) == (std::vector<int>{1, 2, 0}), "filmes saem do menor valor para o maior");
}

static void intervalo_sem_nada_devolve_vazio() {
    ArvoreNumerica arvore;
    arvore.adicionar(50, 0);
    arvore.adicionar(80, 1);
    arvore.montar();
    conferir(arvore.buscarIntervalo(51, 79).empty(), "intervalo entre dois valores devolve vazio");
    conferir(arvore.buscarIntervalo(500, 900).empty(), "intervalo acima de tudo devolve vazio");
    conferir(arvore.buscarIntervalo(0, 10).empty(), "intervalo abaixo de tudo devolve vazio");
}

static void intervalo_invertido_devolve_vazio() {
    ArvoreNumerica arvore;
    arvore.adicionar(50, 0);
    arvore.montar();
    conferir(arvore.buscarIntervalo(100, 10).empty(), "minimo maior que maximo devolve vazio");
}

static void valores_em_ordem_crescente_mantem_arvore_baixa() {
    ArvoreNumerica arvore;
    const int QUANTIDADE = 1000;
    for (int valor = 1; valor <= QUANTIDADE; valor++) {
        arvore.adicionar(valor, valor);
    }
    arvore.montar();
    conferir(arvore.quantidadeValores() == QUANTIDADE, "1000 valores distintos entraram");
    conferir(arvore.altura() <= 12, "altura com 1000 valores crescentes fica em 12 ou menos (sem balanceamento seria 1000)");
    conferir(arvore.buscarIntervalo(1, QUANTIDADE).size() == static_cast<size_t>(QUANTIDADE), "nenhum valor se perdeu na montagem");
}

static void valores_em_ordem_decrescente_mantem_arvore_baixa() {
    ArvoreNumerica arvore;
    const int QUANTIDADE = 1000;
    for (int valor = QUANTIDADE; valor >= 1; valor--) {
        arvore.adicionar(valor, valor);
    }
    arvore.montar();
    conferir(arvore.altura() <= 12, "altura com 1000 valores decrescentes fica em 12 ou menos");
    std::vector<int> todos = arvore.buscarIntervalo(1, QUANTIDADE);
    bool emOrdem = true;
    for (size_t i = 0; i < todos.size(); i++) {
        if (todos[i] != static_cast<int>(i) + 1) {
            emOrdem = false;
        }
    }
    conferir(emOrdem, "resultado continua em ordem crescente depois da montagem");
}

static void valores_embaralhados_nao_perdem_nada() {
    ArvoreNumerica arvore;
    const int QUANTIDADE = 5000;
    unsigned int semente = 12345;
    std::vector<int> contagem(QUANTIDADE, 0);
    for (int i = 0; i < 20000; i++) {
        semente = semente * 1103515245u + 12345u;
        int valor = static_cast<int>((semente >> 8) % QUANTIDADE);
        contagem[valor]++;
        arvore.adicionar(valor, i);
    }
    arvore.montar();
    size_t esperado = 0;
    for (int valor = 1000; valor <= 2000; valor++) {
        esperado += contagem[valor];
    }
    conferir(arvore.buscarIntervalo(1000, 2000).size() == esperado, "20000 insercoes embaralhadas: contagem do intervalo bate");
    conferir(arvore.altura() <= 15, "altura com ate 5000 valores distintos fica em 15 ou menos");
}

static void arvore_montada_pelo_meio_tem_altura_minima() {
    ArvoreNumerica arvore;
    for (int valor = 1; valor <= 1000; valor++) {
        arvore.adicionar(valor, valor);
    }
    arvore.montar();
    conferir(arvore.altura() == 10, "1000 valores montados pelo meio dao exatamente 10 andares");
    for (int valor = 1; valor <= 7; valor++) {
        ArvoreNumerica pequena;
        for (int i = 1; i <= valor; i++) {
            pequena.adicionar(i, i);
        }
        pequena.montar();
        int esperado = valor == 1 ? 1 : (valor <= 3 ? 2 : 3);
        conferir(pequena.altura() == esperado, "arvore com " + std::to_string(valor) + " valores tem a menor altura possivel");
    }
}

static void montar_de_novo_nao_duplica_nada() {
    ArvoreNumerica arvore;
    arvore.adicionar(10, 0);
    arvore.adicionar(20, 1);
    arvore.montar();
    arvore.adicionar(15, 2);
    arvore.montar();
    conferir(arvore.quantidadeValores() == 3, "montar de novo conta cada valor uma vez");
    conferir(arvore.buscarIntervalo(0, 100) == (std::vector<int>{0, 2, 1}), "montar de novo inclui o valor novo e nao repete os antigos");
}

int main() {
    arvore_vazia_nao_acha_nada();
    valor_exato_acha_so_aquele_valor();
    valores_repetidos_ficam_na_mesma_entrada();
    intervalo_inclui_as_duas_pontas();
    resultado_sai_em_ordem_crescente_do_valor();
    intervalo_sem_nada_devolve_vazio();
    intervalo_invertido_devolve_vazio();
    valores_em_ordem_crescente_mantem_arvore_baixa();
    valores_em_ordem_decrescente_mantem_arvore_baixa();
    valores_embaralhados_nao_perdem_nada();
    arvore_montada_pelo_meio_tem_altura_minima();
    montar_de_novo_nao_duplica_nada();

    std::cout << (total - falhas) << "/" << total << " verificacoes passaram" << std::endl;
    return falhas == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
