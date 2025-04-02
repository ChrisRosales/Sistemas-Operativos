#!/bin/bash

# 20201865 Kevin Luis Levano Cuzcano
#Indico la carpeta donde se encuentran los archivos
carpeta="/home/osboxes/Desktop/LABORATORIO"

patron="$1"

for archivo in "$carpeta"/*; do
  if [ -f "$archivo" ]; then
      nombre=$(basename "$archivo")
      nuevo_nombre="${nombre//$patron}" #Quito el patron del nombre del archivo
      mv "$archivo" "$carpeta/$nuevo_nombre" #Hago el reemplazo
  fi
  
done      
