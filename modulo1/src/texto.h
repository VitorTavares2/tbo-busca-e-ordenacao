/**
 * @file texto.h
 * @brief Ferramentas simples para recortar e limpar textos.
 *
 * Os arquivos de dados são texto puro. Cada linha traz várias informações
 * separadas por um caractere (uma vírgula ou uma tabulação). Estas funções
 * separam essas informações e tiram espaços que sobram nas pontas.
 */

#ifndef TEXTO_H
#define TEXTO_H

#include <string>
#include <vector>

/**
 * @brief Corta um texto em pedaços sempre que encontra um separador.
 *
 * Exemplo: `dividir("a,b,c", ',')` devolve `a`, `b` e `c`.
 *
 * @param texto        O texto que será cortado.
 * @param delimitador  O caractere que separa os pedaços (vírgula, tabulação etc.).
 * @return Os pedaços, na mesma ordem em que aparecem no texto. Pedaços vazios
 *         também entram: `"a,,b"` devolve `a`, um pedaço vazio e `b`.
 *
 * @note Velocidade: lê o texto uma vez só, do começo ao fim.
 */
std::vector<std::string> dividir(const std::string& texto, char delimitador);

/**
 * @brief Tira espaços e quebras de linha do começo e do fim de um texto.
 *
 * Exemplo: `aparar("  Comedy  ")` devolve `Comedy`.
 *
 * @param texto O texto a limpar.
 * @return Uma cópia do texto, já limpa.
 *
 * @note A tabulação NÃO é removida, porque no arquivo de filmes ela é o separador.
 */
std::string aparar(const std::string& texto);

#endif
