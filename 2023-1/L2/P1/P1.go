package main

import (
	"fmt"
	"os"
	"sync"
)

// Codigo: 20203371
// Nombre: Gabriel Omar Durán Ruiz
var (
	count = 0
	wg    sync.WaitGroup
	chA   chan struct{} = make(chan struct{})
	chB   chan struct{} = make(chan struct{})
	chC   chan struct{} = make(chan struct{})
	chD   chan struct{} = make(chan struct{})
	chE   chan struct{} = make(chan struct{})
	chAux chan struct{} = make(chan struct{})
)

func admin() {
	//Codigo de administrador
	var cadena string
	wg.Wait()
	for {
		for i := 1; i < len(os.Args); i++ {
			cadena = os.Args[i]
			switch cadena {
			case "A":
				chA <- struct{}{}
			case "B":
				chB <- struct{}{}
			case "C":
				chC <- struct{}{}
			case "D":
				chD <- struct{}{}
			case "E":
				chE <- struct{}{}
			}
			<-chAux
			count++

		}
	}
}

func worker1() {
	wg.Done()
	for {
		<-chA
		fmt.Printf("A")
		chAux <- struct{}{}
	}
}

func worker2() {
	wg.Done()
	for {
		<-chB
		fmt.Printf("B")
		chAux <- struct{}{}
	}

}

func worker3() {
	wg.Done()
	for {
		<-chC
		fmt.Printf("C")
		chAux <- struct{}{}
	}

}

func worker4() {
	wg.Done()
	for {
		<-chD
		fmt.Printf("D")
		chAux <- struct{}{}
	}

}

func worker5() {
	wg.Done()
	for {
		<-chE
		fmt.Printf("E")
		chAux <- struct{}{}
	}

}

func main() {
	//var cadena string
	/*for i:= 1;i<len(os.Args); i++{
		cadena= os.Args[i]
		switch cadena{
			case "A":
				fmt.Println(cadena)
			case "B":
				fmt.Println(cadena)
			case "C":
				fmt.Println(cadena)
			case "D":
				fmt.Println(cadena)
			case "E":
				fmt.Println(cadena)
		}
	}*/
	wg.Add(5)
	go worker1()
	go worker2()
	go worker3()
	go worker4()
	go worker5()
	go admin()

	var input string
	fmt.Scanln(&input)
}
