// main.go
package main

import (
	"fmt"
	"math/rand"
	"sync"
	"time"
)

var num [5]int
var cont int
var t int

func task(n int) {
	x := 0
	for {
		if x == 50 {
			break
		}
		<-puerta
		if num[n] == n {
			puerta <- 1
			continue
		}
		num[n] = n

		fmt.Printf("%d", n)
		cont++
		if cont == 5 {
			for y := 0; y < 5; y++ {
				num[y] = -1
			}
			cont = 0
			fmt.Println()
		}
		x++
		puerta <- 1
		t = rand.Intn(3)
		time.Sleep(time.Duration(t) * time.Millisecond)
	}
	wg.Done()
}

var wg sync.WaitGroup
var puerta = make(chan int, 1)

func main() {
	for x := 0; x < 5; x++ {
		num[x] = -1
	}
	cont = 0
	puerta <- 1
	for x := 0; x < 5; x++ {
		wg.Add(1)
		go task(x)
	}
	wg.Wait()
}
