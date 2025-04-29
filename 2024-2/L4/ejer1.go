package main

import (
	"fmt"
	"sync"
	"time"
)
//Diego Castilla ⚓
var wg sync.WaitGroup

func worker1() {
	for x := 0; x < 10; x++ {
		time.Sleep(10 * time.Millisecond) //Establecems un tiempo para no tener secuencias raras
		fmt.Printf("I like to ")
		time.Sleep(200 * time.Millisecond)  //Dormimos para que se sincronize con la otra rutina
		fmt.Println("systems")
	}
	wg.Done()
}

func worker2() {
	for x := 0; x < 10; x++ {
		time.Sleep(210 * time.Millisecond) //Dormimos por el tiempo esperado total de la otra rutina
		fmt.Printf("study ")
		fmt.Printf("operating ")
	}
	wg.Done()
}

func main() {
	wg.Add(2)
	go worker1()
	go worker2()
	wg.Wait()
}

