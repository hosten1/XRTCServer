package framework


import (
	"test/src/glog"
	"fmt"
)

var gconf *FrameworkConf
func Init(confFile string) error {
	var err error
	gconf,err = LoadConf(confFile)
	if err != nil {
		return err
	}
    fmt.Printf("conf:%+v\n",gconf)
	
	glog.SetLogDic("./log")
	glog.SetLogFileName("lymrtc");
	glog.SetAlsoLogToStderr(true)
	glog.SetLogLevel("DEBUG")
	// glog.setlo
	return nil
}