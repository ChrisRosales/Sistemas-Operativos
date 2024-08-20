package main

import (
	"fmt"
	"sync"
)

var wg sync.WaitGroup
var puerta1 = make(chan int, 1)
var puerta2 = make(chan int, 1)
var puerta3 = make(chan int, 1)
var puerta4 = make(chan int, 1)
var puerta5 = make(chan int, 1)
var puerta6 = make(chan int, 1)
var condicion bool

func worker1() {
	for {
		fmt.Printf("A")
		puerta3 <- 1
		<-puerta1
	}
	wg.Done()
}

func worker2() {
	for {
		fmt.Printf("B")
		puerta4 <- 1
		<-puerta2
	}
	wg.Done()
}

func worker3() {
	for {
		<-puerta3
		<-puerta4
		fmt.Printf("C")
		puerta5 <- 1
	}
	wg.Done()
}

func worker4() {
	for {
		<-puerta5
		fmt.Printf("D")
		puerta6 <- 1
	}
	wg.Done()
}

func worker5() {
	for {
		<-puerta6
		fmt.Printf("E")
		if condicion {
			// En el texto indica que no se pueden usar variables locales
			// asi que se uso un boolean global para realizar el cambio en la secuencia
			// AB CDE BA CDE
			condicion = false
			puerta1 <- 1
			puerta2 <- 1
		} else {
			condicion = true
			puerta2 <- 1
			puerta1 <- 1
		}

	}
	wg.Done()
}

func main() {
	condicion = true
	wg.Add(5)
	go worker1()
	go worker2()
	go worker3()
	go worker4()
	go worker5()
	wg.Wait()
	fmt.Printf("\n")
}
