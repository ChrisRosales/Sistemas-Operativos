//20222999 - Mulatillo Gomez, Ariana Camila
package main
import (
	"fmt"
	"math/rand"
	"sync"
	"time"
)

var(
  wg sync.WaitGroup
  ch1= make(chan int,  1)
  ch2= make(chan int, 1)
  ch3= make(chan int, 1)
  mu  sync.Mutex
) 

func worker1() {
	for { 
        <-ch1 
        mu.Lock()
		rand.Seed(time.Now().UnixNano())
		fmt.Printf("A")
		slice := rand.Intn(300)
		time.Sleep(time.Duration(slice) * time.Millisecond)
		fmt.Printf("A")
        ch2<-1   
        mu.Unlock()
	}
	wg.Done()
}

func worker2() {
	for {
        <-ch2
        mu.Lock()
		rand.Seed(time.Now().UnixNano())
		fmt.Printf("B")
		slice := rand.Intn(300)
		time.Sleep(time.Duration(slice) * time.Millisecond)
		fmt.Printf("B")     
        ch3<-1
        mu.Unlock()
	}
	wg.Done()
}

func worker3() {
	for {
        <-ch3
        mu.Lock()
		rand.Seed(time.Now().UnixNano())
		fmt.Printf("C")
		slice := rand.Intn(300)
		time.Sleep(time.Duration(slice) * time.Millisecond)
		fmt.Printf("C")    
        ch1<-1  
        mu.Unlock()
	}
	wg.Done()
}

func main() {
    ch1<-1
	wg.Add(3)
	go worker1()
	go worker2()
	go worker3()
	wg.Wait()
	fmt.Printf("\n")
}
