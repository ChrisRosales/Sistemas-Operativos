#!/bin/bash


while read LINE; 
do
  
  cadena=$LINE
  
  cadena=( $cadena )
  cadena="${cadena[@]^}" #Le pongo mayúsculas a cada palabra

  num=${cadena:0:6} #extraigo el numero
    
  inicio="2020"??  #creo una cadena de la forma del numero 
  
  
  cadena=${cadena#$inicio}  #borro el numero del inicio
  echo "$cadena $num"

done <"$1"  #Ingreso el archivo como parámetro
 
