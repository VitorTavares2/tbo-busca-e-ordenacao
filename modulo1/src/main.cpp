#include <cctype>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include "modelos.h"
#include "carregador.h"
#include "indice.h"
#include "arvore.h"
#include "cache.h"
#include "entrada.h"

const size_t LIMITE_DE_LINHAS = 10;

const int MAXIMO_DE_FILTROS = 10;

const int ANO_MINIMO = 0;
const int ANO_MAXIMO = 9999;

const int DURACAO_MINIMA = 0;
const int DURACAO_MAXIMA = 99999;

struct Filtro {
    std::string tipoCampo;
    std::string valor;
};

std::string paraMaiusculas(std::string texto) {
    for (char& letra : texto) {
        letra = static_cast<char>(std::toupper(static_cast<unsigned char>(letra)));
    }
    return texto;
}

double milissegundosDesde(std::chrono::steady_clock::time_point inicio) {
    return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - inicio).count();
}

bool lerFiltros(std::vector<Filtro>& filtros) {
    int quantidade = 0;
    if (!lerInteiro("Quantos filtros? ", quantidade, 1, MAXIMO_DE_FILTROS)) {
        return false;
    }

    for (int i = 0; i < quantidade; i++) {
        Filtro filtro;
        std::string campo;
        while (true) {
            if (!lerLinha("Filtro " + std::to_string(i + 1) + " - tipo ou genero (T/G)? ", campo)) {
                return false;
            }
            campo = paraMaiusculas(campo);
            if (campo == "T" || campo == "G") {
                break;
            }
            std::cout << "Erro: digite T (para tipo) ou G (para genero)." << std::endl;
        }
        filtro.tipoCampo = (campo == "T") ? "tipo" : "genero";

        while (true) {
            if (!lerLinha("Valor: ", filtro.valor)) {
                return false;
            }
            if (!filtro.valor.empty()) {
                break;
            }
            std::cout << "Erro: o valor nao pode ficar vazio. Exemplo: movie (tipo) ou Comedy (genero)." << std::endl;
        }

        filtros.push_back(filtro);
    }
    return true;
}

bool operadorEscolhido(int quantidadeFiltros, std::string& operador) {
    if (quantidadeFiltros <= 1) {
        operador = "E";
        return true;
    }
    while (true) {
        std::string resposta;
        if (!lerLinha("Combinar filtros com E ou OU? ", resposta)) {
            return false;
        }
        resposta = paraMaiusculas(resposta);
        if (resposta == "E" || resposta == "OU") {
            operador = resposta;
            return true;
        }
        std::cout << "Erro: digite E ou OU." << std::endl;
    }
}

std::string montarChaveCache(const std::vector<Filtro>& filtros, const std::string& operador, const std::string& alvo) {
    std::string chave = alvo + "|" + operador;
    for (const Filtro& filtro : filtros) {
        chave += "|" + filtro.tipoCampo + "=" + filtro.valor;
    }
    return chave;
}

std::string montarChaveIntervalo(const std::string& alvo, const std::string& campo, int minimo, int maximo) {
    return alvo + "|" + campo + "=" + std::to_string(minimo) + ".." + std::to_string(maximo);
}

std::vector<int> aplicarFiltros(const std::vector<Filtro>& filtros, const std::string& operador,
                                 const IndiceCategoria& indiceTipo, const IndiceCategoria& indiceGenero) {
    std::vector<int> resultado;
    for (size_t i = 0; i < filtros.size(); i++) {
        const IndiceCategoria& indice = (filtros[i].tipoCampo == "tipo") ? indiceTipo : indiceGenero;
        std::vector<int> parcial = indice.buscar(filtros[i].valor);

        if (i == 0) {
            resultado = parcial;
        } else if (operador == "OU") {
            resultado = uniao(resultado, parcial);
        } else {
            resultado = intersecao(resultado, parcial);
        }
    }
    return resultado;
}

std::string numeroOuInterrogacao(int numero) {
    return numero < 0 ? "?" : std::to_string(numero);
}

