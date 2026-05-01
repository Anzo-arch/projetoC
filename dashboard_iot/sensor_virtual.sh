#!/bin/bash
while true; do
    TEMP=$((RANDOM % 21 + 15))
    TEMP_DEC=$((RANDOM % 10))
    UMID=$((RANDOM % 61 + 30))
    echo "T:${TEMP}.${TEMP_DEC},U:${UMID}"
    sleep 1
done
