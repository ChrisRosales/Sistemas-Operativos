//20222999 - Mulatillo Gomez, Ariana Camila
package main
import (
	"fmt"
	"sync"
)

var(
   wg sync.WaitGroup
   cha= make(chan int,  1)
   chb= make(chan int,  1)
   chc= make(chan int,  1)
   bandera bool
//la bandera es true cuando estoy de ida, false cuando estoy de regreso
)
func worker1() {
	for {
        <-cha
		fmt.Printf("A")
        if bandera{
            chb<-1
        }else{
            bandera=true
            cha<-1
        }
	}
	wg.Done()
}

func worker2() {
	for {
        <-chb
		fmt.Printf("B")
        if bandera{
            chc<-1
        }else{
            cha<-1
        }
	}
	wg.Done()
}

func worker3() {
	for {
        <-chc
		fmt.Printf("C")
        if bandera{
           bandera=false
            chc<-1
        }else{
           chb<-1
            
        }
	}
	wg.Done()
}

func main() {
    bandera= true
    cha<-1
	wg.Add(3)
	go worker1()
	go worker2()
	go worker3()
	wg.Wait()
	fmt.Printf("\n")
}
