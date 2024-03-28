package framework


import (
	"test/src/glog"
)

func init() error {
	glog.SetLogDic("./log")
	glog.SetLogFileName("lymrtc.log");
	return nil
}