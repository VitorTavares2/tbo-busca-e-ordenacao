#include <iostream>
#include <chrono>
#include "modelos.h"
#include "carregador.h"
#include "indice.h"
#include "cache.h"

struct Filtro {
    std::string tipoCampo;
    std::string valor;
};

std::vector<Filtro> lerFiltros() {
    int quantidade;
    std::cout << "Quantos filtros? ";
    std::cin >> quantidade;

    std::vector<Filtro> filtros;
    for (int i = 0; i < quantidade; i++) {
        Filtro filtro;
        std::cout << "Filtro " << (i + 1) << " - tipo ou genero (T/G)? ";
        std::string campo;
        std::cin >> campo;
        filtro.tipoCampo = (campo == "T" || campo == "t") ? "tipo" : "genero";

        std::cout << "Valor: ";
        std::cin >> filtro.valor;

        filtros.push_back(filtro);
    }
    return filtros;
}

std::string operadorEscolhido(int quantidadeFiltros) {
    if (quantidadeFiltros <= 1) {
        return "E";
    }
    std::cout << "Combinar filtros com E ou OU? ";
    std::string operador;
    std::cin >> operador;
    return operador;
}

std::string montarChaveCache(const std::vector<Filtro>& filtros, const std::string& operador, const std::string& alvo) {
    std::string chave = alvo + "|" + operador;
    for (const Filtro& filtro : filtros) {
        chave += "|" + filtro.tipoCampo + "=" + filtro.valor;
    }
    return chave;
}

std::vector<int> aplicarFiltros(const std::vector<Filtro>& filtros, const std::string& operador,
                                 const IndiceCategoria& indiceTipo, const IndiceCategoria& indiceGenero) {
    std::vector<int> resultado;
    for (size_t i = 0; i < filtros.size(); i++) {
        const IndiceCategoria& indice = (filtros[i].tipoCampo == "tipo") ? indiceTipo : indiceGenero;
        std::vector<int> parcial = indice.buscar(filtros[i].valor);

        if (i == 0) {
            resultado = parcial;
        } else if (operador == "OU" || operador == "ou") {
            resultado = uniao(resultado, parcial);
        } else {
            resultado = intersecao(resultado, parcial);
        }
    }
    return resultado;
}

void imprimirFilmes(const std::vector<int>& indices, const std::vector<Filme>& filmes) {
    std::cout << "Total encontrado: " << indices.size() << std::endl;
    size_t limite = indices.size() < 10 ? indices.size() : 10;
    for (size_t i = 0; i < limite; i++) {
        const Filme& filme = filmes[indices[i]];
        std::cout << " - " << filme.titulo << " (" << filme.tipo << ", " << filme.ano << ")" << std::endl;
    }
}

bool cinemaExibeAlgumFilme(const Cinema& cinema, const std::vector<std::string>& filmeIdsAlvo) {
    for (const std::string& idExibido : cinema.filmeIds) {
        for (const std::string& idAlvo : filmeIdsAlvo) {
            if (idExibido == idAlvo) {
                return true;
            }
        }
    }
    return false;
}

void buscarFilmes(const std::vector<Filme>& filmes, const IndiceCategoria& indiceTipo,
                   const IndiceCategoria& indiceGenero, CacheConsultas& cache) {
    std::vector<Filtro> filtros = lerFiltros();
    std::string operador = operadorEscolhido(static_cast<int>(filtros.size()));
    std::string chave = montarChaveCache(filtros, operador, "filme");

    auto inicio = std::chrono::steady_clock::now();

    std::vector<int> resultado;
    bool veioDoCache = cache.buscar(chave, resultado);
    if (!veioDoCache) {
        resultado = aplicarFiltros(filtros, operador, indiceTipo, indiceGenero);
        cache.adicionar(chave, resultado);
    }

    auto fim = std::chrono::steady_clock::now();
    double milissegundos = std::chrono::duration<double, std::milli>(fim - inicio).count();

    imprimirFilmes(resultado, filmes);
    std::cout << "Veio do cache: " << (veioDoCache ? "sim" : "nao") << std::endl;
    std::cout << "Tempo de busca: " << milissegundos << " ms" << std::endl;
}

