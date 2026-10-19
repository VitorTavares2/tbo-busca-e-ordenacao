#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include "../../src/modelos.h"
#include "../../src/carregador.h"
#include "../../src/indice.h"
#include "../../src/arvore.h"
#include "../../src/cache.h"
#include "arvore_avl.h"
#include "alternativas.h"

using Relogio = std::chrono::steady_clock;

static double milissegundosDesde(Relogio::time_point inicio) {
    return std::chrono::duration<double, std::milli>(Relogio::now() - inicio).count();
}

template <typename Operacao>
static double medir(Operacao operacao, int repeticoes) {
    auto inicio = Relogio::now();
    for (int i = 0; i < repeticoes; i++) {
        operacao();
    }
    return milissegundosDesde(inicio) / repeticoes;
}

int main() {
    int filmesIgnorados = 0;
    int cinemasIgnorados = 0;

    auto inicio = Relogio::now();
    std::vector<Filme> filmes = carregarFilmes("dados/filmesCrop.txt", filmesIgnorados);
    double tempoFilmes = milissegundosDesde(inicio);

    inicio = Relogio::now();
    std::vector<Cinema> cinemas = carregarCinemas("dados/cinemas.txt", cinemasIgnorados);
    double tempoCinemas = milissegundosDesde(inicio);

    inicio = Relogio::now();
    resolverFilmesDosCinemas(cinemas, filmes);
    double tempoLigar = milissegundosDesde(inicio);

    IndiceCategoria indiceTipo;
    IndiceCategoria indiceGenero;
    inicio = Relogio::now();
    for (size_t i = 0; i < filmes.size(); i++) {
        indiceTipo.adicionar(filmes[i].tipo, static_cast<int>(i));
        for (const std::string& genero : filmes[i].generos) {
            indiceGenero.adicionar(genero, static_cast<int>(i));
        }
    }
    double tempoIndices = milissegundosDesde(inicio);

    ArvoreNumerica arvoreAno;
    ArvoreNumerica arvoreDuracao;
    inicio = Relogio::now();
    for (size_t i = 0; i < filmes.size(); i++) {
        arvoreAno.adicionar(filmes[i].ano, static_cast<int>(i));
        arvoreDuracao.adicionar(filmes[i].duracao, static_cast<int>(i));
    }
    double tempoBaldes = milissegundosDesde(inicio);
    inicio = Relogio::now();
    arvoreAno.montar();
    arvoreDuracao.montar();
    double tempoMontar = milissegundosDesde(inicio);

    std::cout << "carregamento\tler filmes\t" << tempoFilmes << "\n";
    std::cout << "carregamento\tler cinemas\t" << tempoCinemas << "\n";
    std::cout << "carregamento\tligar cinema-filme\t" << tempoLigar << "\n";
    std::cout << "carregamento\tindices\t" << tempoIndices << "\n";
    std::cout << "carregamento\tarvores baldes\t" << tempoBaldes << "\n";
    std::cout << "carregamento\tarvores montar\t" << tempoMontar << "\n";

    double tempoAtual = medir([&] {
        ArvoreNumerica ano;
        ArvoreNumerica duracao;
        for (size_t i = 0; i < filmes.size(); i++) {
            ano.adicionar(filmes[i].ano, static_cast<int>(i));
            duracao.adicionar(filmes[i].duracao, static_cast<int>(i));
        }
        ano.montar();
        duracao.montar();
    }, 5);
    double tempoAvl = medir([&] {
        ArvoreAVL ano;
        ArvoreAVL duracao;
        for (size_t i = 0; i < filmes.size(); i++) {
            if (filmes[i].ano >= 0) ano.adicionar(filmes[i].ano, static_cast<int>(i));
            if (filmes[i].duracao >= 0) duracao.adicionar(filmes[i].duracao, static_cast<int>(i));
        }
    }, 5);
    double tempoMergeSort = medir([&] {
        std::vector<ValorPosicao> anos;
        std::vector<ValorPosicao> duracoes;
        for (size_t i = 0; i < filmes.size(); i++) {
            if (filmes[i].ano >= 0) anos.push_back({filmes[i].ano, static_cast<int>(i)});
            if (filmes[i].duracao >= 0) duracoes.push_back({filmes[i].duracao, static_cast<int>(i)});
        }
        std::vector<ValorPosicao> auxiliar(anos.size());
        ordenarComMergeSort(anos, auxiliar, 0, static_cast<int>(anos.size()) - 1);
        auxiliar.assign(duracoes.size(), ValorPosicao{0, 0});
        ordenarComMergeSort(duracoes, auxiliar, 0, static_cast<int>(duracoes.size()) - 1);
    }, 3);
    std::cout << "montar arvore\tbaldes + montar pelo meio\t" << tempoAtual << "\n";
    std::cout << "montar arvore\tAVL\t" << tempoAvl << "\n";
    std::cout << "montar arvore\tmerge sort\t" << tempoMergeSort << "\n";

    double tempoBinaria = medir([&] { resolverFilmesDosCinemas(cinemas, filmes); }, 20);
    long long comparacoesLinear = 0;
    double tempoLinear = medir([&] {
        for (const Cinema& cinema : cinemas) {
            for (const std::string& codigo : cinema.filmeIds) {
                buscaLinearPorCodigo(filmes, codigo, comparacoesLinear);
            }
        }
    }, 1);
    std::cout << "ligar cinema-filme\tbusca binaria\t" << tempoBinaria << "\n";
    std::cout << "ligar cinema-filme\tbusca linear\t" << tempoLinear << "\t" << comparacoesLinear << " comparacoes\n";

    struct Intervalo { const char* nome; bool porAno; int minimo; int maximo; };
    std::vector<Intervalo> intervalos = {
        {"ano 2000", true, 2000, 2000}, {"duracao 90..120", false, 90, 120}, {"anos 1990..1999", true, 1990, 1999},
        {"duracao 1..59", false, 1, 59}, {"anos 1885..2026", true, 1885, 2026}};
    for (const Intervalo& caso : intervalos) {
        const ArvoreNumerica& arvore = caso.porAno ? arvoreAno : arvoreDuracao;
        size_t encontrados = 0;
        double tempoArvore = medir([&] { encontrados = arvore.buscarIntervalo(caso.minimo, caso.maximo).size(); }, 50);
        double tempoSequencial = medir([&] { buscaSequencialIntervalo(filmes, caso.porAno, caso.minimo, caso.maximo); }, 10);
        std::cout << "intervalo\t" << caso.nome << "\t" << tempoArvore << "\t" << tempoSequencial << "\t" << encontrados << " filmes\n";
    }

    struct Combinacao { const char* nome; std::string primeiro; bool primeiroTipo; std::string segundo; bool ou; };
    std::vector<Combinacao> combinacoes = {
        {"short E Animation", "short", true, "Animation", false}, {"movie E Comedy", "movie", true, "Comedy", false},
        {"tvEpisode E Drama", "tvEpisode", true, "Drama", false}, {"Comedy OU Drama", "Comedy", false, "Drama", true}};
    for (const Combinacao& caso : combinacoes) {
        std::vector<int> a = caso.primeiroTipo ? indiceTipo.buscar(caso.primeiro) : indiceGenero.buscar(caso.primeiro);
        std::vector<int> b = indiceGenero.buscar(caso.segundo);
        size_t encontrados = 0;
        double tempoNovo = medir([&] { encontrados = (caso.ou ? uniao(a, b) : intersecao(a, b)).size(); }, 20);
        double tempoAnterior = medir([&] { caso.ou ? uniaoAnterior(a, b) : intersecaoAnterior(a, b); }, 1);
        double tempoVarrer = caso.ou ? -1 : medir([&] { varrerTipoEGenero(filmes, caso.primeiro, caso.segundo); }, 5);
        std::cout << "e/ou\t" << caso.nome << "\t" << tempoNovo << "\t" << tempoAnterior << "\t" << tempoVarrer
                  << "\t" << encontrados << " filmes\n";
    }

    CacheConsultas cache;
    std::vector<int> grande = intersecao(indiceTipo.buscar("tvEpisode"), indiceGenero.buscar("Drama"));
    cache.adicionar("filme|E|tipo=tvEpisode|genero=Drama", grande);
    std::vector<int> copia;
    double tempoCache = medir([&] { cache.buscar("filme|E|tipo=tvEpisode|genero=Drama", copia); }, 50);
    std::cout << "cache\ttvEpisode E Drama do cache\t" << tempoCache << "\t" << grande.size() << " filmes\n";

    std::vector<int> todosComAno = arvoreAno.buscarIntervalo(1885, 2026);
    size_t cinemasAchados = 0;
    double tempoMarcacao = medir([&] {
        std::vector<char> procurado(filmes.size(), 0);
        for (int indice : todosComAno) {
            procurado[indice] = 1;
        }
        cinemasAchados = 0;
        for (const Cinema& cinema : cinemas) {
            for (int posicao : cinema.filmePosicoes) {
                if (posicao >= 0 && procurado[posicao]) {
                    cinemasAchados++;
                    break;
                }
            }
        }
    }, 20);
    std::cout << "cinemas\tmarcacao com 559 mil filmes\t" << tempoMarcacao << "\t" << cinemasAchados << " cinemas\n";

    for (const Intervalo& caso : intervalos) {
        ArvoreContada arvore(filmes, caso.porAno);
        std::cout << "nos visitados\t" << caso.nome << "\t" << arvore.contarVisitas(caso.minimo, caso.maximo)
                  << "\tde " << arvore.quantidade() << "\n";
    }
    return 0;
}
