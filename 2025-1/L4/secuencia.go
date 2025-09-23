package main

import (
	"fmt"
	"math/rand"
	"sync"
	"time"
)

var wg sync.WaitGroup

func worker1() {
	for {
		rand.Seed(time.Now().UnixNano())
		fmt.Printf("A")
		slice := rand.Intn(300)
		time.Sleep(time.Duration(slice) * time.Millisecond)
		fmt.Printf("A")
	}
	wg.Done()
}

func worker2() {
	for {
		rand.Seed(time.Now().UnixNano())
		fmt.Printf("B")
		slice := rand.Intn(300)
		time.Sleep(time.Duration(slice) * time.Millisecond)
		fmt.Printf("B")
	}
	wg.Done()
}

func worker3() {
	for {
		rand.Seed(time.Now().UnixNano())
		fmt.Printf("C")
		slice := rand.Intn(300)
		time.Sleep(time.Duration(slice) * time.Millisecond)
		fmt.Printf("C")
	}
	wg.Done()
}

func main() {
	wg.Add(3)
	go worker1()
	go worker2()
	go worker3()
	wg.Wait()
	fmt.Printf("\n")
}
