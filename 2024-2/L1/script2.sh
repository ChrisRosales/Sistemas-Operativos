#!/bin/bash

# 20201865 Kevin Luis Levano Cuzcano
# Hago el proceso linea por linea:
nombreGuardar="Lista2.txt"

while read LINE; 
do
  
  cadena=$LINE
  num=${cadena:0:8}
  aux=${cadena:0:9}
  cadena=${cadena#$aux}

  IFS=' ' read -r -a array <<< "$cadena"

  variable=${array[0]}
  nuevoVariable="${variable// /}"

  #Ahora el formateo de los nombres:
  primerNombre=( "${array[2],,}" )
  primerNombre=( "${primerNombre^}" )
  

  segundoNombre=( "${array[3],,}" )
  segundoNombre=( "${segundoNombre^}" )
  # Para el correo:
  nombreCorreo=$primerNombre
  nombreCorreo=( "${nombreCorreo,,}" )
  
  apellidoCorreo=${array[0]} 
  apellidoCorreo=( "${apellidoCorreo,,}" )

  echo "$num:${array[0]} ${array[1]} $primerNombre $segundoNombre:$nombreCorreo.$apellidoCorreo@pucp.edu.pe" >> "$nombreGuardar"

done <"$1"  #Ingreso el archivo como parámetro
 
