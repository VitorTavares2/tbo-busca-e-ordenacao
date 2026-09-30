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

/**
 * @brief Transforma um texto em número inteiro, só se o texto for um número de verdade.
 *
 * Exemplos: `"90"` vira 90, `" -5 "` vira -5 (os espaços das pontas são ignorados).
 * Não são aceitos: `"abc"`, `"12abc"`, `"1.5"`, texto vazio e `"-"`.
 *
 * @param texto O texto digitado ou lido do arquivo.
 * @param valor Recebe o número quando a conversão dá certo. Fica como estava quando dá errado.
 * @return `true` se o texto era um número inteiro, `false` caso contrário.
 *
 * @note Números com mais de 9 dígitos são recusados. Isso evita estourar o tamanho de um `int`.
 */
bool converterInteiro(const std::string& texto, int& valor);

#endif
