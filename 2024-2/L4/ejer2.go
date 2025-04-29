package main

import (
	"fmt"
	"sync"
	"time"
)
//Diego Castilla ⚓
var wg sync.WaitGroup
var ch1 = make(chan struct{})
var ch2 = make(chan struct{})


func A() {
	for x := 0; x < 100; x++ {
		fmt.Printf("a1")
		ch1 <- struct{}{}
		<- ch2
		fmt.Printf("a2")
		time.Sleep(1 * time.Millisecond) //Dormimos en ambos lados para evitar secuencias que no deseamos
	}
	wg.Done()
}

func B() {
	for x := 0; x < 100; x++ {
		
		fmt.Printf("b1")
		<- ch1
		ch2 <- struct{}{}
		fmt.Printf("b2")
		time.Sleep(1 * time.Millisecond)
		
	}
	wg.Done()
}

func main() {
	wg.Add(2)
	go A()
	go B()
	wg.Wait()
	fmt.Println()
}
