package main

import(
	"fmt"
	"sync"
	"time"
)

//punteros, aplica tu lp1
func routina(arreglo *[500]int, mutex *sync.Mutex, wg *sync.WaitGroup, numRoutina int){
	defer wg.Done() //indicamos que el hilo termina cuando termina la funcion,eso hace el defer
	//podria ir wg.Done() solo pero es una buena practica hacer lo de arriba
	var cantEscritas int = 0
	for i:=0; i<500;i++{
		mutex.Lock()
		
		if(arreglo[i]==-1){
			arreglo[i]=numRoutina;
			cantEscritas++
			if cantEscritas == 25{
				mutex.Unlock() //no me olvido de liberarlo sino fuentes
				break;
			}
		}
		mutex.Unlock()
		time.Sleep(1*time.Millisecond)//simulo una demora, mi pc lo hace altok asique tengo que hacer esto xD
	}

}
func main(){
	var wg sync.WaitGroup
	var mutex  sync.Mutex

	var arreglo [500]int
	for i:=0; i < 500;i++{
		arreglo[i]=-1

	}
	for i:=0; i<20; i++{
		wg.Add(1)
		go routina(&arreglo,&mutex,&wg,i) //aca con & mutex y wg siempre, el arreglo depende de lo q quieras hacer, 
		//en este caso le pasa el arreglo no una copia del arreglo
	}
	wg.Wait()
	fmt.Println(arreglo)
	//comprobamos
	var count int = 0
	var comprobar int = 1
	for i:=0 ; i<500;i++{
		if arreglo[i] == comprobar{
			count++
		}
	}
	fmt.Printf("La cantidad del valor %d encontrado en el arreglo es %d\n",comprobar,count)
}