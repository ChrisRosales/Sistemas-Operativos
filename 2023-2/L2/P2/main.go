package main

import (
	"fmt"
	"math/rand"
	"sync"
	"time"
)

var wg sync.WaitGroup
var mutex sync.Mutex
var score int = 0

func moverse(arreglo *[]int, idRana int) {
	var posRana int
	for i := 0; i < len(*arreglo); i++ {
		if (*arreglo)[i] == idRana {
			posRana = i
			break
		}
	}
	//PRIMEROO EVALUO SI ESTOY DENTRO DEL RANGO, ES DECIR EL LADO IZQ DEL AND
	//si sale false, entonce no evaluaria el derecho de and y ya no se cairia
	//porque le pongo (*arreglo), tiene algo que ver con indexear
	//Si sale /main.go:46:34: invalid operation: cannot index arreglo (variable of type *[]int)
	// HACES ESA JUGADA

	//
	//if posRana + 2 < len(*arreglo) && (*arreglo)[posRana + 1] != -1 && (*arreglo)[posRana+2] == -1 {
	//		(*arreglo)[posRana + 2] =idRana
	//		(*arreglo)[posRana] = -1
	//	}else{
	//		if posRana - 2 >=0 && (*arreglo)[posRana - 1] != -1 && (*arreglo)[posRana - 2] == -1{
	//			(*arreglo)[posRana - 2] =idRana
	//			(*arreglo)[posRana] = -1
	//		}else{
	//			if posRana + 1 < len(*arreglo) && (*arreglo)[posRana + 1] == -1{
	//				(*arreglo)[posRana + 1] = idRana
	//				(*arreglo)[posRana] = -1
	//			}else{
	//				if posRana - 1 >= 0 && (*arreglo)[posRana - 1] == -1 {
	//					(*arreglo)[posRana-1] = idRana
	//					(*arreglo)[posRana] = -1
	//				}else{
	//
	//				}
	//			}
	//		}
	//
	//	}
	rand.Seed(time.Now().UnixNano()) // Usar el tiempo actual como semilla
	//VAMOS A RANDOMIZAR LAS JUGADAS
	// Obtener un número aleatorio entre 0 y 4
	num := rand.Intn(5) // Devuelve un número entre 0 y 4
	//PDST: SALIO USANDO ESTE METODO DE JUGADAS ALETORIAS,
	if num == 0 {
		//primero chequeo si es jugada valida sino me olvido
		//una vez se cumple lo de la izquierda es como volver a hacer otro if para la derecha
		if posRana+2 < len(*arreglo) && (*arreglo)[posRana+1] != -1 && (*arreglo)[posRana+2] == -1 {
			(*arreglo)[posRana+2] = idRana
			(*arreglo)[posRana] = -1
		}
	}
	if num == 1 {
		if posRana+1 < len(*arreglo) && (*arreglo)[posRana+1] != -1 && (*arreglo)[posRana+1] == -1 {
			(*arreglo)[posRana+1] = idRana
			(*arreglo)[posRana] = -1
		}
	}
	if num == 2 {
		if posRana-1 >= 0 && (*arreglo)[posRana-1] == -1 {
			(*arreglo)[posRana-1] = idRana
			(*arreglo)[posRana] = -1
		}
	}
	if num == 3 {
		if posRana-2 >= 0 && (*arreglo)[posRana-1] != -1 && (*arreglo)[posRana-2] == -1 {
			(*arreglo)[posRana-2] = idRana
			(*arreglo)[posRana] = -1
		}
	}
	if num == 4 {

	}
}

func asignarScore(arreglo *[]int, numRanasLado int) {
	//Inspiracion de APLICA
	for i := 0; i < numRanasLado; i++ {

		for j := numRanasLado + 1; j < 2*numRanasLado+1; j++ {
			if (*arreglo)[i] == j {
				score++
				break
			}
		}
	}

	for i := numRanasLado + 1; i < (numRanasLado*2)+1; i++ {

		for j := 1; j < numRanasLado+1; j++ {
			if (*arreglo)[i] == j {
				score++
				break
			}
		}
	}
}
func ranas(idRana int, numRanasLado int, arreglo *[]int) {
	defer wg.Done()

	for {

		mutex.Lock()
		//Solo consigo llegar a un maximo de 4 piezas, nose si en un  tiempo largo se encontrara la solucion
		//Comentario pre randomizada en la funcion moverse()
		if score == 6 {
			fmt.Println(arreglo)
			fmt.Printf("El score ahora es de %d piezas correctas\n\n", score)
			//mutex.Unlock()
			break
		}
		time.Sleep(5 * time.Millisecond)
		moverse(arreglo, idRana)
		score = 0
		asignarScore(arreglo, numRanasLado)
		fmt.Println(arreglo)
		fmt.Printf("El score ahora es de %d piezas correctas\n\n", score)

		mutex.Unlock()
		time.Sleep(5 * time.Millisecond)
	}
	mutex.Unlock()
}
func main() {

	var numRanas int = 6
	var tamArreglo int = numRanas + 1
	//creacion del arreglo
	//en GO se tiene que conocer el tamaño del arreglo antes de la compilacion
	//entonces tendremos que trabajar con slices (otro tipo de arreglo)
	arreglo := make([]int, tamArreglo)

	for i := 0; i < tamArreglo/2; i++ {
		arreglo[i] = i + 1
	}
	arreglo[tamArreglo/2] = -1
	for i := tamArreglo/2 + 1; i < tamArreglo; i++ {
		arreglo[i] = i
	}
	inicio := time.Now()
	for i := 1; i < numRanas+1; i++ {
		wg.Add(1)
		go ranas(i, numRanas/2, &arreglo)
	}

	wg.Wait()
	fmt.Println(arreglo)
	time.Sleep(1 * time.Second) //importante, nose porq, pero terminaba antes de imprimir el score 6
	fin := time.Now()
	duracion := fin.Sub(inicio)
	fmt.Printf("El proceso tomó %.2v\n", duracion)
	fmt.Printf("En segundos: %.2v segundos\n", duracion.Seconds())
}
