package main

import (
	"fmt"
	"math/rand"
	"sync"
	"time"
)
//Diego Castilla ⚓
var (
	wg sync.WaitGroup
	t  int
)

func cook() {
	for {
		<- ch3 //Espera a que se solciten porciones
		putServingsInPot(5)
	}
	wg.Done()
}

func putServingsInPot(n int) {
	servings = 5
	fmt.Printf("Put Servings %d In Pot\n", n)
	t = rand.Intn(10) // n will be between 0 and 10
	time.Sleep(time.Duration(t) * time.Millisecond)
}

func getServingFromPot(n int) {
	fmt.Printf("%d: Get serving from Pot\n", n)
}
var ch3 = make(chan int)	
var mu sync.Mutex
var servings int = 0 //Cantidad de porciones

func eat(n int) {
	fmt.Printf("%d: eating...\n", n)
}

func savage(n int) {
	for {
		mu.Lock() //Colocamos el mutex para que no modifique el valor de las porciones de forma anomala
		if servings > 0 { //Si la olla tiene porciones, comerá
			getServingFromPot(n)
			eat(n)
			servings--
			t = rand.Intn(10) // n will be between 0 and 10
			time.Sleep(time.Duration(t) * time.Millisecond)
		} else{ //Si no hay porciones, llama al concinero
			ch3 <- 1
		}
		mu.Unlock()
	}
	wg.Done()
}

func main() {
	rand.Seed(time.Now().UnixNano())
	wg.Add(11)
	go cook()
	for x := 0; x < 10; x++ {
		go savage(x)
	}
	wg.Wait()
}
