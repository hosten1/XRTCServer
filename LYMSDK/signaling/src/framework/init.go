package framework


import (
	"test/src/glog"
)

func Init() error {
	glog.SetLogDic("./log")
	glog.SetLogFileName("lymrtc");
	glog.SetAlsoLogToStderr(true)
	glog.SetLogLevel("DEBUG")
	// glog.setlo
	return nil
}