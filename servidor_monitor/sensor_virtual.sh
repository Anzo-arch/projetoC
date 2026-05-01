#!/bin/bash
# Simula um sensor enviando temperatura (15-35 °C) e umidade (30-90 %) a cada 1s
while true; do
    TEMP=$(echo "scale=1; $RANDOM/32767*20+15" | bc -l)   # faixa 15.0 a 35.0
    UMID=$((RANDOM % 61 + 30))                             # faixa 30 a 90
    echo "T:${TEMP},U:${UMID}"
    sleep 1
done
