#!/usr/bin/env bash
# Roda todos os testes do Modulo 1 e 2 e grava o resultado em tests/logs/.
#
# Uso (dentro da pasta modulo1):   bash tests/rodar_testes.sh
#
# Etapas:
#   1. Compila o programa e o teste da arvore.
#   2. Testes da arvore (unitarios).
#   3. Modulo 1 igual ao que era antes (comparacao com tests/golden).
#   4. Tabela de casos (uma consulta por caso, resposta conferida com awk).
#   5. Erros de digitacao e arquivos faltando.
#   6. Casos em cascata (varias consultas na mesma sessao, uma dependendo da outra).
#   7. Volume (centenas de consultas em uma sessao, com tempo de cada uma).
#
# O "esperado" de cada caso NAO vem do programa. Vem de contas feitas com awk
# direto nos arquivos de dados. Assim, se o programa errar, o teste percebe.

set +u
cd "$(dirname "$0")/.."

PASTA_LOGS=tests/logs
LOG=$PASTA_LOGS/ultima_execucao.log
VOLUME_TSV=$PASTA_LOGS/volume_consultas.tsv
FILMES=dados/filmesCrop.txt
CINEMAS=dados/cinemas.txt
EXE=build/busca_teste.exe
TEMPO_MAXIMO_SESSAO=120

mkdir -p build "$PASTA_LOGS"
: > "$LOG"

TOTAL=0
FALHAS=0

escrever() { echo "$*" | tee -a "$LOG"; }

registrar() {
    local situacao=$1 nome=$2 esperado=$3 obtido=$4 tempo=${5:--}
    TOTAL=$((TOTAL + 1))
    if [ "$situacao" != OK ]; then FALHAS=$((FALHAS + 1)); fi
    printf '%-6s | %-52s | esperado=%-8s | obtido=%-8s | %s\n' "$situacao" "$nome" "$esperado" "$obtido" "$tempo" | tee -a "$LOG"
}

conferir() {
    local nome=$1 esperado=$2 obtido=$3 tempo=${4:--}
    if [ "$esperado" = "$obtido" ]; then registrar OK "$nome" "$esperado" "$obtido" "$tempo"
    else registrar FALHOU "$nome" "$esperado" "$obtido" "$tempo"; fi
}

conter() {
    local nome=$1 texto=$2 trecho=$3
    if printf '%s' "$texto" | grep -qF -- "$trecho"; then registrar OK "$nome" "contem '$trecho'" "sim"
    else registrar FALHOU "$nome" "contem '$trecho'" "nao"; fi
}

rodar() { printf '%b' "$1" | timeout "$TEMPO_MAXIMO_SESSAO" "$EXE" 2>&1 | tr -d '\r'; }

totais() { grep -oE 'Total (de cinemas )?encontrados?: [0-9]+' <<<"$1" | grep -oE '[0-9]+$'; }
tempos() { grep -oE 'Tempo de busca: [0-9.e+-]+' <<<"$1" | grep -oE '[0-9.e+-]+$'; }
ultimo_total() { totais "$1" | tail -n 1; }

escrever "=== Testes Modulo 1 e 2 - $(date '+%Y-%m-%d %H:%M:%S') ==="
escrever "Compilador: $(g++ --version | head -n 1)"
escrever ""