void buscarCinemas(const std::vector<Filme>& filmes, const std::vector<Cinema>& cinemas,
                    const IndiceCategoria& indiceTipo, const IndiceCategoria& indiceGenero, CacheConsultas& cache) {
    std::vector<Filtro> filtros = lerFiltros();
    std::string operador = operadorEscolhido(static_cast<int>(filtros.size()));
    std::string chave = montarChaveCache(filtros, operador, "cinema");

    auto inicio = std::chrono::steady_clock::now();

    std::vector<int> indicesFilmes;
    bool veioDoCache = cache.buscar(chave, indicesFilmes);
    if (!veioDoCache) {
        indicesFilmes = aplicarFiltros(filtros, operador, indiceTipo, indiceGenero);
        cache.adicionar(chave, indicesFilmes);
    }

    std::vector<std::string> filmeIdsAlvo;
    for (int indice : indicesFilmes) {
        filmeIdsAlvo.push_back(filmes[indice].id);
    }

    std::vector<int> cinemasEncontrados;
    for (size_t i = 0; i < cinemas.size(); i++) {
        if (cinemaExibeAlgumFilme(cinemas[i], filmeIdsAlvo)) {
            cinemasEncontrados.push_back(static_cast<int>(i));
        }
    }

    auto fim = std::chrono::steady_clock::now();
    double milissegundos = std::chrono::duration<double, std::milli>(fim - inicio).count();

    std::cout << "Total de cinemas encontrados: " << cinemasEncontrados.size() << std::endl;
    for (int indice : cinemasEncontrados) {
        const Cinema& cinema = cinemas[indice];
        std::cout << " - " << cinema.nome << " (R$ " << cinema.preco << ")" << std::endl;
    }
    std::cout << "Veio do cache: " << (veioDoCache ? "sim" : "nao") << std::endl;
    std::cout << "Tempo de busca: " << milissegundos << " ms" << std::endl;
}

int main() {
    auto inicioCarregamento = std::chrono::steady_clock::now();

    std::vector<Filme> filmes = carregarFilmes("dados/filmesCrop.txt");
    std::vector<Cinema> cinemas = carregarCinemas("dados/cinemas.txt");

    IndiceCategoria indiceTipo;
    IndiceCategoria indiceGenero;
    for (size_t i = 0; i < filmes.size(); i++) {
        indiceTipo.adicionar(filmes[i].tipo, static_cast<int>(i));
        for (const std::string& genero : filmes[i].generos) {
            indiceGenero.adicionar(genero, static_cast<int>(i));
        }
    }

    auto fimCarregamento = std::chrono::steady_clock::now();
    double segundosCarregamento = std::chrono::duration<double>(fimCarregamento - inicioCarregamento).count();

    std::cout << "Filmes carregados: " << filmes.size() << std::endl;
    std::cout << "Cinemas carregados: " << cinemas.size() << std::endl;
    std::cout << "Tempo de carregamento: " << segundosCarregamento << " s" << std::endl;

    CacheConsultas cache;
    int opcao = -1;

    while (opcao != 0) {
        std::cout << std::endl;
        std::cout << "1 - Buscar filmes por tipo/genero" << std::endl;
        std::cout << "2 - Buscar cinemas que exibem filmes por tipo/genero" << std::endl;
        std::cout << "0 - Sair" << std::endl;
        std::cout << "Escolha uma opcao: ";
        std::cin >> opcao;

        if (opcao == 1) {
            buscarFilmes(filmes, indiceTipo, indiceGenero, cache);
        } else if (opcao == 2) {
            buscarCinemas(filmes, cinemas, indiceTipo, indiceGenero, cache);
        }
    }

    return 0;
}
