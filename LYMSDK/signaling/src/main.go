package main

// import "fmt"

import (
	"fmt"
	"test/src/framework"
	"test/src/glog"
)
func main(){
	flag.Parse()
	errInit := framework.Init()
	if errInit != nil {
		panic(errInit)
	}
	glog.info("Init success")
	errHttp := framework.StartHttp()
	if errHttp != nil {
		panic(errHttp)
	}
	fmt.Println("HTTP server started successfully!")
}