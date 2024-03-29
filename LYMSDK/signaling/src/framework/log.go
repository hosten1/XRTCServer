package framework


import (
	"math/rand"
	"time"
	"test/src/glog"
	"fmt"
)

func init(){
	rand.Seed(time.Now().Unix())
}
func getLogId32() uint32{
	return rand.Uint32()
}


type logItem struct {
	field string
	value string
}


type TimeItem struct {
	field 		string
	begingTime 	int64
	endTime 	int64
}

type CommonLog struct {
	mainLog []logItem
	timeLogs []TimeItem
}

func (l *CommonLog) AddNotice(field,value string){
   item := logItem{
	field:field,
	value:value,
   }
   l.mainLog = append(l.mainLog,item)
}

func (l *CommonLog) TimeBegin(field string)  {
	item := TimeItem{
		field:field,
		begingTime:time.Now().UnixNano()/1000,
	}
	l.timeLogs = append(l.timeLogs,item)
}
func  (l *CommonLog) TimeEnd(field string){
	for i,item := range l.timeLogs {
		if item.field == field{
			l.timeLogs[i].endTime = time.Now().UnixNano()/1000
			break
		}
	}
}

func (l *CommonLog) GetPrefixLog() string{
	prefixLog := ""

	for _, item := range l.mainLog {
		prefixLog += fmt.Sprintf("%s[%s]",item.field,item.value)		
	}
	for _.timeItem := range l.timeLogs {
		diff = timeItem.endTime - timeItem.begingTime
		if diff < 0 {
			continue
		}
		fdiff := float64(diff)/1000.0
		prefixLog += fmt.Sprintf("%s[%.3fms]",item.field,fdiff)
	}
	return prefixLog
}


func (l *CommonLog) Errorf(format string,args ...interface{}){
	totalLog := l.GetPrefixLog() + format
	glog.Errorf(totalLog,args...)
}
func (l *CommonLog) Debugf(format string,args ...interface{}){
	totalLog := l.GetPrefixLog() + format
	glog.Debugf(totalLog,args...)
}
func (l *CommonLog) Infof(format string,args ...interface{}){
	totalLog := l.GetPrefixLog() + format
	glog.Infof(totalLog,args...)
}
func (l *CommonLog) Warningf(format string,args ...interface{}){
	totalLog := l.GetPrefixLog() + format
	glog.Warningf(totalLog,args...)
}