void imprimirFilmes(const std::vector<int>& indices, const std::vector<Filme>& filmes, const std::string& dica) {
    std::cout << "Total encontrado: " << indices.size() << std::endl;
    if (indices.empty()) {
        std::cout << dica << std::endl;
        return;
    }
    size_t limite = indices.size() < LIMITE_DE_LINHAS ? indices.size() : LIMITE_DE_LINHAS;
    for (size_t i = 0; i < limite; i++) {
        const Filme& filme = filmes[indices[i]];
        std::cout << " - " << filme.titulo << " (" << filme.tipo << ", ano " << numeroOuInterrogacao(filme.ano)
                  << ", " << numeroOuInterrogacao(filme.duracao) << " min)" << std::endl;
    }
    if (indices.size() > limite) {
        std::cout << "   ... e mais " << (indices.size() - limite) << " (so os " << LIMITE_DE_LINHAS << " primeiros aparecem)" << std::endl;
    }
}

std::vector<int> cinemasQueExibem(const std::vector<int>& indicesFilmes, size_t totalFilmes, const std::vector<Cinema>& cinemas) {
    std::vector<char> procurado(totalFilmes, 0);
    for (int indice : indicesFilmes) {
        procurado[indice] = 1;
    }

    std::vector<int> encontrados;
    for (size_t i = 0; i < cinemas.size(); i++) {
        for (int posicao : cinemas[i].filmePosicoes) {
            if (posicao >= 0 && procurado[posicao]) {
                encontrados.push_back(static_cast<int>(i));
                break;
            }
        }
    }
    return encontrados;
}

void imprimirCinemas(const std::vector<int>& indices, const std::vector<Cinema>& cinemas, const std::string& dica) {
    std::cout << "Total de cinemas encontrados: " << indices.size() << std::endl;
    if (indices.empty()) {
        std::cout << dica << std::endl;
        return;
    }
    size_t limite = indices.size() < LIMITE_DE_LINHAS ? indices.size() : LIMITE_DE_LINHAS;
    for (size_t i = 0; i < limite; i++) {
        const Cinema& cinema = cinemas[indices[i]];
        std::cout << " - " << cinema.nome << " (R$ " << std::fixed << std::setprecision(2) << cinema.preco << ")" << std::endl;
        std::cout.unsetf(std::ios::fixed);
    }
    if (indices.size() > limite) {
        std::cout << "   ... e mais " << (indices.size() - limite) << " (so os " << LIMITE_DE_LINHAS << " primeiros aparecem)" << std::endl;
    }
}

void imprimirRodape(bool veioDoCache, double milissegundos) {
    std::cout << "Veio do cache: " << (veioDoCache ? "sim" : "nao") << std::endl;
    std::cout << "Tempo de busca: " << std::fixed << std::setprecision(3) << milissegundos << " ms" << std::endl;
    std::cout.unsetf(std::ios::fixed);
}

void buscarFilmes(const std::vector<Filme>& filmes, const IndiceCategoria& indiceTipo,
                   const IndiceCategoria& indiceGenero, CacheConsultas& cache) {
    std::vector<Filtro> filtros;
    std::string operador;
    if (!lerFiltros(filtros) || !operadorEscolhido(static_cast<int>(filtros.size()), operador)) {
        return;
    }
    std::string chave = montarChaveCache(filtros, operador, "filme");

    auto inicio = std::chrono::steady_clock::now();

    std::vector<int> resultado;
    bool veioDoCache = cache.buscar(chave, resultado);
    if (!veioDoCache) {
        resultado = aplicarFiltros(filtros, operador, indiceTipo, indiceGenero);
        cache.adicionar(chave, resultado);
    }

    double milissegundos = milissegundosDesde(inicio);

    imprimirFilmes(resultado, filmes, "Nenhum resultado. Dica: maiusculas e minusculas importam (use Comedy, nao comedy).");
    imprimirRodape(veioDoCache, milissegundos);
}

void buscarCinemas(const std::vector<Filme>& filmes, const std::vector<Cinema>& cinemas,
                    const IndiceCategoria& indiceTipo, const IndiceCategoria& indiceGenero, CacheConsultas& cache) {
    std::vector<Filtro> filtros;
    std::string operador;
    if (!lerFiltros(filtros) || !operadorEscolhido(static_cast<int>(filtros.size()), operador)) {
        return;
    }
    std::string chave = montarChaveCache(filtros, operador, "cinema");

    auto inicio = std::chrono::steady_clock::now();

    std::vector<int> indicesFilmes;
    bool veioDoCache = cache.buscar(chave, indicesFilmes);
    if (!veioDoCache) {
        indicesFilmes = aplicarFiltros(filtros, operador, indiceTipo, indiceGenero);
        cache.adicionar(chave, indicesFilmes);
    }

    std::vector<int> cinemasEncontrados = cinemasQueExibem(indicesFilmes, filmes.size(), cinemas);

    double milissegundos = milissegundosDesde(inicio);

    imprimirCinemas(cinemasEncontrados, cinemas, "Nenhum cinema exibe filmes com esses filtros. Dica: maiusculas e minusculas importam (use Comedy, nao comedy).");
    imprimirRodape(veioDoCache, milissegundos);
}

