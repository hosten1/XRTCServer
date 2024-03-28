package main

// import "fmt"

import (
	"fmt"
	"flag"  // 导入flag包
	"test/src/framework"
	"test/src/glog"
)
func main(){
	flag.Parse()
	errInit := framework.Init()
	if errInit != nil {
		panic(errInit)
	}
	glog.Info("Init success")
	glog.Flash()
	errHttp := framework.StartHttp()
	if errHttp != nil {
		panic(errHttp)
	}
	fmt.Println("HTTP server started successfully!")
}