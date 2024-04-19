package framework


import (
	"test/src/glog"
	// "fmt"
)

var gconf *FrameworkConf
func Init(confFile string) error {
	var err error
	gconf,err = LoadConf(confFile)
	if err != nil {
		return err
	}
    // fmt.Printf("conf:%+v\n",gconf)
	
	glog.SetLogDic(gconf.logDir)
	glog.SetLogFileName(gconf.logFile);
	glog.SetLogToStderr(gconf.logToStderr)
	glog.SetLogLevel(gconf.logLevel)
	// glog.setlo
	err = loadLrpc()
	if err != nil {
		return err
	}
	return nil
}