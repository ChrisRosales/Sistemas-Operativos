#!/bin/bash

#Indico la carpeta donde se encuentran los archivos
carpeta="/home/alulab/Escritorio/archivos - pregunta 2"

patron="$1"

for archivo in "$carpeta"/*; do
  if [ -f "$archivo" ]; then
      nombre=$(basename "$archivo")
      nuevo_nombre="${nombre//$patron}" #quito el patron del nombre del archivo

      
      mv "$archivo" "$carpeta/$nuevo_nombre" #hago el reemplazo
  fi
  
done      
