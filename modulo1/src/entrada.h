/**
 * @file entrada.h
 * @brief Perguntas feitas ao usuário no terminal, com conferência do que foi digitado.
 *
 * O que estas funções resolvem: se o usuário digita letras onde era esperado
 * um número, ou um número fora do que faz sentido, o programa NÃO quebra e
 * NÃO repete o menu sem parar. Ele explica o erro em uma linha que começa
 * com `Erro:` e faz a pergunta de novo.
 *
 * Quando a entrada acaba (o usuário apertou Ctrl+Z no Windows / Ctrl+D no
 * Linux, ou o programa está lendo de um arquivo que terminou), as funções
 * devolvem `false`. Quem chamou deve parar o que estava fazendo.
 */

#ifndef ENTRADA_H
#define ENTRADA_H

#include <string>

/**
 * @brief Mostra uma pergunta e lê uma linha inteira digitada pelo usuário.
 *
 * @param pergunta Texto mostrado antes da resposta. Exemplo: `"Valor: "`.
 * @param texto    Recebe o que foi digitado, já sem espaços nas pontas.
 * @return `true` se leu uma linha. `false` se a entrada acabou.
 *
 * @note A linha inteira é lida, então valores com espaço funcionam.
 */
bool lerLinha(const std::string& pergunta, std::string& texto);

/**
 * @brief Pergunta um número inteiro e insiste até receber um valor válido.
 *
 * Mostra `Erro:` e pergunta de novo quando:
 *  - o texto não é um número (ex: `abc`, `12x`, `1.5`);
 *  - o número está fora de `minimo`..`maximo`.
 *
 * @param pergunta Texto mostrado antes da resposta.
 * @param valor    Recebe o número digitado.
 * @param minimo   Menor valor aceito.
 * @param maximo   Maior valor aceito.
 * @param padrao   Opcional. Se informado, apertar só Enter (resposta vazia)
 *                 usa este valor. Se for `nullptr`, resposta vazia é erro.
 * @return `true` se `valor` foi preenchido. `false` se a entrada acabou.
 */
bool lerInteiro(const std::string& pergunta, int& valor, int minimo, int maximo, const int* padrao = nullptr);

#endif
