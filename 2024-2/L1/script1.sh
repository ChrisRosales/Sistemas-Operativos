#!/bin/bash

# 20201865 Kevin Luis Levano Cuzcano
# Función para saber si un número llega a ser un PRIMO:
esPrimo() {
    local num=$1
    if [[ $num -lt 2 ]]; then
        return 1
    fi
    for ((i=2; i*i<=num; i++)); do
        if ((num % i == 0)); then
            return 1
        fi
    done
    return 0
}

# Verificar si la cantidad de mi INPUT es 2:
if [ $# -ne 2 ]; then
    echo "Uso: $0 n1 n2"
    exit 1
fi

#Ingreso estos valores:
n1=$1
n2=$2

for ((num=n1; num<=n2; num++)); do
    echo -n "$num"
   
    if esPrimo $num; then
        echo " (Es Primo)"
    else
        echo ""
    fi
done