bool lerIntervalo(const std::string& perguntaMinimo, const std::string& perguntaMaximo,
                  int limiteInferior, int limiteSuperior, int& minimo, int& maximo) {
    while (true) {
        if (!lerInteiro(perguntaMinimo, minimo, limiteInferior, limiteSuperior)) {
            return false;
        }
        if (!lerInteiro(perguntaMaximo, maximo, limiteInferior, limiteSuperior, &minimo)) {
            return false;
        }
        if (minimo <= maximo) {
            return true;
        }
        std::cout << "Erro: o valor minimo (" << minimo << ") nao pode ser maior que o maximo (" << maximo
                  << "). Digite os dois de novo." << std::endl;
    }
}

std::vector<int> buscarIntervaloComCache(const ArvoreNumerica& arvore, const std::string& chave, int minimo, int maximo,
                                          CacheConsultas& cache, bool& veioDoCache) {
    std::vector<int> resultado;
    veioDoCache = cache.buscar(chave, resultado);
    if (!veioDoCache) {
        resultado = arvore.buscarIntervalo(minimo, maximo);
        cache.adicionar(chave, resultado);
    }
    return resultado;
}

void buscarPorDuracao(const std::vector<Filme>& filmes, const ArvoreNumerica& arvoreDuracao, CacheConsultas& cache) {
    int minimo = 0;
    int maximo = 0;
    if (!lerIntervalo("Duracao minima (em minutos): ", "Duracao maxima (Enter = igual a minima): ",
                      DURACAO_MINIMA, DURACAO_MAXIMA, minimo, maximo)) {
        return;
    }

    auto inicio = std::chrono::steady_clock::now();
    bool veioDoCache = false;
    std::vector<int> resultado = buscarIntervaloComCache(arvoreDuracao, montarChaveIntervalo("filme", "duracao", minimo, maximo),
                                                         minimo, maximo, cache, veioDoCache);
    double milissegundos = milissegundosDesde(inicio);

    imprimirFilmes(resultado, filmes, "Nenhum filme com essa duracao. Dica: filmes sem duracao informada no arquivo nao entram na busca.");
    imprimirRodape(veioDoCache, milissegundos);
}

void buscarPorAno(const std::vector<Filme>& filmes, const ArvoreNumerica& arvoreAno, CacheConsultas& cache) {
    int minimo = 0;
    int maximo = 0;
    if (!lerIntervalo("Ano inicial: ", "Ano final (Enter = mesmo ano): ", ANO_MINIMO, ANO_MAXIMO, minimo, maximo)) {
        return;
    }

    auto inicio = std::chrono::steady_clock::now();
    bool veioDoCache = false;
    std::vector<int> resultado = buscarIntervaloComCache(arvoreAno, montarChaveIntervalo("filme", "ano", minimo, maximo),
                                                         minimo, maximo, cache, veioDoCache);
    double milissegundos = milissegundosDesde(inicio);

    imprimirFilmes(resultado, filmes, "Nenhum filme nesse periodo. Dica: a base tem filmes de 1885 a 2026.");
    imprimirRodape(veioDoCache, milissegundos);
}

void buscarCinemasPorAno(const std::vector<Filme>& filmes, const std::vector<Cinema>& cinemas,
                          const ArvoreNumerica& arvoreAno, CacheConsultas& cache) {
    int minimo = 0;
    int maximo = 0;
    if (!lerIntervalo("Ano inicial: ", "Ano final (Enter = mesmo ano): ", ANO_MINIMO, ANO_MAXIMO, minimo, maximo)) {
        return;
    }

    auto inicio = std::chrono::steady_clock::now();
    bool veioDoCache = false;
    std::vector<int> indicesFilmes = buscarIntervaloComCache(arvoreAno, montarChaveIntervalo("cinema", "ano", minimo, maximo),
                                                             minimo, maximo, cache, veioDoCache);
    std::vector<int> cinemasEncontrados = cinemasQueExibem(indicesFilmes, filmes.size(), cinemas);
    double milissegundos = milissegundosDesde(inicio);

    imprimirCinemas(cinemasEncontrados, cinemas, "Nenhum cinema exibe filmes desse periodo.");
    imprimirRodape(veioDoCache, milissegundos);
}

