package main

// import "fmt"

import (
	"fmt"
	"test/src/framework"
)
func main(){
	errInit := framework.Init()
	if errInit != nil {
		panic(errInit)
	}
	errHttp := framework.StartHttp()
	if errHttp != nil {
		panic(errHttp)
	}
	fmt.Println("HTTP server started successfully!")
}