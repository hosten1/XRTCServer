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
	mainLog []logItem
}
type logItem struct {
	field string
	value string
}


func (l *commonLog) AddNotice(field,value string){
   item := logItem{
	field:field,
	value:value
}
   l.mainLog = append(l.mainLog,item)
}
func (l *commonLog) GetPrefixLog() string{
	prefixLog := ""
	for _, item range l.mainLog{
		prefixLog += fmt.Sprintf("%s[%s]",item.field,item.value)
		glog.Infof("%s:%s",item.field,item.value)
		
	}
	return prefixLog
}


func (l *commonLog) Errorf(format string,args ...interface{}){
	totalLog := l.GetPrefixLog() + format
	glog.Errorf(totalLog,args...)
}
func (l *commonLog) Debugf(format string,args ...interface{}){
	totalLog := l.GetPrefixLog() + format
	glog.Debugf(totalLog,args...)
}
func (l *commonLog) Infof(format string,args ...interface{}){
	totalLog := l.GetPrefixLog() + format
	glog.Infof(fortotalLogmat,args...)
}
func (l *commonLog) Warningf(format string,args ...interface{}){
	totalLog := l.GetPrefixLog() + format
	glog.Warningf(totalLog,args...)
}