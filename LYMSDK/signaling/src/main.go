package main

// import "fmt"

import (
	"fmt"
	"test/src/framework"
)
func main(){
	err := framework.StartHttp()
	if err != nil {
		panic(err)
	}
	fmt.Println("HTTP server started successfully!")
}