escrever "--- 1. Compilacao ---"
if ! g++ -O2 -std=c++17 -Wall -Wextra -static src/*.cpp -o "$EXE" 2>&1 | tee -a "$LOG" | grep -q .; then
    registrar OK "programa compila sem avisos" "0 avisos" "0 avisos"
else
    registrar FALHOU "programa compila sem avisos" "0 avisos" "ver log"
fi
if [ ! -x "$EXE" ]; then escrever "Programa nao compilou. Parando."; exit 1; fi
g++ -O2 -std=c++17 -Wall -Wextra -static tests/teste_arvore.cpp src/arvore.cpp -o build/teste_arvore.exe 2>&1 | tee -a "$LOG"

escrever ""
escrever "--- 2. Arvore (unitarios) ---"
RESULTADO_ARVORE=$(./build/teste_arvore.exe)
escrever "$RESULTADO_ARVORE"
if [ $? -eq 0 ] && ! grep -q FALHOU <<<"$RESULTADO_ARVORE"; then registrar OK "testes da arvore" "sem falhas" "sem falhas"
else registrar FALHOU "testes da arvore" "sem falhas" "com falhas"; fi

escrever ""
escrever "--- Contas de referencia (awk direto nos arquivos) ---"
awk -F'\t' 'NR>1 && NF>=9 && $6 ~ /^[0-9]+$/ {c[$6+0]++} END{for(k in c) print k, c[k]}' "$FILMES" | sort -n > build/cont_ano.txt
awk -F'\t' 'NR>1 && NF>=9 && $8 ~ /^[0-9]+$/ {c[$8+0]++} END{for(k in c) print k, c[k]}' "$FILMES" | sort -n > build/cont_dur.txt
escrever "anos diferentes: $(wc -l < build/cont_ano.txt) | duracoes diferentes: $(wc -l < build/cont_dur.txt)"

soma_faixa() { awk -v lo="$2" -v hi="$3" '$1>=lo && $1<=hi {n+=$2} END{print n+0}' "$1"; }
esperado_ano() { soma_faixa build/cont_ano.txt "$1" "$2"; }
esperado_duracao() { soma_faixa build/cont_dur.txt "$1" "$2"; }

esperado_cinemas_ano() {
    awk -F'\t' -v lo="$1" -v hi="$2" -v arquivoCinemas="$CINEMAS" '
        NR>1 && NF>=9 && $6 ~ /^[0-9]+$/ { ano[$1]=$6+0 }
        END {
            FS=","
            while ((getline linha < arquivoCinemas) > 0) {
                linhaNum++
                if (linhaNum==1) continue
                gsub(/\r/, "", linha)
                n=split(linha, p, ",")
                achou=0
                for (i=6; i<=n; i++) { id=p[i]; gsub(/ /, "", id); if ((id in ano) && ano[id]>=lo && ano[id]<=hi) { achou=1; break } }
                total+=achou
            }
            print total+0
        }' "$FILMES"
}

escrever ""
escrever "--- 3. Modulo 1 continua igual ao de antes ---"
SAIDA_M1=$(rodar "$(tr -d '\r' < tests/golden/modulo1_entrada.txt | sed 's/$/\\n/' | tr -d '\n')")
normalizar() {
    tr -d '\r' | grep -E '^ - |Total' | sed -E 's/^.*Total/Total/; s/ \(.*$//' \
        | awk '/^Total/ {n=0; print; next} {n++; if (n<=10) print}'
}
if diff <(normalizar <<<"$SAIDA_M1") <(normalizar < tests/golden/modulo1_saida_base.txt) > build/diff_m1.txt; then
    registrar OK "8 consultas do modulo 1 iguais a saida de referencia" "0 diferencas" "0 diferencas"
else
    registrar FALHOU "8 consultas do modulo 1 iguais a saida de referencia" "0 diferencas" "$(wc -l < build/diff_m1.txt) linhas diferentes"
    head -n 10 build/diff_m1.txt | tee -a "$LOG"
fi

escrever ""
escrever "--- 4. Tabela de casos (1 consulta por caso) ---"
caso_ano() {
    local saida; saida=$(rodar "4\n$1\n$2\n0\n")
    conferir "filmes por ano $1..$2" "$(esperado_ano "$1" "$2")" "$(ultimo_total "$saida")" "$(tempos "$saida" | tail -n 1) ms"
}
caso_duracao() {
    local saida; saida=$(rodar "3\n$1\n$2\n0\n")
    conferir "filmes por duracao $1..$2 min" "$(esperado_duracao "$1" "$2")" "$(ultimo_total "$saida")" "$(tempos "$saida" | tail -n 1) ms"
}
caso_cinemas_ano() {
    local saida; saida=$(rodar "5\n$1\n$2\n0\n")
    conferir "cinemas por ano $1..$2" "$(esperado_cinemas_ano "$1" "$2")" "$(ultimo_total "$saida")" "$(tempos "$saida" | tail -n 1) ms"
}

caso_ano 2000 2000
caso_ano 1885 1885
caso_ano 2026 2026
caso_ano 1990 1999
caso_ano 1885 2026
caso_ano 1800 1850
caso_ano 2100 2200
caso_duracao 90 90
caso_duracao 0 0
caso_duracao 90 120
caso_duracao 1 59
caso_duracao 180 60000
caso_duracao 99000 99999
caso_cinemas_ano 2000 2000
caso_cinemas_ano 1990 2010
caso_cinemas_ano 1800 1850
caso_cinemas_ano 1885 2026

SAIDA=$(rodar "4\n2000\n\n0\n")
conferir "ano final vazio = ano exato (2000)" "$(esperado_ano 2000 2000)" "$(ultimo_total "$SAIDA")"

escrever ""
escrever "--- 5. Erros de digitacao e arquivos faltando ---"
SAIDA=$(rodar "abc\n0\n");               conter "letras no menu nao travam, mostram erro" "$SAIDA" "Erro"
SAIDA=$(rodar "9\n0\n");                 conter "opcao fora do menu mostra erro" "$SAIDA" "Erro"
SAIDA=$(rodar "3\nabc\n90\n100\n0\n");   conter "duracao com letras pede de novo" "$SAIDA" "Erro"
conferir "duracao com letras: depois aceita 90..100" "$(esperado_duracao 90 100)" "$(ultimo_total "$SAIDA")"
SAIDA=$(rodar "3\n-5\n90\n100\n0\n");    conter "duracao negativa mostra erro" "$SAIDA" "Erro"
SAIDA=$(rodar "3\n120\n90\n90\n120\n0\n"); conter "minimo maior que maximo mostra erro" "$SAIDA" "minimo"
conferir "minimo>maximo: depois aceita 90..120" "$(esperado_duracao 90 120)" "$(ultimo_total "$SAIDA")"
SAIDA=$(rodar "4\n99999\n2000\n2000\n0\n"); conter "ano gigante mostra erro" "$SAIDA" "Erro"
SAIDA=$(rodar "1\n2\nT\nmovie\nG\nComedy\nXYZ\nE\n0\n"); conter "operador invalido mostra erro" "$SAIDA" "Erro"
conferir "operador invalido: depois aceita E (3382)" "3382" "$(ultimo_total "$SAIDA")"
SAIDA=$(rodar "1\n0\n1\n1\nX\nG\nComedy\n0\n"); conter "zero filtros mostra erro" "$SAIDA" "Erro"
SAIDA=$(rodar "1\n1\nG\n\\\\N\n0\n");    conferir "genero \\N (sem genero) nao vira categoria" "0" "$(ultimo_total "$SAIDA")"
SAIDA=$(rodar "1\n1\nG\ncomedy\n0\n");   conter "0 resultados explica maiusculas/minusculas" "$SAIDA" "maiusculas"
SAIDA=$(printf '3\n90\n' | timeout 30 "$EXE" 2>&1 | tr -d '\r'; echo "codigo=${PIPESTATUS[1]}")
conter "entrada acaba no meio: termina sem travar" "$SAIDA" "codigo=0"
EXE_ABSOLUTO="$PWD/$EXE"
SAIDA=$(cd /tmp && printf '0\n' | "$EXE_ABSOLUTO" 2>&1; echo "codigo=$?")
conter "sem a pasta dados: mostra erro claro" "$SAIDA" "Erro"
conter "sem a pasta dados: termina com codigo 1" "$SAIDA" "codigo=1"

PASTA_CRLF=build/dados_crlf
rm -rf "$PASTA_CRLF"; mkdir -p "$PASTA_CRLF/dados"
printf 'tconst\ttitleType\tprimaryTitle\toriginalTitle\tisAdult\tstartYear\tendYear\truntimeMinutes\tgenres\r\n' > "$PASTA_CRLF/dados/filmesCrop.txt"
printf 'tt0000001\tmovie\tUm\tUm\t0\t2001\t\\N\t100\tDrama,Comedy\r\n'  >> "$PASTA_CRLF/dados/filmesCrop.txt"
printf 'tt0000002\tmovie\tDois\tDois\t0\t\\N\t\\N\t\\N\t\\N\r\n'       >> "$PASTA_CRLF/dados/filmesCrop.txt"
printf 'tt0000003\tmovie\tTres\tTres\t0\tabcd\t\\N\t90\tDrama\r\n'      >> "$PASTA_CRLF/dados/filmesCrop.txt"
printf 'tt0000004\tmovie\tQuatro\tQuatro\t0\t2001\t\\N\t100\tDrama\r\n' >> "$PASTA_CRLF/dados/filmesCrop.txt"
printf 'tt0000005\tmovie\tCinco\r\n' >> "$PASTA_CRLF/dados/filmesCrop.txt"
printf 'Cinema_ID, Nome, X, Y, Preco, Filmes\r\n' > "$PASTA_CRLF/dados/cinemas.txt"
printf 'cc1, Bom, 1, 2, 10.50, tt0000001, tt0000004\r\n' >> "$PASTA_CRLF/dados/cinemas.txt"
printf 'cc2, Ruim, 1, 2, dez, tt0000001\r\n'              >> "$PASTA_CRLF/dados/cinemas.txt"
printf 'cc3, FilmeFantasma, 1, 2, 9.00, tt9999999\r\n'    >> "$PASTA_CRLF/dados/cinemas.txt"
SAIDA=$(cd "$PASTA_CRLF" && printf '1\n1\nG\n\\\\N\n4\n2001\n\n5\n2001\n\n0\n' | "$EXE_ABSOLUTO" 2>&1 | tr -d '\r')
mapfile -t T < <(totais "$SAIDA")
conferir "arquivos com final de linha do Windows: 2 filmes validos (1 e 4) + 1 sem ano (2)" "3" "$(grep -oE 'Filmes carregados: [0-9]+' <<<"$SAIDA" | grep -oE '[0-9]+$')"
conter "linhas com defeito sao puladas e avisadas (2 filmes, 1 cinema)" "$SAIDA" "filmes: 2, cinemas: 1"
conferir "genero \\N vira lista vazia mesmo com final de linha do Windows" "0" "${T[0]:-}"
conferir "ano 2001 acha os 2 filmes validos" "2" "${T[1]:-}"
conferir "cinema com filme que nao existe: so o cinema bom aparece" "1" "${T[2]:-}"

escrever ""
escrever "--- 6. Casos em cascata (varias consultas na mesma sessao) ---"
SAIDA=$(rodar "3\n90\n120\n3\n90\n120\n3\n90\n105\n3\n106\n120\n0\n")
mapfile -t T < <(totais "$SAIDA")
conferir "C1 duracao 90..120 repetida da o mesmo total" "${T[0]}" "${T[1]}"
conferir "C1 segunda vez vem do cache" "sim" "$(grep -oE 'Veio do cache: (sim|nao)' <<<"$SAIDA" | sed -n 2p | awk '{print $4}')"
conferir "C1 90..105 + 106..120 = 90..120" "${T[0]}" "$(( ${T[2]} + ${T[3]} ))"

SAIDA=$(rodar "4\n2000\n2019\n4\n2000\n2009\n4\n2010\n2019\n0\n")
mapfile -t T < <(totais "$SAIDA")
conferir "C2 ano 2000..2009 + 2010..2019 = 2000..2019" "${T[0]}" "$(( ${T[1]} + ${T[2]} ))"

SAIDA=$(rodar "5\n1990\n2010\n5\n2000\n2000\n5\n1990\n2010\n0\n")
mapfile -t T < <(totais "$SAIDA")
conferir "C3 cinemas de 1990..2010 (repetida) mesmo total" "${T[0]}" "${T[2]}"
if [ "${T[1]}" -le "${T[0]}" ]; then registrar OK "C3 cinemas do ano 2000 <= cinemas de 1990..2010" "<= ${T[0]}" "${T[1]}"
else registrar FALHOU "C3 cinemas do ano 2000 <= cinemas de 1990..2010" "<= ${T[0]}" "${T[1]}"; fi

SAIDA=$(rodar "4\n2000\n2000\n5\n2000\n2000\n0\n")
conferir "C4 cinemas do ano 2000 nao reaproveitam cache de filmes" "nao" "$(grep -oE 'Veio do cache: (sim|nao)' <<<"$SAIDA" | sed -n 2p | awk '{print $4}')"

SAIDA=$(rodar "1\n2\nT\nmovie\nG\nComedy\nE\n4\n2000\n2009\n1\n2\nT\nmovie\nG\nComedy\nE\n0\n")
mapfile -t T < <(totais "$SAIDA")
conferir "C5 modulo 1 e modulo 2 na mesma sessao: movie E Comedy repete" "${T[0]}" "${T[2]}"
conferir "C5 segunda busca de movie E Comedy vem do cache" "sim" "$(grep -oE 'Veio do cache: (sim|nao)' <<<"$SAIDA" | sed -n 3p | awk '{print $4}')"

escrever ""
escrever "--- 7. Volume: centenas de consultas em uma unica sessao ---"
awk -v seed=2024 -v arqAno=build/cont_ano.txt -v arqDur=build/cont_dur.txt '
    function soma(v, lo, hi,    k, n) { n=0; for (k in v) if (k+0>=lo && k+0<=hi) n+=v[k]; return n }
    BEGIN {
        while ((getline l < arqAno) > 0) { split(l, p, " "); ano[p[1]]=p[2] }
        while ((getline l < arqDur) > 0) { split(l, p, " "); dur[p[1]]=p[2] }
        srand(seed)
        for (a=1880; a<=2030; a++)        consulta("4", "filmes ano " a, a, a, soma(ano, a, a))
        for (d=0; d<=150; d++)            consulta("3", "filmes duracao " d, d, d, soma(dur, d, d))
        for (i=0; i<150; i++) {
            lo=int(rand()*200); hi=lo+int(rand()*120)
            consulta("3", "filmes duracao " lo ".." hi, lo, hi, soma(dur, lo, hi))
        }
        for (i=0; i<100; i++) {
            lo=1885+int(rand()*140); hi=lo+int(rand()*30)
            consulta("4", "filmes ano " lo ".." hi, lo, hi, soma(ano, lo, hi))
        }
        print "0" > "build/volume_entrada.txt"
    }
    function consulta(opcao, nome, lo, hi, esperado) {
        printf "%s\n%s\n%s\n", opcao, lo, hi > "build/volume_entrada.txt"
        printf "%s\t%s\n", nome, esperado > "build/volume_esperado.txt"
    }' /dev/null
QTD=$(wc -l < build/volume_esperado.txt)
INICIO=$(date +%s.%N)
SAIDA=$(timeout 300 "$EXE" < build/volume_entrada.txt 2>&1 | tr -d '\r')
FIM=$(date +%s.%N)
mapfile -t T < <(totais "$SAIDA")
mapfile -t MS < <(tempos "$SAIDA")
CARGA=$(grep -oE 'Tempo de carregamento: [0-9.e+-]+' <<<"$SAIDA" | grep -oE '[0-9.e+-]+$')
printf 'consulta\tesperado\tobtido\tms\tsituacao\n' > "$VOLUME_TSV"
VOLUME_FALHAS=0
i=0
while IFS=$'\t' read -r nome esperado; do
    obtido=${T[$i]:-faltou}
    situacao=OK
    if [ "$obtido" != "$esperado" ]; then situacao=FALHOU; VOLUME_FALHAS=$((VOLUME_FALHAS + 1)); fi
    printf '%s\t%s\t%s\t%s\t%s\n' "$nome" "$esperado" "$obtido" "${MS[$i]:-?}" "$situacao" >> "$VOLUME_TSV"
    i=$((i + 1))
done < build/volume_esperado.txt
ESTATISTICA=$(awk -F'\t' 'NR>1 && $4!="?" {n++; s+=$4; if(min==""||$4<min)min=$4; if($4>max)max=$4; v[n]=$4}
    END{ printf "consultas=%d total=%.2fms media=%.4fms minimo=%.4fms maximo=%.4fms", n, s, s/n, min, max }' "$VOLUME_TSV")
escrever "$QTD consultas de filmes | sessao inteira: $(awk -v a="$INICIO" -v b="$FIM" 'BEGIN{printf "%.1f s", b-a}') | carregamento: ${CARGA} s"
escrever "tempo de busca -> $ESTATISTICA"
escrever "detalhe de cada consulta: $VOLUME_TSV"
conferir "volume: $QTD consultas de filmes conferidas com awk" "0 erros" "$VOLUME_FALHAS erros"
conferir "volume: nenhuma consulta ficou sem resposta" "$QTD" "${#T[@]}"

escrever ""
escrever "--- 7b. Volume de cinemas: um ano por vez ---"
awk -F'\t' -v arquivoCinemas="$CINEMAS" '
    NR>1 && NF>=9 && $6 ~ /^[0-9]+$/ { ano[$1]=$6+0 }
    END {
        while ((getline linha < arquivoCinemas) > 0) {
            linhaNum++
            if (linhaNum==1) continue
            gsub(/\r/, "", linha)
            n=split(linha, p, ",")
            delete vistos
            for (i=6; i<=n; i++) { id=p[i]; gsub(/ /, "", id); if (id in ano) vistos[ano[id]]=1 }
            for (a in vistos) cont[a]++
        }
        for (a=1880; a<=2030; a++) {
            printf "5\n%d\n%d\n", a, a > "build/volume_cinemas_entrada.txt"
            printf "cinemas ano %d\t%d\n", a, cont[a]+0 > "build/volume_cinemas_esperado.txt"
        }
        print "0" > "build/volume_cinemas_entrada.txt"
    }' "$FILMES"
SAIDA=$(timeout 300 "$EXE" < build/volume_cinemas_entrada.txt 2>&1 | tr -d '\r')
mapfile -t T < <(totais "$SAIDA")
mapfile -t MS < <(tempos "$SAIDA")
i=0; VOLUME_FALHAS=0
while IFS=$'\t' read -r nome esperado; do
    obtido=${T[$i]:-faltou}
    situacao=OK
    if [ "$obtido" != "$esperado" ]; then situacao=FALHOU; VOLUME_FALHAS=$((VOLUME_FALHAS + 1)); fi
    printf '%s\t%s\t%s\t%s\t%s\n' "$nome" "$esperado" "$obtido" "${MS[$i]:-?}" "$situacao" >> "$VOLUME_TSV"
    i=$((i + 1))
done < build/volume_cinemas_esperado.txt
ESTATISTICA=$(awk -F'\t' '$1 ~ /^cinemas/ && $4!="?" {n++; s+=$4; if(min==""||$4<min)min=$4; if($4>max)max=$4}
    END{ printf "consultas=%d total=%.2fms media=%.4fms minimo=%.4fms maximo=%.4fms", n, s, s/n, min, max }' "$VOLUME_TSV")
escrever "tempo de busca (cinemas) -> $ESTATISTICA"
conferir "volume: 151 consultas de cinemas conferidas com awk" "0 erros" "$VOLUME_FALHAS erros"

escrever ""
escrever "=== Resultado: $((TOTAL - FALHAS))/$TOTAL verificacoes OK, $FALHAS falhas ==="
[ "$FALHAS" -eq 0 ]
