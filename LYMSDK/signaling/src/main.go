package main

// import "fmt"

import (
	// "fmt"
	"flag"  // 导入flag包
	"test/src/framework"
	"test/src/glog"
)
func main(){
	flag.Parse()
	errInit := framework.Init("./conf/framwork.conf")
	if errInit != nil {
		panic(errInit)
	}
	// 协程的方式启动 http 和 https服务
    go StartHttp()
    StartHttps()
	
}
func StartHttp(){
	errHttp := framework.StartHttp()
	if errHttp != nil {
		glog.Info("HTTP server started error!")
		panic(errHttp)
	}
}
func StartHttps(){
	errHttps := framework.StartHttps()
	if errHttps != nil {
		glog.Info("HTTPS server started error!")
		panic(errHttps)
	}
}