int main() {
    auto inicioCarregamento = std::chrono::steady_clock::now();

    std::vector<Filme> filmes;
    std::vector<Cinema> cinemas;
    int filmesIgnorados = 0;
    int cinemasIgnorados = 0;
    try {
        filmes = carregarFilmes("dados/filmesCrop.txt", filmesIgnorados);
        cinemas = carregarCinemas("dados/cinemas.txt", cinemasIgnorados);
    } catch (const std::runtime_error& problema) {
        std::cout << "Erro: " << problema.what() << std::endl;
        std::cout << "Dica: execute o programa de dentro da pasta modulo1, onde existe a pasta dados." << std::endl;
        return 1;
    }
    resolverFilmesDosCinemas(cinemas, filmes);

    IndiceCategoria indiceTipo;
    IndiceCategoria indiceGenero;
    ArvoreNumerica arvoreAno;
    ArvoreNumerica arvoreDuracao;
    for (size_t i = 0; i < filmes.size(); i++) {
        int posicao = static_cast<int>(i);
        indiceTipo.adicionar(filmes[i].tipo, posicao);
        for (const std::string& genero : filmes[i].generos) {
            indiceGenero.adicionar(genero, posicao);
        }
        if (filmes[i].ano >= 0) {
            arvoreAno.adicionar(filmes[i].ano, posicao);
        }
        if (filmes[i].duracao >= 0) {
            arvoreDuracao.adicionar(filmes[i].duracao, posicao);
        }
    }
    arvoreAno.montar();
    arvoreDuracao.montar();

    double segundosCarregamento = milissegundosDesde(inicioCarregamento) / 1000.0;

    std::cout << "Filmes carregados: " << filmes.size() << std::endl;
    std::cout << "Cinemas carregados: " << cinemas.size() << std::endl;
    if (filmesIgnorados > 0 || cinemasIgnorados > 0) {
        std::cout << "Aviso: linhas com defeito foram puladas (filmes: " << filmesIgnorados
                  << ", cinemas: " << cinemasIgnorados << ")." << std::endl;
    }
    std::cout << "Arvore de anos: " << arvoreAno.quantidadeValores() << " anos diferentes (altura " << arvoreAno.altura() << ")" << std::endl;
    std::cout << "Arvore de duracoes: " << arvoreDuracao.quantidadeValores() << " duracoes diferentes (altura " << arvoreDuracao.altura() << ")" << std::endl;
    std::cout << "Tempo de carregamento: " << segundosCarregamento << " s" << std::endl;

    CacheConsultas cache;
    int opcao = -1;

    while (opcao != 0) {
        std::cout << std::endl;
        std::cout << "1 - Buscar filmes por tipo/genero" << std::endl;
        std::cout << "2 - Buscar cinemas que exibem filmes por tipo/genero" << std::endl;
        std::cout << "3 - Buscar filmes por duracao (em minutos)" << std::endl;
        std::cout << "4 - Buscar filmes por ano de lancamento" << std::endl;
        std::cout << "5 - Buscar cinemas que exibem filmes lancados em um ano ou periodo" << std::endl;
        std::cout << "0 - Sair" << std::endl;
        if (!lerInteiro("Escolha uma opcao: ", opcao, 0, 5)) {
            std::cout << "Entrada encerrada. Ate logo." << std::endl;
            break;
        }

        if (opcao == 1) {
            buscarFilmes(filmes, indiceTipo, indiceGenero, cache);
        } else if (opcao == 2) {
            buscarCinemas(filmes, cinemas, indiceTipo, indiceGenero, cache);
        } else if (opcao == 3) {
            buscarPorDuracao(filmes, arvoreDuracao, cache);
        } else if (opcao == 4) {
            buscarPorAno(filmes, arvoreAno, cache);
        } else if (opcao == 5) {
            buscarCinemasPorAno(filmes, cinemas, arvoreAno, cache);
        }
    }

    return 0;
}
