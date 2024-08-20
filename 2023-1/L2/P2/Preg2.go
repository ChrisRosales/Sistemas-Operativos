package main

import (
	"fmt"
	"sync"
	"time"
)

var wgProducers sync.WaitGroup
var wgConsumers sync.WaitGroup
var totalProduced, totalConsumed int64

func main() {
	ch := make(chan int, 100)

	// Productores
	wgProducers.Add(3)
	for i := 1; i <= 3; i++ {
		go producer(ch, i)
	}

	// Consumidores
	wgConsumers.Add(2)
	for i := 1; i <= 2; i++ {
		go consumer(ch, i)
	}

	wgProducers.Wait()
	close(ch) // Cierre del canal después de la producción
	wgConsumers.Wait()
	fmt.Printf("Total Produced: %d, Total Consumed: %d\n", totalProduced, totalConsumed)
	if totalProduced == totalConsumed {
		fmt.Println("Success: All produced items have been consumed.")
	} else {
		fmt.Println("Error: Mismatch in produced and consumed items.")
	}
}

func producer(ch chan<- int, id int) {
	defer wgProducers.Done()
	for i := 0; i < 100; i++ {
		fmt.Printf("Productor %d: envía %v\n", id, id*100+i)
		ch <- id*100 + i
		time.Sleep(10 * time.Millisecond) // Retardo para visualización
		totalProduced++
	}
}

func consumer(ch <-chan int, id int) {
	defer wgConsumers.Done()
	for val := range ch {
		fmt.Printf("Consumidor %d: lee %v\n", id, val)
		time.Sleep(5 * time.Millisecond) // Retardo para visualización
		totalConsumed++
	}
}
