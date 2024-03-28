package framework


import (
	"math/rand"
	"time"
	"test/src/glog"
)

func init(){
	rand.Seed(time.Now().Unix())
}
func getLogId32() uint32{
	return rand.Uint32()
}

type commonLog struct {
	
}

func (l *commonLog) Debugf(format string,args ...interface{}){
	glog.Debugf(format,args...)
}
func (l *commonLog) Infof(format string,args ...interface{}){
	glog.Infof(format,args...)
}
func (l *commonLog) Warningf(format string,args ...interface{}){
	glog.Warningf(format,args...)
}