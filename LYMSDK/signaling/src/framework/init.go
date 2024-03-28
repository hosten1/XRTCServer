package framework


import (
	"test/src/glog"
)

func init() error {
	glog.setLogDic("./log")
	glog.setLogFileName("lymrtc.log");
	return nil
}